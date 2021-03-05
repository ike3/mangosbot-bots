#include "botpch.h"
#include "../../playerbot.h"
#include "RpgAction.h"
#include "ChooseRpgTargetAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../values/PossibleRpgTargetsValue.h"
#include "EmoteAction.h"
#include "GossipDef.h"


using namespace ai;

bool RpgAction::Execute(Event event)
{    
    ObjectGuid guid = AI_VALUE(ObjectGuid, "rpg target");
    Creature* creature = ai->GetCreature(guid);
    Unit* target = ai->GetUnit(guid);
    if (!target)
        return false;

    if (sServerFacade.isMoving(bot))
        return false;

    if (bot->GetMapId() != target->GetMapId())
    {
        context->GetValue<ObjectGuid>("rpg target")->Set(ObjectGuid());
        return false;
    }

    if (!sServerFacade.IsInFront(bot, target, sPlayerbotAIConfig.sightDistance, CAST_ANGLE_IN_FRONT) && !bot->IsTaxiFlying() && !bot->IsFlying())
    {
        sServerFacade.SetFacingTo(bot, target, true);
        ai->SetNextCheckDelay(sPlayerbotAIConfig.globalCoolDown);
        return false;
    }

    if (!bot->GetNPCIfCanInteractWith(target->GetObjectGuid(), UNIT_NPC_FLAG_NONE))
    {
        context->GetValue<ObjectGuid>("rpg target")->Set(ObjectGuid());
        return false;
    }

    if (bot->GetShapeshiftForm() > 0)
        bot->SetShapeshiftForm(FORM_NONE);

    bool withPlayer = false;
    Player* master = bot->GetPlayerbotAI()->GetMaster();
    if (master && !master->GetPlayerbotAI())
        withPlayer = true;

    if (target->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_FLIGHTMASTER) && !withPlayer)
    {
        WorldPacket emptyPacket;
        bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
        taxi(target);
        return true;
    }
   
    vector<RpgElement> elements;
#ifdef MANGOS
    if (target->IsVendor())
        elements.push_back(&RpgAction::trade);
    if (bot->GetSession()->getDialogStatus(bot, target, DIALOG_STATUS_NONE) == DIALOG_STATUS_REWARD2 || bot->GetSession()->getDialogStatus(bot, target, DIALOG_STATUS_NONE) == DIALOG_STATUS_AVAILABLE)
        elements.push_back(&RpgAction::quest);
    if (target->IsArmorer() && needRepair())
        elements.push_back(&RpgAction::repair);
    if (creature && CanTrain(guid))
        elements.push_back(&RpgAction::train);
    if (target->HealthBelowPct(100) && (bot->getClass() == CLASS_PRIEST || bot->getClass() == CLASS_DRUID || bot->getClass() == CLASS_PALADIN || bot->getClass() == CLASS_SHAMAN))
        elements.push_back(&RpgAction::heal);
#endif
#ifdef CMANGOS
    if (target->isVendor())
        elements.push_back(&RpgAction::trade);
    if (bot->GetSession()->getDialogStatus(bot, target, DIALOG_STATUS_NONE) == DIALOG_STATUS_REWARD2 || bot->GetSession()->getDialogStatus(bot, target, DIALOG_STATUS_NONE) == DIALOG_STATUS_AVAILABLE)
        elements.push_back(&RpgAction::quest);
    if (target->isArmorer() && needRepair())
        elements.push_back(&RpgAction::repair);
    if (creature && CanTrain(guid))
        elements.push_back(&RpgAction::train);
    if (target->GetHealthPercent() < 100 && (bot->getClass() == CLASS_PRIEST || bot->getClass() == CLASS_DRUID || bot->getClass() == CLASS_PALADIN || bot->getClass() == CLASS_SHAMAN))
        elements.push_back(&RpgAction::heal);
#endif

    if (AddIgnore(target->GetObjectGuid()))
    {
        if (elements.empty() && !ChooseRpgTargetAction::isFollowValid(bot, target))
        {
            elements.push_back(&RpgAction::emote);
            elements.push_back(&RpgAction::stay);
            elements.push_back(&RpgAction::work);
        }
    }    
    else
        elements.push_back(&RpgAction::cancel);

    if (elements.empty())
        elements.push_back(&RpgAction::cancel);

    RpgElement element = elements[urand(0, elements.size() - 1)];
    (this->*element)(target);
    return true;
}

bool RpgAction::AddIgnore(ObjectGuid guid)
{
    if (HasIgnore(guid))
        return false;

    set<ObjectGuid>& ignoreList = context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Get();

    ignoreList.insert(guid);

    if (ignoreList.size() > 50)
        ignoreList.erase(ignoreList.begin());

    context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Set(ignoreList);

    return true;
}

