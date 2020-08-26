#include "botpch.h"
#include "../../playerbot.h"
#include "../values/LastMovementValue.h"
#include "MovementActions.h"
#include "MotionMaster.h"
#include "MovementGenerator.h"
#include "../../FleeManager.h"
#include "../../LootObjectStack.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"
#include "../values/PositionValue.h"
#include "../values/Stances.h"
#include "MotionGenerators/TargetedMovementGenerator.h"

using namespace ai;

bool MovementAction::MoveNear(uint32 mapId, float x, float y, float z, float distance)
{
    float angle = GetFollowAngle();
    return MoveTo(mapId, x + cos(angle) * distance, y + sin(angle) * distance, z);
}

bool MovementAction::MoveNear(WorldObject* target, float distance)
{
    if (!target)
        return false;

#ifdef MANGOS
    distance += target->GetObjectBoundingRadius();
#endif

    float x = target->GetPositionX();
    float y = target->GetPositionY();
    float z = target->GetPositionZ();
    float followAngle = GetFollowAngle();
    for (float angle = followAngle; angle <= followAngle + 2 * M_PI; angle += M_PI / 4)
    {
#ifdef CMANGOS
        float dist = distance + target->GetObjectBoundingRadius();
        target->GetNearPoint(bot, x, y, z, bot->GetObjectBoundingRadius(), min(dist, sPlayerbotAIConfig.followDistance), angle);
#endif
#ifdef MANGOS
        float x = target->GetPositionX() + cos(angle) * distance,
             y = target->GetPositionY()+ sin(angle) * distance,
             z = target->GetPositionZ();
#endif
        if (!bot->IsWithinLOS(x, y, z))
            continue;
        bool moved = MoveTo(target->GetMapId(), x, y, z);
        if (moved)
            return true;
    }

    ai->TellError("All paths not in LOS");
    return false;
}

bool MovementAction::MoveTo(uint32 mapId, float x, float y, float z, bool idle)
{
    UpdateMovementState();

    bool generatePath = !bot->IsFlying() && !bot->HasMovementFlag(MOVEFLAG_SWIMMING) && !bot->IsInWater() && !sServerFacade.IsUnderwater(bot);
    if (generatePath)
    {
        z += CONTACT_DISTANCE;
        bot->UpdateAllowedPositionZ(x, y, z);
    }

    if (!IsMovingAllowed(mapId, x, y, z))
    {
        ai->TellError("I am stuck");
        return false;
    }

    float distance = sServerFacade.GetDistance2d(bot, x, y);
    if (sServerFacade.IsDistanceGreaterThan(distance, sPlayerbotAIConfig.contactDistance))
    {
        WaitForReach(distance);

        bot->HandleEmoteState(0);
        if (bot->IsSitState())
            bot->SetStandState(UNIT_STAND_STATE_STAND);

        if (bot->IsNonMeleeSpellCasted(true))
        {
            bot->CastStop();
            ai->InterruptSpell();
        }

        MotionMaster &mm = *bot->GetMotionMaster();
        mm.MovePoint(mapId, x, y, z, generatePath);

        AI_VALUE(LastMovement&, "last movement").Set(x, y, z, bot->GetOrientation());
        if (!idle)
            ClearIdleState();
        return true;
    }

    //ai->TellMasterNoFacing("No need to move");
    return false;
}

