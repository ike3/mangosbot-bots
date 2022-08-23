#pragma once

#include "../Action.h"
#include "ChooseTargetActions.h"
#include "MovementGenerator.h"
#include "CreatureAI.h"

bool AttackAnythingAction::isUseful() {

    if (!ai->AllowActivity(GRIND_ACTIVITY))                                              //Bot not allowed to be active
        return false;

    if (!AI_VALUE(bool, "can move around"))
        return false;

    Unit* target = GetTarget();

    if (!target)
        return false;

    if(!target->IsPlayer() && bot->isInFront(target,target->GetAttackDistance(bot)*1.5f, M_PI_F*0.5f) && target->CanAttackOnSight(bot) && target->GetLevel() < bot->GetLevel() + 3.0) //Attack before being attacked.
        return true;

    if (context->GetValue<TravelTarget*>("travel target")->Get()->isTraveling() && ChooseRpgTargetAction::isFollowValid(bot, *context->GetValue<TravelTarget*>("travel target")->Get()->getPosition())) //Bot is traveling
        return false;

    string name = string(target->GetName());

    if (!name.empty() && name.find("Dummy") != std::string::npos) //Target is not a targetdummy
        return false;

    if(!ChooseRpgTargetAction::isFollowValid(bot, target))                               //Do not grind mobs far away from master.
        return false;

    return true;
}

/*

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