#include "botpch.h"
#include "../../playerbot.h"
#include "RpgSubActions.h"
#include "ChooseRpgTargetAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../values/PossibleRpgTargetsValue.h"
#include "../values/Formations.h"
#include "EmoteAction.h"
#include "GossipDef.h"
#include "GuildCreateActions.h"
#include "SocialMgr.h"


using namespace ai;

void RpgHelper::BeforeExecute()
{
    bot->SetSelectionGuid(guidP());

    setFacingTo(guidP());

    setFacing(guidP());
}

void RpgHelper::AfterExecute(bool doDelay, bool waitForGroup, string nextAction)
{
    if (ai->HasRealPlayerMaster() && nextAction == "rpg") 
        nextAction = "rpg cancel"; 
    
    SET_AI_VALUE(string, "next rpg action", nextAction);

    if(doDelay)
        setDelay(waitForGroup);
}

void RpgHelper::setFacingTo(GuidPosition guidPosition)
{
    //sServerFacade.SetFacingTo(bot, guidPosition.GetWorldObject());
    MotionMaster& mm = *bot->GetMotionMaster();
    bot->SetFacingTo(bot->GetAngle(guidPosition.GetWorldObject()));
    bot->m_movementInfo.RemoveMovementFlag(MovementFlags(MOVEFLAG_SPLINE_ENABLED | MOVEFLAG_FORWARD));
}

void RpgHelper::setFacing(GuidPosition guidPosition)
{
    if (!guidPosition.IsUnit())
        return;

    if (guidPosition.IsPlayer())
        return;

    Unit* unit = guidPosition.GetUnit();
    //sServerFacade.SetFacingTo(unit,bot);   

    MotionMaster& mm = *unit->GetMotionMaster();
    unit->SetFacingTo(unit->GetAngle(bot));
    unit->m_movementInfo.RemoveMovementFlag(MovementFlags(MOVEFLAG_SPLINE_ENABLED | MOVEFLAG_FORWARD));
}

void RpgHelper::resetFacing(GuidPosition guidPosition)
{
    if (!guidPosition.IsCreature())
        return;

    Creature* unit = guidPosition.GetCreature();

    CreatureData* data = guidPosition.GetCreatureData();

    if (data)
        unit->SetFacingTo(data->orientation);
}

void RpgHelper::setDelay(bool waitForGroup)
{
    if (!ai->HasRealPlayerMaster() || (waitForGroup && ai->GetGroupMaster() == bot && bot->GetGroup()))
        ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
    else
        ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay / 5);
}

bool RpgEmoteAction::Execute(Event event)
{
    rpg->BeforeExecute();

    Unit* unit = rpg->guidP().GetUnit();

    uint32 type;

    if (unit && unit->GetEntry() == 620)
    {
        type = TEXTEMOTE_CHICKEN;
        WorldPacket data(SMSG_TEXT_EMOTE);
        data << type;
        data << 1;
        data << rpg->guidP();
        bot->GetSession()->HandleTextEmoteOpcode(data);
    }
    else
        type = TalkAction::GetRandomEmote(rpg->guidP().GetUnit());

    WorldPacket p1;
    p1 << rpg->guid();
    bot->GetSession()->HandleGossipHelloOpcode(p1);

    bot->HandleEmoteCommand(type);

    if (type != TEXTEMOTE_CHICKEN)
        rpg->AfterExecute();
    else if(unit && !bot->GetNPCIfCanInteractWith(rpg->guidP(), UNIT_NPC_FLAG_QUESTGIVER) && AI_VALUE(TravelTarget*, "travel target")->getEntry() == 620)
        rpg->AfterExecute(true,false, "rpg emote");

    return true;
}

