#pragma once

#include "../Action.h"
#include "ChooseTargetActions.h"

#include "../../ServerFacade.h"
#include "MovementGenerator.h"
#include "CreatureAI.h"

bool DropTargetAction::Execute(Event event)
{
    Unit* target = context->GetValue<Unit*>("current target")->Get();
    ObjectGuid pullTarget = context->GetValue<ObjectGuid>("pull target")->Get();
    list<ObjectGuid> possible = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("possible targets")->Get();
    if (pullTarget && find(possible.begin(), possible.end(), pullTarget) == possible.end())
    {
        context->GetValue<ObjectGuid>("pull target")->Set(ObjectGuid());
    }

    context->GetValue<Unit*>("current target")->Set(NULL);
    bot->SetSelectionGuid(ObjectGuid());
    ai->ChangeEngine(BOT_STATE_NON_COMBAT);
    ai->InterruptSpell();
    bot->AttackStop();
    bot->StopMoving();

    Pet* pet = bot->GetPet();
    if (pet)
    {
#ifdef MANGOS
        CreatureAI*
#endif
#ifdef CMANGOS
        UnitAI*
#endif
        creatureAI = ((Creature*)pet)->AI();
        if (creatureAI)
        {
#ifdef CMANGOS
            creatureAI->SetReactState(REACT_PASSIVE);
#endif
#ifdef MANGOS
            pet->GetCharmInfo()->SetReactState(REACT_PASSIVE);
            pet->GetCharmInfo()->SetCommandState(COMMAND_FOLLOW);
#endif
            pet->AttackStop();
            pet->StopMoving();
        }
    }
    if (!urand(0, 25))
    {
        vector<uint32> sounds;
        if (target && sServerFacade.UnitIsDead(target))
        {
            sounds.push_back(TEXTEMOTE_CHEER);
            sounds.push_back(TEXTEMOTE_CONGRATULATE);
        }
        else
        {
            sounds.push_back(304); // guard
            sounds.push_back(325); // stay
        }
        if (!sounds.empty()) ai->PlaySound(sounds[urand(0, sounds.size() - 1)]);
    }
    return true;
}


bool AttackAnythingAction::Execute(Event event)
{
    bool result = AttackAction::Execute(event);
    if (result && GetTarget()) context->GetValue<ObjectGuid>("pull target")->Set(GetTarget()->GetObjectGuid());

    if (result && sRandomPlayerbotMgr.IsRandomBot(bot) && !bot->GetGroup())
    {
        if (urand(0, 100) > 100.0 * sPlayerbotAIConfig.randomBotGrindChance) return false;

        Pet* pet = bot->GetPet();
        if (pet)
        {
            list<string> autoSpells;
            autoSpells.push_back("growl");
            autoSpells.push_back("bite");
            autoSpells.push_back("claw");
            autoSpells.push_back("suffering");
            autoSpells.push_back("torment");
            autoSpells.push_back("blood pact");
            autoSpells.push_back("firebolt");
            autoSpells.push_back("lash of pain");
            for (list<string>::iterator i = autoSpells.begin(); i != autoSpells.end(); ++i)
            {
                uint32 spellId = AI_VALUE2(uint32, "spell id", *i);
                if (spellId && pet->HasSpell(spellId)) pet->ToggleAutocast(spellId, true);
            }
        }
    }

    return result;
}
