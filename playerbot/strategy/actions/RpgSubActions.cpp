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
    OnExecute();

    bot->SetSelectionGuid(guidP());

    setFacingTo(guidP());
}

void RpgHelper::AfterExecute(bool doDelay, bool waitForGroup)
{
    OnExecute();

    bot->SetSelectionGuid(guidP());

    setFacingTo(guidP());

    if(doDelay)
        setDelay(waitForGroup);

    setFacing(guidP());
}

void RpgHelper::setFacingTo(GuidPosition guidPosition)
{
    bot->SetFacingTo(guidPosition.getAngleTo(bot)+ M_PI_F);
}

void RpgHelper::setFacing(GuidPosition guidPosition)
{
    if (!guidPosition.IsUnit())
        return;

    if (guidPosition.IsPlayer())
        return;

    Unit* unit = guidPosition.GetUnit();

    unit->SetFacingTo(unit->GetAngle(bot));
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
    uint32 type = TalkAction::GetRandomEmote(rpg->guidP().GetUnit());

    WorldPacket p1;
    p1 << rpg->guid();
    bot->GetSession()->HandleGossipHelloOpcode(p1);

    bot->HandleEmoteCommand(type);

    rpg->AfterExecute();

    return true;
}

bool RpgTaxiAction::Execute(Event event)
{
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
    GuidPosition guidP = rpg->guidP();

    uint32 node = sObjectMgr.GetNearestTaxiNode(guidP.getX(), guidP.getY(), guidP.getZ(), guidP.getMapId(), bot->GetTeam());

    if (!node)
        return false;

    Creature* flightMaster = bot->GetNPCIfCanInteractWith(guidP, UNIT_NPC_FLAG_FLIGHTMASTER);

    if (!flightMaster)
        return false;

    return bot->GetSession()->SendLearnNewTaxiNode(flightMaster);    
}

bool RpgHealAction::Execute(Event event)
{
    bool retVal = false;
    
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
    return retVal;
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

            if (bot->GetTradeData() && bot->GetTradeData()->HasItem(item->GetObjectGuid()))
                continue;

            ItemUsage otherUsage = PAI_VALUE2(ItemUsage, "item usage", item->GetEntry());

            if (std::find(myUsages.begin(), myUsages.end(), otherUsage) == myUsages.end())
                giveItems.push_back(item);
        }
    }

    return giveItems;
}

bool RpgTradeUsefulAction::Execute(Event event)
{
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

    bool hasTraded = ai->DoSpecificAction("trade", Event("rpg action", param.str().c_str()), true);

    if (hasTraded || bot->GetTradeData())
    {
        if (bot->GetTradeData() && bot->GetTradeData()->HasItem(item->GetObjectGuid()))
        {
            if (bot->GetGroup() && bot->GetGroup()->IsMember(guidP) && ai->HasRealPlayerMaster())
                ai->TellMasterNoFacing("You can use this " + chat->formatItem(item->GetProto()) + " better than me, " + guidP.GetPlayer()->GetName()/*chat->formatWorldobject(guidP.GetPlayer())*/ + ".");
            else
                bot->Say("You can use this " + chat->formatItem(item->GetProto()) + " better than me, " + player->GetName()/*chat->formatWorldobject(player)*/ + ".", (bot->GetTeam() == ALLIANCE ? LANG_COMMON : LANG_ORCISH));

            if (!urand(0, 4) || items.size() < 2)
            {
                //bot->Say("End trade with" + chat->formatWorldobject(player), (bot->GetTeam() == ALLIANCE ? LANG_COMMON : LANG_ORCISH));
                WorldPacket p;
                uint32 status = TRADE_STATUS_TRADE_ACCEPT;
                p << status;
                bot->GetSession()->HandleAcceptTradeOpcode(p);
            }
        }
        else
            //bot->Say("Start trade with" + chat->formatWorldobject(player), (bot->GetTeam() == ALLIANCE ? LANG_COMMON : LANG_ORCISH));

        ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
        return true;
    }

    return false;
}

bool RpgDuelAction::isUseful(Event event)
{
    // do not offer duel in non pvp areas
    if (sPlayerbotAIConfig.IsInPvpProhibitedZone(bot->GetAreaId()))
        return false;

    // Players can only fight a duel with each other outside (=not inside dungeons and not in capital cities)
    AreaTableEntry const* casterAreaEntry = GetAreaEntryByAreaID(bot->GetAreaId());
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