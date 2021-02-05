#include "botpch.h"
#include "../../playerbot.h"
#include "ChooseRpgTargetAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../values/PossibleRpgTargetsValue.h"
#include "../../Travelmgr.h"

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
    return false;
}

uint32 ChooseRpgTargetAction::HasSameTarget(ObjectGuid guid)
{
    if (ai->GetMaster())
        return 0;

    uint32 num = 0;

    list<ObjectGuid> nearGuids = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest friendly players")->Get();
    for (auto& i : nearGuids)
    {
        Player* player = sObjectMgr.GetPlayer(i);

        if (!player)
            continue;

        PlayerbotAI* ai = player->GetPlayerbotAI();

        if (!ai)
            continue;

        if (ai->GetAiObjectContext()->GetValue<ObjectGuid>("rpg target")->Get() != guid)
            continue;

        num++;
    }

    return num;
}

bool ChooseRpgTargetAction::Execute(Event event)
{    
    TravelTarget* travelTarget = context->GetValue<TravelTarget*>("travel target")->Get();
    list<ObjectGuid> possibleTargets = AI_VALUE(list<ObjectGuid>, "possible rpg targets");
    set<ObjectGuid>& ignoreList = context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Get();
    if (possibleTargets.empty())
    {
        return false;
    }

    vector<Unit*> units;
    Player* master = ai->GetMaster();
    bool nearMaster = false;
    if (master)
    {
        if (ai->HasStrategy("follow", BOT_STATE_NON_COMBAT))
        {
            nearMaster = bot->GetDistance(master) < sPlayerbotAIConfig.rpgDistance * 2;
        }
    }

    int maxPriority = 1;

    //First handing in quests
    for (list<ObjectGuid>::iterator i = possibleTargets.begin(); i != possibleTargets.end(); ++i)
    {
        Unit* unit = ai->GetUnit(*i);

        if (!unit)
            continue;

        if (!ignoreList.empty() && ignoreList.find(unit->GetObjectGuid()) != ignoreList.end() && urand(0, 100) < 10) //10% chance to retry ignored.
            continue;

        if (nearMaster && unit->GetDistance(master) > sPlayerbotAIConfig.sightDistance)
            continue;
        
        int priority = 1;

#ifdef MANGOS
        if (AI_VALUE(uint8, "bag space") > 80 && unit->IsVendor())
#endif
#ifdef CMANGOS
        if (AI_VALUE(uint8, "bag space") > 80 && unit->isVendor())
#endif
            priority = 100;
        uint32 dialogStatus = bot->GetSession()->getDialogStatus(bot, unit, DIALOG_STATUS_NONE);        
        if (dialogStatus == DIALOG_STATUS_REWARD2)
            priority = 90;
        else if (CanTrain(*i) || dialogStatus == DIALOG_STATUS_AVAILABLE)
            priority = 80;
        else if (travelTarget->getDestination() && travelTarget->getDestination()->getEntry() == unit->GetEntry())
            priority = 70;

        if (priority < maxPriority)
            continue;

        if (!ai->GetMaster() && HasSameTarget(unit->GetObjectGuid()) > urand(5, 15))
            continue;

        if (priority > maxPriority)
            units.clear();

        maxPriority = priority;

        units.push_back(unit);
    }

    if (units.empty())
    {
        sLog.outDetail("%s can't choose RPG target: all %zu are not available", bot->GetName(), possibleTargets.size());
        ignoreList.clear(); //Clear ignore list.
        context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Set(ignoreList);
        context->GetValue<ObjectGuid>("rpg target")->Set(ObjectGuid());
        return false;
    }

    Unit* target = units[urand(0, units.size() - 1)];
    if (!target) {
        context->GetValue<ObjectGuid>("rpg target")->Set(ObjectGuid());
        return false;
    }

    context->GetValue<ObjectGuid>("rpg target")->Set(target->GetObjectGuid());

    return true;
}

bool ChooseRpgTargetAction::isUseful()
{
    return !context->GetValue<ObjectGuid>("rpg target")->Get() && !context->GetValue<TravelTarget*>("travel target")->Get()->isTraveling();
}