bool RpgTaxiAction::Execute(Event event)
{
    rpg->BeforeExecute();

    GuidPosition guidP = rpg->guidP();

    WorldPacket emptyPacket;
    bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);

    uint32 node = sObjectMgr.GetNearestTaxiNode(guidP.getX(), guidP.getY(), guidP.getZ(), guidP.getMapId(), bot->GetTeam());

    vector<uint32> nodes;
    for (uint32 i = 0; i < sTaxiPathStore.GetNumRows(); ++i)
    {
        TaxiPathEntry const* entry = sTaxiPathStore.LookupEntry(i);
        if (entry && entry->from == node && (bot->m_taxi.IsTaximaskNodeKnown(entry->to) || bot->isTaxiCheater()))
        {
            nodes.push_back(i);
        }
    }

    if (nodes.empty())
    {
        sLog.outError("Bot %s - No flight paths available", bot->GetName());
        return false;
    }

    uint32 path = nodes[urand(0, nodes.size() - 1)];
    uint32 money = bot->GetMoney();
    bot->SetMoney(money + 100000);

    TaxiPathEntry const* entry = sTaxiPathStore.LookupEntry(path);
    if (!entry)
        return false;

    TaxiNodesEntry const* nodeFrom = sTaxiNodesStore.LookupEntry(entry->from);
    TaxiNodesEntry const* nodeTo = sTaxiNodesStore.LookupEntry(entry->to);

    Creature* flightMaster = bot->GetNPCIfCanInteractWith(guidP, UNIT_NPC_FLAG_FLIGHTMASTER);
    if (!flightMaster)
    {
        sLog.outError("Bot %s cannot talk to flightmaster (%zu location available)", bot->GetName(), nodes.size());
        return false;
    }
    if (!bot->ActivateTaxiPathTo({ entry->from, entry->to }, flightMaster, 0))
    {
        sLog.outError("Bot %s cannot fly %u (%zu location available)", bot->GetName(), path, nodes.size());
        return false;
    }
    sLog.outString("Bot #%d <%s> is flying from %s to %s (%zu location available)", bot->GetGUIDLow(), bot->GetName(), nodeFrom->name[0], nodeTo->name[0], nodes.size());
    bot->SetMoney(money);

    rpg->AfterExecute();

    return true;
}


bool RpgDiscoverAction::Execute(Event event)
{
    rpg->BeforeExecute();

    GuidPosition guidP = rpg->guidP();

    uint32 node = sObjectMgr.GetNearestTaxiNode(guidP.getX(), guidP.getY(), guidP.getZ(), guidP.getMapId(), bot->GetTeam());

    if (!node)
        return false;

    Creature* flightMaster = bot->GetNPCIfCanInteractWith(guidP, UNIT_NPC_FLAG_FLIGHTMASTER);

    if (!flightMaster)
        return false;

    rpg->AfterExecute(true, true);

    return bot->GetSession()->SendLearnNewTaxiNode(flightMaster);    
}

bool RpgHealAction::Execute(Event event)
{
    bool retVal = false;

    rpg->BeforeExecute();
    
    switch (bot->getClass())
    {
    case CLASS_PRIEST:
        retVal = ai->DoSpecificAction("lesser heal on party", Event(), true);
        break;
    case CLASS_DRUID:
        retVal=ai->DoSpecificAction("healing touch on party", Event(), true);
        break;
    case CLASS_PALADIN:
        retVal=ai->DoSpecificAction("holy light on party", Event(), true);
        break;
    case CLASS_SHAMAN:
        retVal=ai->DoSpecificAction("healing wave on party", Event(), true);
        break;
    }

    rpg->AfterExecute(true, false);

    return retVal;
}

bool RpgTradeUsefulAction::IsTradingItem(uint32 entry)
{
    TradeData* trade = bot->GetTradeData();

    if (!trade)
        return false;

    for (uint8 i = 0; i < TRADE_SLOT_TRADED_COUNT; i++)
    {
        Item* tradeItem = trade->GetItem(TradeSlots(i));

        if (tradeItem && tradeItem->GetEntry() == entry)
            return true;;
    }

    return false;
}

list<Item*> RpgTradeUsefulAction::CanGiveItems(GuidPosition guidPosition)
{
    Player* player = guidPosition.GetPlayer();

    list<Item*> giveItems;

    if (ai->HasActivePlayerMaster() || !player->GetPlayerbotAI())
        return giveItems;

    list<ItemUsage> myUsages = { ITEM_USAGE_NONE , ITEM_USAGE_VENDOR, ITEM_USAGE_AH, ITEM_USAGE_DISENCHANT };

    for (auto& myUsage : myUsages)
    {
        list<Item*> myItems = AI_VALUE2(list<Item*>, "inventory items", "usage " + to_string(myUsage));
        myItems.reverse();

        for (auto& item : myItems)
        {
            if (!item->CanBeTraded())
                continue;

            TradeData* trade = bot->GetTradeData();

            if (trade)
            {

                if (trade->HasItem(item->GetObjectGuid())) //This specific item isn't being traded.
                    continue;

                if (IsTradingItem(item->GetEntry())) //A simular item isn't being traded.
                    continue;

                if (std::any_of(giveItems.begin(), giveItems.end(), [item](Item* i) {return i->GetEntry() == item->GetEntry(); })) //We didn't already add a simular item to this list.
                    continue;
            }

            ItemUsage otherUsage = PAI_VALUE2(ItemUsage, "item usage", item->GetEntry());

            if (std::find(myUsages.begin(), myUsages.end(), otherUsage) == myUsages.end())
                giveItems.push_back(item);
        }
    }

    return giveItems;
}

