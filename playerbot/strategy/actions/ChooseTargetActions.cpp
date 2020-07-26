/*#pragma once

#include "../Action.h"
#include "ChooseTargetActions.h"
#include "MovementGenerator.h"
#include "CreatureAI.h"

bool DropTargetAction::Execute(Event event)
{
    context->GetValue<Unit*>("current target")->Set(NULL);
    bot->SetSelectionGuid(ObjectGuid());
    ai->ChangeEngine(BOT_STATE_NON_COMBAT);
    ai->InterruptSpell();
    bot->AttackStop();
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
#endif
            pet->AttackStop();
        }
    }
    if (!urand(0, 200))
    {
        vector<uint32> sounds;
        sounds.push_back(TEXTEMOTE_CHEER);
        sounds.push_back(TEXTEMOTE_CONGRATULATE);
        ai->PlaySound(sounds[urand(0, sounds.size() - 1)]);
    }
    return true;
}
*/