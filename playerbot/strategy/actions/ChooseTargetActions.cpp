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

    if (ai->ContainsStrategy(STRATEGY_TYPE_HEAL))
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
                if (ai->HasStrategy("pull", BotState::BOT_STATE_COMBAT))
                {
                    Event pullEvent("attack anything", grindTarget->GetObjectGuid());
                    bool doAction = ai->DoSpecificAction("pull my target", pullEvent, true);
                    
                    if (doAction)
                        return true;
                }

                context->GetValue<ObjectGuid>("attack target")->Set(grindTarget->GetObjectGuid());
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

bool SelectNewTargetAction::Execute(Event& event)
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (target && sServerFacade.UnitIsDead(target))
    {
        // Save the dead target for later looting
        ObjectGuid guid = target->GetObjectGuid();
        if (guid)
        {
            AI_VALUE(LootObjectStack*, "available loot")->Add(guid);
        }
    }

    // Clear the target variables
    ObjectGuid attackTarget = AI_VALUE(ObjectGuid, "attack target");
    list<ObjectGuid> possible = AI_VALUE(list<ObjectGuid>, "possible targets no los");
    if (attackTarget && find(possible.begin(), possible.end(), attackTarget) == possible.end())
    {
        SET_AI_VALUE(ObjectGuid, "attack target", ObjectGuid());
    }

    // Save the old target and clear the current target
    if(target)
    {
        SET_AI_VALUE(Unit*, "old target", target);
        SET_AI_VALUE(Unit*, "current target", nullptr);
    }
    
    // Stop attacking
    bot->SetSelectionGuid(ObjectGuid());
    ai->InterruptSpell();
    bot->AttackStop();

    // Stop pet attacking
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

    // Check if there is any enemy targets available to attack
    if (!AI_VALUE(list<ObjectGuid>, "attackers").empty())
    {
        // Check if there is an enemy player nearby
        Unit* enemyPlayer = AI_VALUE(Unit*, "enemy player target");
        if (enemyPlayer && 
           (ai->HasStrategy("pvp", BotState::BOT_STATE_NON_COMBAT) ||
            ai->HasStrategy("duel", BotState::BOT_STATE_NON_COMBAT)))
        {
            return ai->DoSpecificAction("attack enemy player", event, true);
        }
        else
        {
            // Let the dps/tank assist pick a target to attack
            if (ai->HasStrategy("dps assist", BotState::BOT_STATE_NON_COMBAT))
            {
                return ai->DoSpecificAction("dps assist", event, true);
            }
            else if (ai->HasStrategy("tank assist", BotState::BOT_STATE_NON_COMBAT))
            {
                return ai->DoSpecificAction("tank assist", event, true);
            }
        }
    }

    return false;
}