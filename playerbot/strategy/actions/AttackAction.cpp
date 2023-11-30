#include "botpch.h"
#include "../../playerbot.h"
#include "AttackAction.h"
#include "MovementGenerator.h"
#include "CreatureAI.h"
#include "../../LootObjectStack.h"
#include "../../ServerFacade.h"
#include "../generic/CombatStrategy.h"

using namespace ai;

bool AttackAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    Unit* target = GetTarget();
    if (target && target->IsInWorld() && target->GetMapId() == bot->GetMapId())
    {
        //Unit* victim = bot->GetVictim();
        //if (victim && victim->IsPlayer() && victim->GetObjectGuid() == target->GetObjectGuid())
        //{
        //    return false;
        //}

        return Attack(requester, target);
    }

    return false;
}

bool AttackMyTargetAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    if(requester)
    {
        const ObjectGuid guid = requester->GetSelectionGuid();
        if (guid)
        {
            if (Attack(requester, ai->GetUnit(guid)))
            {
                SET_AI_VALUE(ObjectGuid, "attack target", guid);
                return true;
            }
        }
        else if (verbose)
        {
            ai->TellError(requester, "You have no target");
        }
    }

    return false;
}

bool AttackRTITargetAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    Unit* rtiTarget = AI_VALUE(Unit*, "rti target");

    if (rtiTarget && rtiTarget->IsInWorld() && rtiTarget->GetMapId() == bot->GetMapId())
    {
        if (Attack(requester, rtiTarget))
        {
            SET_AI_VALUE(ObjectGuid, "attack target", rtiTarget->GetObjectGuid());
            return true;
        }
    }
    else
    {
        ai->TellError(requester, "I dont see my rti attack target");
    }

    return false;
}

bool AttackMyTargetAction::isUseful()
{
    if (ai->ContainsStrategy(STRATEGY_TYPE_HEAL))
        return false;

    return true;
}

bool AttackRTITargetAction::isUseful()
{
    if (ai->ContainsStrategy(STRATEGY_TYPE_HEAL))
        return false;

    return true;
}

bool AttackAction::Attack(Player* requester, Unit* target)
{
    MotionMaster &mm = *bot->GetMotionMaster();
	if (mm.GetCurrentMovementGeneratorType() == TAXI_MOTION_TYPE || (bot->IsFlying() && WorldPosition(bot).currentHeight() > 10.0f))
    {
        if (verbose) ai->TellError(requester, "I cannot attack in flight");
        return false;
    }

    if(IsTargetValid(requester, target))
    {
        if (bot->IsMounted() && (sServerFacade.GetDistance2d(bot, target) < 40.0f || bot->IsFlying()))
        {
            WorldPacket emptyPacket;
            bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
            
            if (bot->IsFlying())
            {
                bot->GetMotionMaster()->MoveFall();
                return true;
            }
        }

        ObjectGuid guid = target->GetObjectGuid();
        bot->SetSelectionGuid(target->GetObjectGuid());

        Unit* oldTarget = AI_VALUE(Unit*, "current target");
        if(oldTarget)
        {
            SET_AI_VALUE(Unit*, "old target", oldTarget);
        }

        SET_AI_VALUE(Unit*, "current target", target);
        AI_VALUE(LootObjectStack*, "available loot")->Add(guid);

        const bool isWaitingForAttack = WaitForAttackStrategy::ShouldWait(ai);

        Pet* pet = bot->GetPet();
        if (pet)
        {
            UnitAI* creatureAI = ((Creature*)pet)->AI();
            if (creatureAI)
            {
                creatureAI->SetReactState(REACT_DEFENSIVE);

                // Don't send the pet to attack if the bot is waiting for attack
                if (!isWaitingForAttack && (!ai->HasStrategy("stay", BotState::BOT_STATE_COMBAT) || AI_VALUE2(float, "distance", "current target") < ai->GetRange("spell")))
                {
                    creatureAI->AttackStart(target);
                }
            }
        }

        if (IsMovingAllowed() && !sServerFacade.IsInFront(bot, target, sPlayerbotAIConfig.sightDistance, CAST_ANGLE_IN_FRONT))
        {
            sServerFacade.SetFacingTo(bot, target);
        }

        // Don't attack target if it is waiting for attack or in stealth
        if (!ai->HasStrategy("stealthed", BotState::BOT_STATE_COMBAT) && !isWaitingForAttack)
        {
            if (urand(0, 100) < sPlayerbotAIConfig.attackEmoteChance * 100)
            {
                vector<uint32> sounds;
                sounds.push_back(TEXTEMOTE_OPENFIRE);
                sounds.push_back(305);
                sounds.push_back(307);
                ai->PlaySound(sounds[urand(0, sounds.size() - 1)]);
            }

            return bot->Attack(target, !ai->IsRanged(bot) || (sServerFacade.GetDistance2d(bot, target) < 5.0f));
        }
        else
        {
            return true;
        }
    }

    return false;
}

bool AttackAction::IsTargetValid(Player* requester, Unit* target)
{
    ostringstream msg;
    if (!target)
    {
        if (verbose) ai->TellError(requester, "I have no target");
        return false;
    }
    else if (sServerFacade.IsFriendlyTo(bot, target))
    {
        msg << target->GetName();
        msg << " is friendly to me";
        if (verbose) ai->TellError(requester, msg.str());
        return false;
    }
    else if (sServerFacade.UnitIsDead(target))
    {
        msg << target->GetName();
        msg << " is dead";
        if (verbose) ai->TellError(requester, msg.str());
        return false;
    }

    return true;
}

bool AttackDuelOpponentAction::isUseful()
{
    return AI_VALUE(Unit*, "duel target");
}

bool AttackDuelOpponentAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    return Attack(requester, AI_VALUE(Unit*, "duel target"));
}
