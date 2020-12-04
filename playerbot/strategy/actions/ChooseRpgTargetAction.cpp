#include "botpch.h"
#include "../../playerbot.h"
#include "ChooseRpgTargetAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../values/PossibleRpgTargetsValue.h"

using namespace ai;

bool ChooseRpgTargetAction::CanTrain(ObjectGuid guid)
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
}

bool ChooseRpgTargetAction::Execute(Event event)
{
    list<ObjectGuid> possibleTargets = AI_VALUE(list<ObjectGuid>, "possible rpg targets");
    set<ObjectGuid>& ignore = context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Get();
    if (possibleTargets.empty())
    {
        return false;
    }

    vector<Unit*> units;

    for (list<ObjectGuid>::iterator i = possibleTargets.begin(); i != possibleTargets.end(); ++i)
    {
        Unit* unit = ai->GetUnit(*i);

        uint32 dialogStatus = bot->GetSession()->getDialogStatus(bot, unit, DIALOG_STATUS_NONE);
        
        if (unit && (ignore.empty() || ignore.find(unit->GetObjectGuid()) == ignore.end()) && (CanTrain(*i) || dialogStatus == DIALOG_STATUS_REWARD2 || dialogStatus == DIALOG_STATUS_AVAILABLE)) units.push_back(unit);
    }

    if (units.empty())
    {        

        for (list<ObjectGuid>::iterator i = possibleTargets.begin(); i != possibleTargets.end(); ++i)
        {
            Unit* unit = ai->GetUnit(*i);
            if (unit && (ignore.empty() || ignore.find(unit->GetObjectGuid()) == ignore.end())) units.push_back(unit);
        }
    }

    if (units.empty())
    {
        sLog.outDetail("%s can't choose RPG target: all %zu are not available", bot->GetName(), possibleTargets.size());
        ignore.clear(); //Clear ignore list.
        context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Set(ignore);
        return false;
    }

    Unit* target = units[urand(0, units.size() - 1)];
    if (!target) {
        return false;
    }

    context->GetValue<ObjectGuid>("rpg target")->Set(target->GetObjectGuid());

    if (ignore.size() > 50)
        ignore.erase(ignore.begin());

    ignore.insert(target->GetObjectGuid());

    
    context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Set(ignore);

    return true;
}

bool ChooseRpgTargetAction::isUseful()
{
    return !context->GetValue<ObjectGuid>("rpg target")->Get();
}