bool RpgTradeUsefulAction::Execute(Event event)
{
    rpg->BeforeExecute();

    GuidPosition guidP = AI_VALUE(GuidPosition, "rpg target");

    Player* player = guidP.GetPlayer();

    if (!player)
        return false;

    list<Item*> items = CanGiveItems(guidP);

    if (items.empty())
        return false;

    Item* item = items.front();

    ostringstream param;

    param << chat->formatWorldobject(player);
    param << " ";
    param << chat->formatItem(item->GetProto());

    if (ai->IsRealPlayer() && !bot->GetTradeData()) //Start the trade from the other side to open the window
    {
        WorldPacket packet(CMSG_INITIATE_TRADE);
        packet << bot->GetObjectGuid();
        player->GetSession()->HandleInitiateTradeOpcode(packet);
    }

    if (!IsTradingItem(item->GetEntry()))
        ai->DoSpecificAction("trade", Event("rpg action", param.str().c_str()), true);

    bool isTrading = bot->GetTradeData();

    if (isTrading)
    {
        if (IsTradingItem(item->GetEntry())) //Did we manage to add the item to the trade?
        {
            if (bot->GetGroup() && bot->GetGroup()->IsMember(guidP) && ai->HasRealPlayerMaster())
                ai->TellMasterNoFacing("You can use this " + chat->formatItem(item->GetProto()) + " better than me, " + player->GetName()/*chat->formatWorldobject(guidP.GetPlayer())*/ + ".");
            else
                bot->Say("You can use this " + chat->formatItem(item->GetProto()) + " better than me, " + player->GetName() + ".", (bot->GetTeam() == ALLIANCE ? LANG_COMMON : LANG_ORCISH));

            if (!urand(0, 4) || items.size() < 2) //Complete the trade if we have no more items to trade.
            {
                //bot->Say("End trade with" + chat->formatWorldobject(player), (bot->GetTeam() == ALLIANCE ? LANG_COMMON : LANG_ORCISH));
                WorldPacket p;
                uint32 status = TRADE_STATUS_TRADE_ACCEPT;
                p << status;
                bot->GetSession()->HandleAcceptTradeOpcode(p);
            }
        }
        //else
        //   bot->Say("Start trade with" + chat->formatWorldobject(player), (bot->GetTeam() == ALLIANCE ? LANG_COMMON : LANG_ORCISH));

        ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
    }

    rpg->AfterExecute(isTrading, true, isTrading ? "rpg trade useful" : "rpg");

    return isTrading;
}

bool RpgDuelAction::isUseful()
{
    // do not offer duel in non pvp areas
    if (sPlayerbotAIConfig.IsInPvpProhibitedZone(sServerFacade.GetAreaId(bot)))
        return false;

    // Players can only fight a duel with each other outside (=not inside dungeons and not in capital cities)
    AreaTableEntry const* casterAreaEntry = GetAreaEntryByAreaID(sServerFacade.GetAreaId(bot));
    if (casterAreaEntry && !(casterAreaEntry->flags & AREA_FLAG_DUEL))
    {
        // Dueling isn't allowed here
        return false;
    }

    return true;
}

bool RpgDuelAction::Execute(Event event)
{
    GuidPosition guidP = AI_VALUE(GuidPosition, "rpg target");

    Player* player = guidP.GetPlayer();

    if (!player)
        return false;

    return ai->DoSpecificAction("cast custom spell", Event("rpg action", chat->formatWorldobject(player) + " 7266"), true);
}

bool RpgMountAnimAction::isUseful()
{
    return AI_VALUE2(bool, "mounted", "self target") && !AI_VALUE2(bool, "moving", "self target");
}

bool RpgMountAnimAction::Execute(Event event)
{
    WorldPacket p;
    bot->GetSession()->HandleMountSpecialAnimOpcode(p);

    return true;
}
