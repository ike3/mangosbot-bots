#include "botpch.h"
#include "../../playerbot.h"
#include "RpgAction.h"
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

    if (!sServerFacade.IsInFront(bot, target, sPlayerbotAIConfig.sightDistance, CAST_ANGLE_IN_FRONT) && !bot->IsTaxiFlying())
    {
        sServerFacade.SetFacingTo(bot, target, true);
        ai->SetNextCheckDelay(sPlayerbotAIConfig.globalCoolDown);
        return false;
    }

    if (!bot->GetNPCIfCanInteractWith(target->GetObjectGuid(), UNIT_NPC_FLAG_NONE))
        return false;

    if (bot->GetShapeshiftForm() > 0)
        bot->SetShapeshiftForm(FORM_NONE);

    if (target->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_FLIGHTMASTER))
    {
        WorldPacket emptyPacket;
        bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
        taxi(target);
        return true;
    }

    if (bot->GetSession()->getDialogStatus(bot, target, DIALOG_STATUS_NONE) == DIALOG_STATUS_REWARD2 || bot->GetSession()->getDialogStatus(bot, target, DIALOG_STATUS_NONE) == DIALOG_STATUS_AVAILABLE)
    {
        WorldPacket emptyPacket;
        bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
        quest(target);
        return true;
    }

    if (creature && CanTrain(guid))
    {
        WorldPacket emptyPacket;
        bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
        train(creature);
        return true;
    }

    if (target->IsArmorer() && needRepair())
    {
        WorldPacket emptyPacket;
        bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
        repair(target);
        return true;
    }
     
    vector<RpgElement> elements;
    elements.push_back(&RpgAction::cancel);
    elements.push_back(&RpgAction::emote);
    elements.push_back(&RpgAction::stay);
    elements.push_back(&RpgAction::work);
    if (target->IsVendor())
        elements.push_back(&RpgAction::trade);

    RpgElement element = elements[urand(0, elements.size() - 1)];
    (this->*element)(target);
    return true;
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
    sLog.outString("Bot %s is flying to %u (%zu location available)", bot->GetName(), path, nodes.size());
    bot->SetMoney(money);
}

void RpgAction::quest(Unit* unit)
{
    uint32 type = TalkAction::GetRandomEmote(unit);

    ObjectGuid oldSelection = bot->GetSelectionGuid();

    bot->SetSelectionGuid(unit->GetObjectGuid());        
    
    if (bot->GetSession()->getDialogStatus(bot, unit, DIALOG_STATUS_NONE) == DIALOG_STATUS_REWARD2)
        ai->DoSpecificAction("talk to quest giver");
    else if (bot->GetSession()->getDialogStatus(bot, unit, DIALOG_STATUS_NONE) == DIALOG_STATUS_AVAILABLE)
        ai->DoSpecificAction("accept all quests");
    else
        bot->HandleEmoteCommand(type);

    bot->HandleEmoteCommand(type);
    unit->SetFacingTo(unit->GetAngle(bot));

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}

void RpgAction::trade(Unit* unit)
{
    ObjectGuid oldSelection = bot->GetSelectionGuid();

    bot->SetSelectionGuid(unit->GetObjectGuid());

    ai->DoSpecificAction("sell gray");

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

void RpgAction::train(Creature* unit)
{
    ObjectGuid oldSelection = bot->GetSelectionGuid();

    bot->SetSelectionGuid(unit->GetObjectGuid());

    bot->Say("Want to train.",0);

    ai->DoSpecificAction("trainer");

    unit->SetFacingTo(unit->GetAngle(bot));

    if (oldSelection)
        bot->SetSelectionGuid(oldSelection);

    ai->SetNextCheckDelay(sPlayerbotAIConfig.rpgDelay);
}


bool RpgAction::isUseful()
{
    return context->GetValue<ObjectGuid>("rpg target")->Get();
}
