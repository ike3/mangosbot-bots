#pragma once

#include "../Action.h"
#include "ChooseTargetActions.h"
#include "MovementGenerator.h"
#include "CreatureAI.h"

bool DpsAssistAction::isUseful()
{
    // if carry flag, do not start fight
    if (bot->HasAura(23333) || bot->HasAura(23335) || bot->HasAura(34976))
        return false;

    return true;
}

bool AttackAnythingAction::isUseful() 
{
    if (!ai->AllowActivity(GRIND_ACTIVITY)) //Bot not allowed to be active
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

    if (!name.empty() && name.find("Dummy") != std::string::npos) //Target is not a target dummy
        return false;

    if(!ChooseRpgTargetAction::isFollowValid(bot, target)) //Do not grind mobs far away from master.
        return false;

    return true;
}

bool ai::AttackAnythingAction::isPossible()
{
    return AttackAction::isPossible() && GetTarget();
}

bool ai::AttackAnythingAction::Execute(Event& event)
{
    bool result = AttackAction::Execute(event);
    if (result)
    {
        Unit* grindTarget = GetTarget();
        if (grindTarget)
        {
            const char* grindName = grindTarget->GetName();
            if (grindName)
            {
                context->GetValue<ObjectGuid>("pull target")->Set(grindTarget->GetObjectGuid());
                ai->StopMoving();
            }
        }
    }

    return result;
}

bool AttackEnemyPlayerAction::isUseful()
{
    // if carry flag, do not start fight
    if (bot->HasAura(23333) || bot->HasAura(23335) || bot->HasAura(34976))
        return false;

    return !sPlayerbotAIConfig.IsInPvpProhibitedZone(sServerFacade.GetAreaId(bot));
}

bool AttackEnemyFlagCarrierAction::isUseful()
{
    Unit* target = context->GetValue<Unit*>("enemy flag carrier")->Get();
    return target && sServerFacade.IsDistanceLessOrEqualThan(sServerFacade.GetDistance2d(bot, target), 75.0f) && (bot->HasAura(23333) || bot->HasAura(23335) || bot->HasAura(34976));
}

/*
bool DropTargetAction::Execute(Event& event)
{
    context->GetValue<Unit*>("current target")->Set(NULL);
    bot->SetSelectionGuid(ObjectGuid());
    ai->ChangeEngine(BotState::BOT_STATE_NON_COMBAT);
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

bool DropTargetAction::Execute(Event& event)
{
    Unit* target = context->GetValue<Unit*>("current target")->Get();
    if (target && sServerFacade.UnitIsDead(target))
    {
        ObjectGuid guid = target->GetObjectGuid();
        if (guid)
            context->GetValue<LootObjectStack*>("available loot")->Get()->Add(guid);
    }

    ObjectGuid pullTarget = context->GetValue<ObjectGuid>("pull target")->Get();
    list<ObjectGuid> possible = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("possible targets no los")->Get();
    if (pullTarget && find(possible.begin(), possible.end(), pullTarget) == possible.end())
    {
        context->GetValue<ObjectGuid>("pull target")->Set(ObjectGuid());
    }

    context->GetValue<Unit*>("current target")->Set(NULL);
    bot->SetSelectionGuid(ObjectGuid());

    // attack next target if in combat
    uint32 attackers = AI_VALUE(uint8, "attacker count");
    if (attackers > 0)
    {
        Unit* enemy = AI_VALUE(Unit*, "enemy player target");
        if (!enemy)
        {
            ai->InterruptSpell();
            bot->AttackStop();

            if (ai->HasStrategy("dps assist", BotState::BOT_STATE_NON_COMBAT))
                return ai->DoSpecificAction("dps assist", Event(), true);
            if (ai->HasStrategy("tank assist", BotState::BOT_STATE_NON_COMBAT))
                return ai->DoSpecificAction("tank assist", Event(), true);
        }
    }

    ai->InterruptSpell();
    bot->AttackStop();
    Pet* pet = bot->GetPet();
    if (pet)
    {
#ifdef MANGOS
        CreatureAI* creatureAI = ((Creature*)pet)->AI();
#endif
#ifdef CMANGOS
        UnitAI* creatureAI = ((Creature*)pet)->AI();
#endif
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
        }
    }
    return true;
}