bool MovementAction::MoveTo(Unit* target, float distance)
{
    if (!IsMovingAllowed(target))
    {
        ai->TellError("Seems I am stuck");
        return false;
    }

    float bx = bot->GetPositionX();
    float by = bot->GetPositionY();
    float bz = bot->GetPositionZ();

    Stance* stance = AI_VALUE(Stance*, "stance");
    WorldLocation loc = stance->GetLocation();
    if (Formation::IsNullLocation(loc) || loc.mapid == -1)
    {
        ai->TellError("Nowhere to move");
        return false;
}

    float tx = loc.coord_x, ty = loc.coord_y, tz = loc.coord_z;
    float distanceToTarget = sServerFacade.GetDistance2d(bot, tx, ty);
    float angle = bot->GetAngle(tx, ty);
    float needToGo = distanceToTarget - distance;

    float maxDistance = ai->GetRange("spell");
    if (needToGo > 0 && needToGo > maxDistance)
        needToGo = maxDistance;
    else if (needToGo < 0 && needToGo < -maxDistance)
        needToGo = -maxDistance;

    float dx = cos(angle) * needToGo + bx;
    float dy = sin(angle) * needToGo + by;
    float dz = bz + (tz - bz) * needToGo;

    return MoveTo(target->GetMapId(), dx, dy, dz);
}

float MovementAction::GetFollowAngle()
{
    Player* master = GetMaster();
    Group* group = master ? master->GetGroup() : bot->GetGroup();
    if (!group)
        return 0.0f;

    int index = 1;
    for (GroupReference *ref = group->GetFirstMember(); ref; ref = ref->next())
    {
        if( ref->getSource() == master)
            continue;

        if( ref->getSource() == bot)
            return 2 * M_PI / (group->GetMembersCount() -1) * index;

        index++;
    }
    return 0;
}

bool MovementAction::IsMovingAllowed(Unit* target)
{
    if (!target)
        return false;

    if (bot->GetMapId() != target->GetMapId())
        return false;

    float distance = bot->GetDistance(target);
    if (distance > sPlayerbotAIConfig.reactDistance)
        return false;

    return IsMovingAllowed();
}

bool MovementAction::IsMovingAllowed(uint32 mapId, float x, float y, float z)
{
    float distance = bot->GetDistance(x, y, z);
    if (distance > sPlayerbotAIConfig.reactDistance)
        return false;

    return IsMovingAllowed();
}

bool MovementAction::IsMovingAllowed()
{
    if (sServerFacade.IsFrozen(bot) || bot->IsPolymorphed() ||
			(sServerFacade.UnitIsDead(bot) && !bot->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST)) ||
            bot->IsBeingTeleported() ||
            sServerFacade.IsInRoots(bot) ||
            bot->HasAuraType(SPELL_AURA_MOD_CONFUSE) || sServerFacade.IsCharmed(bot) ||
            bot->HasAuraType(SPELL_AURA_MOD_STUN) || bot->IsFlying())
        return false;

    MotionMaster &mm = *bot->GetMotionMaster();
#ifdef CMANGOS
    return mm.GetCurrentMovementGeneratorType() != TAXI_MOTION_TYPE;
#endif
#ifdef MANGOS
    return mm.GetCurrentMovementGeneratorType() != FLIGHT_MOTION_TYPE;
#endif
}

bool MovementAction::Follow(Unit* target, float distance)
{
    return Follow(target, distance, GetFollowAngle());
}

void MovementAction::UpdateMovementState()
{
    if (bot->IsInWater() || sServerFacade.IsUnderwater(bot))
    {
		bot->m_movementInfo.AddMovementFlag(MOVEFLAG_SWIMMING);
        bot->UpdateSpeed(MOVE_SWIM, true);
    }
    else
    {
		bot->m_movementInfo.RemoveMovementFlag(MOVEFLAG_SWIMMING);
        bot->UpdateSpeed(MOVE_SWIM, true);
    }
}