bool RpgAction::RemIgnore(ObjectGuid guid)
{
    if (!HasIgnore(guid))
        return false;

    set<ObjectGuid>& ignoreList = context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Get();

    ignoreList.erase(ignoreList.find(guid));

    context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Set(ignoreList);

    return true;
}

bool RpgAction::HasIgnore(ObjectGuid guid)
{
    set<ObjectGuid>& ignoreList = context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Get();
    if (ignoreList.empty())
        return false;

    if (ignoreList.find(guid) == ignoreList.end())
        return false;

    return true;
}

void RpgAction::stay(Unit* unit)
{
    if (bot->PlayerTalkClass) bot->PlayerTalkClass->CloseGossip();
    ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

void RpgAction::work(Unit* unit)
{
    bot->HandleEmoteCommand(EMOTE_STATE_USESTANDING);
    ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

void RpgAction::emote(Unit* unit)
{
    uint32 type = TalkAction::GetRandomEmote(unit);

    ObjectGuid oldSelection = bot->GetSelectionGuid();

    bot->SetSelectionGuid(unit->GetObjectGuid());

    WorldPacket p1;
    p1 << unit->GetObjectGuid();
    bot->GetSession()->HandleGossipHelloOpcode(p1);

    bot->HandleEmoteCommand(type);
    unit->SetFacingTo(unit->GetAngle(bot));

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

void RpgAction::cancel(Unit* unit)
{
    context->GetValue<ObjectGuid>("rpg target")->Set(ObjectGuid());
}

void RpgAction::taxi(Unit* unit)
{
    uint32 curloc = sObjectMgr.GetNearestTaxiNode(unit->GetPositionX(), unit->GetPositionY(), unit->GetPositionZ(), unit->GetMapId(), bot->GetTeam());

    vector<uint32> nodes;
    for (uint32 i = 0; i < sTaxiPathStore.GetNumRows(); ++i)
    {
        TaxiPathEntry const* entry = sTaxiPathStore.LookupEntry(i);
        if (entry && entry->from == curloc)
        {
            //uint8  field = uint8((i - 1) / 32);
            //if (field < TaxiMaskSize) nodes.push_back(i);
			nodes.push_back(i);
        }
    }

    if (nodes.empty())
    {
        sLog.outError("Bot %s - No flight paths available", bot->GetName());
        return;
    }

    context->GetValue<ObjectGuid>("rpg target")->Set(ObjectGuid());

    uint32 path = nodes[urand(0, nodes.size() - 1)];
    //bot->m_taxi.SetTaximaskNode(path); // Causes crash/freeze
    uint32 money = bot->GetMoney();
    bot->SetMoney(money + 100000);

    TaxiPathEntry const* entry = sTaxiPathStore.LookupEntry(path);
    if (!entry)
        return;

    TaxiNodesEntry const* nodeFrom = sTaxiNodesStore.LookupEntry(entry->from);
    TaxiNodesEntry const* nodeTo = sTaxiNodesStore.LookupEntry(entry->to);

    Creature* flightMaster = bot->GetNPCIfCanInteractWith(unit->GetObjectGuid(), UNIT_NPC_FLAG_FLIGHTMASTER);
    if (!flightMaster)
    {
        sLog.outError("Bot %s cannot talk to flightmaster (%zu location available)", bot->GetName(), nodes.size());
        return;
    }
    if (!bot->ActivateTaxiPathTo({ entry->from, entry->to }, flightMaster, 0))
    {
        sLog.outError("Bot %s cannot fly %u (%zu location available)", bot->GetName(), path, nodes.size());
        return;
    }
    sLog.outString("Bot #%d <%s> is flying from %s to %s (%zu location available)", bot->GetGUIDLow(), bot->GetName(), nodeFrom->name[0], nodeTo->name[0], nodes.size());
    bot->SetMoney(money);
}

void RpgAction::quest(Unit* unit)
{
    uint32 type = TalkAction::GetRandomEmote(unit);

    ObjectGuid oldSelection = bot->GetSelectionGuid();

    bot->SetSelectionGuid(unit->GetObjectGuid());        

    //Parse rpg target to quest action.
    WorldPacket p(CMSG_QUESTGIVER_ACCEPT_QUEST);
    p << unit->GetObjectGuid();
    p.rpos(0);

    bool retVal = false;

  
    if (bot->GetSession()->getDialogStatus(bot, unit, DIALOG_STATUS_NONE) == DIALOG_STATUS_REWARD2) 
        retVal = ai->DoSpecificAction("talk to quest giver", Event("rpg action", p)); 
    else if (bot->GetSession()->getDialogStatus(bot, unit, DIALOG_STATUS_NONE) == DIALOG_STATUS_AVAILABLE)
        retVal = ai->DoSpecificAction("accept all quests", Event("rpg action", p));
    else
        bot->HandleEmoteCommand(type);

    if (retVal)
        RemIgnore(unit->GetObjectGuid());

    bot->HandleEmoteCommand(type);
    unit->SetFacingTo(unit->GetAngle(bot));

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);

    cancel(unit);
}

void RpgAction::trade(Unit* unit)
{
    ObjectGuid oldSelection = bot->GetSelectionGuid();

    bot->SetSelectionGuid(unit->GetObjectGuid());

    ai->DoSpecificAction("sell", Event("rpg action", "vendor"));
    ai->DoSpecificAction("buy", Event("rpg action", "vendor"));

    unit->SetFacingTo(unit->GetAngle(bot));

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

void RpgAction::repair(Unit* unit)
{
    ObjectGuid oldSelection = bot->GetSelectionGuid();

    bot->SetSelectionGuid(unit->GetObjectGuid());

    ai->DoSpecificAction("repair");

    unit->SetFacingTo(unit->GetAngle(bot));

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

void RpgAction::train(Unit* target)
{
    ObjectGuid oldSelection = bot->GetSelectionGuid();
    ObjectGuid newSelection = target->GetObjectGuid();

    bot->SetSelectionGuid(newSelection);

    ai->DoSpecificAction("trainer");

    target->SetFacingTo(target->GetAngle(bot));

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

void RpgAction::heal(Unit* unit)
{
    ObjectGuid oldSelection = bot->GetSelectionGuid();

    bot->SetSelectionGuid(unit->GetObjectGuid());

    switch (bot->getClass())
    {
    case CLASS_PRIEST:
        ai->DoSpecificAction("lesser heal on party");
        /* Example to cast a specific spel on a specif unit. Maybe usefull later.
        if(!ai->HasAura("power word: fortitude", unit))
            ai->DoSpecificAction("cast custom spell", Event("rpg action", chat->formatWorldobject(unit) + " power word: fortitude"), true);
        else
            ai->DoSpecificAction("cast custom spell", Event("rpg action", chat->formatWorldobject(unit) + " 2052"), true); 
        */
        break;
    case CLASS_DRUID:
        ai->DoSpecificAction("healing touch on party");
        break;
    case CLASS_PALADIN:
        ai->DoSpecificAction("holy light on party");
        break;
    case CLASS_SHAMAN:
        ai->DoSpecificAction("healing wave on party");
        break;
    }

    unit->SetFacingTo(unit->GetAngle(bot));

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}


bool RpgAction::isUseful()
{
    return context->GetValue<ObjectGuid>("rpg target")->Get();
}


bool RpgAction::CanTrain(ObjectGuid guid)
{
    Creature* creature = ai->GetCreature(guid);

    if (!creature)
        return false;

    if (!creature->IsTrainerOf(bot, false))
        return false;

    // check present spell in trainer spell list
    TrainerSpellData const* cSpells = creature->GetTrainerSpells();
    TrainerSpellData const* tSpells = creature->GetTrainerTemplateSpells();
    if (!cSpells && !tSpells)
    {
        return false;
    }

    float fDiscountMod = bot->GetReputationPriceDiscount(creature);

    TrainerSpellData const* trainer_spells = cSpells;
    if (!trainer_spells)
        trainer_spells = tSpells;

    for (TrainerSpellMap::const_iterator itr = trainer_spells->spellList.begin(); itr != trainer_spells->spellList.end(); ++itr)
    {
        TrainerSpell const* tSpell = &itr->second;

        if (!tSpell)
            continue;

        uint32 reqLevel = 0;

        reqLevel = tSpell->isProvidedReqLevel ? tSpell->reqLevel : std::max(reqLevel, tSpell->reqLevel);
        TrainerSpellState state = bot->GetTrainerSpellState(tSpell, reqLevel);
        if (state != TRAINER_SPELL_GREEN)
            continue;

        uint32 spellId = tSpell->spell;
        const SpellEntry* const pSpellInfo = sServerFacade.LookupSpellInfo(spellId);
        if (!pSpellInfo)
            continue;

        uint32 cost = uint32(floor(tSpell->spellCost * fDiscountMod));
        if (cost > bot->GetMoney())
            continue;

        return true;
    }

    return false;
}

bool RpgAction::needRepair()
{
    for (int i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; ++i)
    {
        uint16 pos = ((INVENTORY_SLOT_BAG_0 << 8) | i);
        Item* item = bot->GetItemByPos(pos);

        uint32 TotalCost = 0;
        if (!item)
            continue;

        uint32 maxDurability = item->GetUInt32Value(ITEM_FIELD_MAXDURABILITY);
        if (!maxDurability)
            continue;

        uint32 curDurability = item->GetUInt32Value(ITEM_FIELD_DURABILITY);

        if (maxDurability > curDurability)
            return true;
    }
    return false;
}