bool MovementAction::Follow(Unit* target, float distance, float angle)
{
    MotionMaster &mm = *bot->GetMotionMaster();

    UpdateMovementState();

    if (!target)
        return false;

    if (sServerFacade.IsDistanceLessOrEqualThan(sServerFacade.GetDistance2d(bot, target->GetPositionX(), target->GetPositionY()), sPlayerbotAIConfig.sightDistance) &&
            abs(bot->GetPositionZ() - target->GetPositionZ()) >= sPlayerbotAIConfig.spellDistance)
    {
        bot->StopMoving();
        float x = bot->GetPositionX(), y = bot->GetPositionY(), z = target->GetPositionZ();
        if (target->GetMapId() && bot->GetMapId() != target->GetMapId())
        {
            bot->TeleportTo(target->GetMapId(), x, y, z, bot->GetOrientation());
        }
        else
        {
            bot->Relocate(x, y, z, bot->GetOrientation());
        }
        AI_VALUE(LastMovement&, "last movement").Set(target);
        ClearIdleState();
        return true;
    }

    if (!IsMovingAllowed(target))
    {
        if (sServerFacade.UnitIsDead(bot))
        {
            bot->ResurrectPlayer(1.0f, false);
            ai->TellMasterNoFacing("I live, again!");
        }
        else
            ai->TellError("I am stuck while following");

        bot->TeleportTo(target->GetMapId(), target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation());
        return false;
    }

    if (sServerFacade.IsFriendlyTo(target, bot) && bot->IsMounted() && AI_VALUE(list<ObjectGuid>, "all targets").empty())
        distance += angle;

    if (sServerFacade.IsDistanceLessOrEqualThan(sServerFacade.GetDistance2d(bot, target), sPlayerbotAIConfig.followDistance))
    {
        ai->TellError("No need to follow");
        return false;
    }

    bot->HandleEmoteState(0);
    if (bot->IsSitState())
        bot->SetStandState(UNIT_STAND_STATE_STAND);

    if (bot->IsNonMeleeSpellCasted(true))
    {
        bot->CastStop();
        ai->InterruptSpell();
    }

    if (sServerFacade.isMoving(bot))
        return false;

    AI_VALUE(LastMovement&, "last movement").Set(target);
    ClearIdleState();

    if (bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == FOLLOW_MOTION_TYPE)
    {
        Unit *currentTarget = sServerFacade.GetChaseTarget(bot);
        if (currentTarget && currentTarget->GetObjectGuid() == target->GetObjectGuid()) return false;
    }

    mm.MoveFollow(target,
#ifdef MANGOS
            distance,
#endif
#ifdef CMANGOS
            distance - target->GetObjectBoundingRadius(),
#endif
            angle);
    return true;
}

void MovementAction::WaitForReach(float distance)
{
    float delay = 1000.0f * distance / bot->GetSpeed(MOVE_RUN) + sPlayerbotAIConfig.reactDelay;

    if (delay > sPlayerbotAIConfig.maxWaitForMove)
        delay = sPlayerbotAIConfig.maxWaitForMove;

    Unit* target = *ai->GetAiObjectContext()->GetValue<Unit*>("current target");
    Unit* player = *ai->GetAiObjectContext()->GetValue<Unit*>("enemy player target");
    if ((player || target) && delay > sPlayerbotAIConfig.globalCoolDown)
        delay = sPlayerbotAIConfig.globalCoolDown;

    ai->SetNextCheckDelay((uint32)delay);
}

bool MovementAction::Flee(Unit *target)
{
    Player* master = GetMaster();
    if (!target)
        target = master;

    if (!target)
        return false;

    if (!sPlayerbotAIConfig.fleeingEnabled)
        return false;

    if (!IsMovingAllowed())
    {
        ai->TellError("I am stuck while fleeing");
        return false;
    }

    HostileReference *ref = target->GetThreatManager().getCurrentVictim();
    if (ref && ref->getTarget() == bot)
    {
        Group *group = bot->GetGroup();
        if (group)
        {
            for (GroupReference *gref = group->GetFirstMember(); gref; gref = gref->next())
            {
                Player* player = gref->getSource();
                if (!player || player == bot) continue;
                if (ai->IsTank(player))
                {
                    bool moved = MoveTo(player, sPlayerbotAIConfig.followDistance);
                    if (moved)
                        return true;
                }
            }
        }
    }

    FleeManager manager(bot, ai->GetRange("flee"), bot->GetAngle(target) + M_PI);

    if (!manager.isUseful())
        return false;

    if (!urand(0, 25))
    {
        vector<uint32> sounds;
        sounds.push_back(304); // guard
        sounds.push_back(306); // flee
        ai->PlaySound(sounds[urand(0, sounds.size() - 1)]);
    }

    float rx, ry, rz;
    if (!manager.CalculateDestination(&rx, &ry, &rz))
    {
        ai->TellError("Nowhere to flee");
        return false;
    }

    bool result = MoveTo(target->GetMapId(), rx, ry, rz);
    if (result && !urand(0, 25))
    {
        vector<uint32> sounds;
        sounds.push_back(304); // guard
        sounds.push_back(306); // flee
        ai->PlaySound(sounds[urand(0, sounds.size() - 1)]);
    }
    return result;
}

void MovementAction::ClearIdleState()
{
    context->GetValue<time_t>("stay time")->Set(0);
    context->GetValue<ai::PositionMap&>("position")->Get()["random"].Reset();
}

bool FleeAction::Execute(Event event)
{
    return Flee(AI_VALUE(Unit*, "current target"));
}

bool FleeWithPetAction::Execute(Event event)
{
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
        }
    }
    return Flee(AI_VALUE(Unit*, "current target"));
}

bool RunAwayAction::Execute(Event event)
{
    return Flee(AI_VALUE(Unit*, "master target"));
}

bool MoveToLootAction::Execute(Event event)
{
    LootObject loot = AI_VALUE(LootObject, "loot target");
    if (!loot.IsLootPossible(bot))
        return false;

    WorldObject *wo = loot.GetWorldObject(bot);
    return MoveNear(wo, sPlayerbotAIConfig.contactDistance);
}

bool MoveOutOfEnemyContactAction::Execute(Event event)
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target)
        return false;

    return MoveTo(target, sPlayerbotAIConfig.contactDistance);
}

bool MoveOutOfEnemyContactAction::isUseful()
{
    return AI_VALUE2(bool, "inside target", "current target");
}

bool SetFacingTargetAction::Execute(Event event)
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target)
        return false;

    if (bot->IsTaxiFlying()
#ifdef MANGOSBOT_ZERO
            || bot->IsFlying()
#endif
            )
        return true;

    sServerFacade.SetFacingTo(bot, target);
    ai->SetNextCheckDelay(sPlayerbotAIConfig.globalCoolDown);
    return true;
}

bool SetFacingTargetAction::isUseful()
{
    return !AI_VALUE2(bool, "facing", "current target");
}

bool SetBehindTargetAction::Execute(Event event)
{
    Unit* target = AI_VALUE(Unit*, "current target");
    if (!target)
        return false;

    float angle = GetFollowAngle() / 3 + target->GetOrientation() + M_PI;

    float distance = sPlayerbotAIConfig.contactDistance;
    float x = target->GetPositionX() + cos(angle) * distance,
        y = target->GetPositionY() + sin(angle) * distance,
        z = target->GetPositionZ();
    bot->UpdateGroundPositionZ(x, y, z);

    return MoveTo(bot->GetMapId(), x, y, z);
}

bool SetBehindTargetAction::isUseful()
{
    return !AI_VALUE2(bool, "behind", "current target");
}

bool MoveOutOfCollisionAction::Execute(Event event)
{
    float angle = M_PI * 2000 / (float)urand(1, 1000);
    float distance = sPlayerbotAIConfig.followDistance;
    return MoveTo(bot->GetMapId(), bot->GetPositionX() + cos(angle) * distance, bot->GetPositionY() + sin(angle) * distance, bot->GetPositionZ());
}

bool MoveOutOfCollisionAction::isUseful()
{
    return AI_VALUE2(bool, "collision", "self target");
}

