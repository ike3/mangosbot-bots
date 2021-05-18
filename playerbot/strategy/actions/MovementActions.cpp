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
#include "../../TravelMgr.h"
#include "../../TravelNode.h"
#include "Entities/Transports.h"

using namespace ai;

void MovementAction::CreateWp(Player* wpOwner, float x, float y, float z, float o, uint32 entry, bool important)
{
    float dist = wpOwner->GetDistance(x, y, z);
    float delay = 5000.0f; // 1000.0f * dist / wpOwner->GetSpeed(MOVE_RUN) + sPlayerbotAIConfig.reactDelay;

    //if(!important)
    //    delay *= 0.25;

    Creature* wpCreature = wpOwner->SummonCreature(entry, x, y, z - 1, o, TEMPSPAWN_TIMED_DESPAWN, delay);

    if (!important)
        wpCreature->SetObjectScale(0.2f);

}

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
    for (float angle = followAngle; angle <= followAngle + 2 * M_PI; angle += M_PI / 4.0f)
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

    //ai->TellError("All paths not in LOS");
    return false;
}

bool MovementAction::MoveTo(uint32 mapId, float x, float y, float z, bool idle, bool react)
{
    UpdateMovementState();

    bool detailedMove = ai->AllowActive(DETAILED_MOVE_ACTIVITY);

    if (!detailedMove)
    {
        time_t now = time(0);
        if (AI_VALUE(LastMovement&, "last movement").nextTeleport > now) //We can not teleport yet. Wait.
            return true;
    }

    float minDist = sPlayerbotAIConfig.targetPosRecalcDistance; //Minium distance a bot should move.
    float maxDist = sPlayerbotAIConfig.reactDistance;           //Maxium distance a bot can move in one single action.


    bool generatePath = !bot->IsFlying() && !bot->HasMovementFlag(MOVEFLAG_SWIMMING) && !bot->IsInWater() && !sServerFacade.IsUnderwater(bot);
    
    if (generatePath)
    {
        z += CONTACT_DISTANCE;
        bot->UpdateAllowedPositionZ(x, y, z);
    }    

    if (!IsMovingAllowed() && sServerFacade.UnitIsDead(bot))
    {
        bot->StopMoving();
        return false;
    }

    LastMovement& lastMove = *context->GetValue<LastMovement&>("last movement");

    WorldPosition startPosition = WorldPosition(bot);             //Current location of the bot
    WorldPosition endPosition = WorldPosition(mapId, x, y, z, 0); //The requested end location
    WorldPosition movePosition;                                   //The actual end location

    float totalDistance = startPosition.distance(endPosition);    //Total distance to where we want to go
    float maxDistChange = totalDistance * 0.1;                    //Maximum change between previous destination before needing a recalulation


    if (totalDistance < minDist)
    {
        if(lastMove.lastMoveShort.distance(endPosition) < maxDistChange)
            AI_VALUE(LastMovement&, "last movement").clear();
        bot->StopMoving();
        return false;
    }

    TravelPath movePath;

    if (lastMove.lastMoveShort.distance(endPosition) < maxDistChange && startPosition.distance(lastMove.lastMoveShort) < maxDist) //The last short movement was to the same place we want to move now.
        movePosition = endPosition;
    else if (!lastMove.lastPath.empty() && lastMove.lastPath.getBack().distance(endPosition) < maxDistChange) //The last long movement was to the same place we want to move now.
    {
        movePath = lastMove.lastPath;
    }
    else
    {
        movePosition = endPosition;

        vector<WorldPosition> beginPath, endPath;

        if (totalDistance > maxDist)
        {
            if (!sTravelNodeMap.getNodes().empty())
            {
                //[[Node pathfinding system]]
                //We try to find nodes near the bot and near the end position that have a route between them.
                //Then bot has to move towards/along the route.
                sTravelNodeMap.m_nMapMtx.lock_shared();

                //Find the route of nodes starting at a node closest to the start position and ending at a node closest to the endposition.
                //Also returns longPath: The path from the start position to the first node in the route.
                TravelNodeRoute route = sTravelNodeMap.getRoute(&startPosition, &endPosition, beginPath, bot);

                if (sPlayerbotAIConfig.hasLog("bot_pathfinding.csv"))
                {
                    sPlayerbotAIConfig.log("bot_pathfinding.csv", route.print().str().c_str());
                }

                if (route.isEmpty())
                {
                    sTravelNodeMap.m_nMapMtx.unlock_shared();
                    //We have no path. Beyond 450yd the standard pathfinder will probably move the wrong way.
                    if (sServerFacade.IsDistanceGreaterThan(totalDistance, maxDist * 3))
                    {
                        bot->StopMoving();
                        if (ai->HasStrategy("debug move", BOT_STATE_NON_COMBAT))
                            ai->TellMasterNoFacing("I have no path");
                        return false;
                    }

                    movePosition = endPosition;
                }
                else
                {
                    endPath = route.getNodes().back()->getPosition()->getPathTo(endPosition, NULL);
                    movePath = route.buildPath(beginPath, endPath);

                    if (sPlayerbotAIConfig.hasLog("bot_pathfinding.csv"))
                    {
                        sPlayerbotAIConfig.log("bot_pathfinding.csv", movePath.print().str().c_str());
                    }

                    sTravelNodeMap.m_nMapMtx.unlock_shared();
                }
            }
            else
            {
                //Use standard pathfinder to find a route. 
                movePosition = endPosition;
            }
        }
    }

    if (movePath.empty() && movePosition.distance(startPosition) > maxDist)
    {
        //Use standard pathfinder to find a route. 
        PathFinder path(bot);
        path.calculate(movePosition.getX(), movePosition.getY(), movePosition.getZ(), false);
        PathType type = path.getPathType();
        PointsArray& points = path.getPath();
        movePath.addPath(startPosition.fromPointsArray(points));
    }

    if (!movePath.empty())
    {
        movePath.makeShortCut(startPosition, maxDist);

        if (movePath.empty())
        {

            AI_VALUE(LastMovement&, "last movement").setPath(movePath);

            ai->TellMasterNoFacing("Too far from path. Rebuilding.");
            return true;
        }

        bool isTeleport, isTransport;
        uint32 entry;
        movePosition = movePath.getNextPoint(startPosition, maxDist, isTeleport, isTransport, entry);

        if (isTeleport)// && !ai->isRealPlayer())
        {           

            //Log bot movement
            if (sPlayerbotAIConfig.hasLog("bot_movement.csv"))
            {
                WorldPosition telePos;
                if (entry)
                {
                    AreaTrigger const* at = sObjectMgr.GetAreaTrigger(entry);
                    if (at)
                        telePos = WorldPosition(at->target_mapId, at->target_X, at->target_Y, at->target_Z, at->target_Orientation);
                }
                else
                    telePos = movePosition;

                ostringstream out;
                out << sPlayerbotAIConfig.GetTimestampStr() << "+00,";
                out << bot->GetName() << ",";
                if (telePos && telePos != movePosition)
                    startPosition.printWKT({ startPosition, movePosition, telePos }, out,1);
                else
                    startPosition.printWKT({ startPosition, movePosition}, out,1);

                out << to_string(bot->getRace()) << ",";
                out << to_string(bot->getClass()) << ",";
                out << bot->getLevel() << ",";
                out << (entry ? -1 : entry);

                sPlayerbotAIConfig.log("bot_movement.csv", out.str().c_str());
            }

            if (entry)
            {
                AI_VALUE(LastMovement&, "last area trigger").lastAreaTrigger = entry;
            }
            else
                return bot->TeleportTo(movePosition.getMapId(), movePosition.getX(), movePosition.getY(), movePosition.getZ(), movePosition.getO(), 0);
        }

        if (isTransport && entry)
        {
            if (!bot->GetTransport())
            {
                for (auto& transport : movePosition.getTransports(entry))
                    if (movePosition.sqDistance2d(WorldPosition((WorldObject*)transport)) < 5 * 5)
                        transport->AddPassenger(bot, true);
            }            
            WaitForReach(100.0f);
            return true;
        }
        //if (!isTransport && bot->GetTransport())
        //    bot->GetTransport()->RemovePassenger(bot);
    }

    AI_VALUE(LastMovement&, "last movement").setPath(movePath);

    if (movePosition == WorldPosition()) {
        movePath.clear();
        AI_VALUE(LastMovement&, "last movement").setPath(movePath);

        ai->TellMasterNoFacing("No point. Rebuilding.");
        return false;
    }

    //Visual waypoints
    if (ai->HasStrategy("debug move", BOT_STATE_NON_COMBAT))
    {
        if(!movePath.empty())
        {
            float cx = x;
            float cy = y;
            float cz = z;
            for (auto i : movePath.getPath())
            {
                CreateWp(bot, i.point.getX(), i.point.getY(), i.point.getZ(), 0.0, 15631);

                cx = i.point.getX();
                cy = i.point.getY();
                cz = i.point.getZ();
            }
        }
        else
            CreateWp(bot, movePosition.getX(), movePosition.getY(), movePosition.getZ(), 0, 15631, true);
    }

    //Log bot movement
    if (sPlayerbotAIConfig.hasLog("bot_movement.csv") && lastMove.lastMoveShort != movePosition)
    {
        ostringstream out;
        out << sPlayerbotAIConfig.GetTimestampStr() << "+00,";
        out << bot->GetName() << ",";
        startPosition.printWKT({ startPosition, movePosition }, out,1);
        out << to_string(bot->getRace()) << ",";
        out << to_string(bot->getClass()) << ",";
        out << bot->getLevel();
        out << 0;

        sPlayerbotAIConfig.log("bot_movement.csv", out.str().c_str());
    }

    if (!react)
        WaitForReach(startPosition.distance(movePosition)- 10.0f);

    bot->HandleEmoteState(0);
    if (bot->IsSitState())
        bot->SetStandState(UNIT_STAND_STATE_STAND);

    if (bot->IsNonMeleeSpellCasted(true))
    {
        bot->CastStop();
        ai->InterruptSpell();
    }

    MotionMaster& mm = *bot->GetMotionMaster();

#ifdef MANGOS
    mm.MovePoint(mapId, x, y, z, generatePath);
#endif
#ifdef CMANGOS
    if (lastMove.lastMoveShort.distance(movePosition) < minDist)
    {
        bot->StopMoving();
        mm.Clear();
    }

    if (!detailedMove && !ai->HasPlayerNearby(&movePosition)) //Why walk if you can fly?
    {
        time_t now = time(0);

        AI_VALUE(LastMovement&, "last movement").nextTeleport = now + (time_t)MoveDelay(startPosition.distance(movePosition));

        return bot->TeleportTo(movePosition.getMapId(), movePosition.getX(), movePosition.getY(), movePosition.getZ(), startPosition.getAngleTo(movePosition));
    }

    mm.MovePoint(movePosition.getMapId(), movePosition.getX(), movePosition.getY(), movePosition.getZ(), FORCED_MOVEMENT_RUN, generatePath);

    AI_VALUE(LastMovement&, "last movement").setShort(movePosition);            
#endif
    if (!idle)
        ClearIdleState();

    return true;
}

bool MovementAction::MoveTo(Unit* target, float distance)
{
    if (!IsMovingAllowed(target))
    {
        //ai->TellError("Seems I am stuck");
        return false;
    }

    float bx = bot->GetPositionX(), by = bot->GetPositionY(), bz = bot->GetPositionZ();
    float tx = target->GetPositionX(), ty = target->GetPositionY(), tz = target->GetPositionZ();

    if (sServerFacade.IsHostileTo(bot, target))
    {
        Stance* stance = AI_VALUE(Stance*, "stance");
        WorldLocation loc = stance->GetLocation();
        if (Formation::IsNullLocation(loc) || loc.mapid == -1)
        {
            //ai->TellError("Nowhere to move");
            return false;
        }

        tx = loc.coord_x;
        ty = loc.coord_y;
        tz = loc.coord_z;
}

    float distanceToTarget = sServerFacade.GetDistance2d(bot, tx, ty);
    if (sServerFacade.IsDistanceGreaterThan(distanceToTarget, sPlayerbotAIConfig.targetPosRecalcDistance))
    {
        float angle = bot->GetAngle(tx, ty);
        float needToGo = distanceToTarget - distance;

        float maxDistance = ai->GetRange("spell");
        if (needToGo > 0 && needToGo > maxDistance)
            needToGo = maxDistance;
        else if (needToGo < 0 && needToGo < -maxDistance)
            needToGo = -maxDistance;

        float dx = cos(angle) * needToGo + bx;
        float dy = sin(angle) * needToGo + by;
        float dz = bz + (tz - bz) * needToGo / distanceToTarget;

        return MoveTo(target->GetMapId(), dx, dy, dz);
    }

    return true;
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
    if (!bot->InBattleGround() && distance > sPlayerbotAIConfig.reactDistance)
        return false;

    return IsMovingAllowed();
}

bool MovementAction::IsMovingAllowed(uint32 mapId, float x, float y, float z)
{
    float distance = bot->GetDistance(x, y, z);
    if (!bot->InBattleGround() && distance > sPlayerbotAIConfig.reactDistance)
        return false;

    return IsMovingAllowed();
}

bool MovementAction::IsMovingAllowed()
{
    if (sServerFacade.IsFrozen(bot) || bot->IsPolymorphed() ||
			(sServerFacade.UnitIsDead(bot) && !bot->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST)) ||
            bot->IsBeingTeleported() ||
            sServerFacade.IsInRoots(bot) ||
            bot->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION) ||
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

    // Temporary speed increase in group
    if (ai->hasRealPlayerMaster())
        bot->UpdateSpeed(MOVE_RUN, true, 1.1f);
}

bool MovementAction::Follow(Unit* target, float distance, float angle)
{
    MotionMaster &mm = *bot->GetMotionMaster();

    UpdateMovementState();

    if (!target)
        return false;

    if (!bot->InBattleGround() && sServerFacade.IsDistanceLessOrEqualThan(sServerFacade.GetDistance2d(bot, target), sPlayerbotAIConfig.followDistance))
    {
        //ai->TellError("No need to follow");
        return false;
    }

    if (!bot->InBattleGround() 
     && sServerFacade.IsDistanceLessOrEqualThan(sServerFacade.GetDistance2d(bot, target->GetPositionX(), target->GetPositionY()), sPlayerbotAIConfig.sightDistance)
     && abs(bot->GetPositionZ() - target->GetPositionZ()) >= sPlayerbotAIConfig.spellDistance
     && ai->hasRealPlayerMaster()
     && (target->GetMapId() && bot->GetMapId() != target->GetMapId()))
    {
        bot->StopMoving();
        mm.Clear();
        float x = bot->GetPositionX(), y = bot->GetPositionY(), z = target->GetPositionZ();
        if (target->GetMapId() && bot->GetMapId() != target->GetMapId())
        {
#ifdef MANGOSBOT_ZERO
            if ((target->GetMap() && target->GetMap()->IsBattleGround()) || (bot->GetMap() && bot->GetMap()->IsBattleGround()))
#else
            if ((target->GetMap() && target->GetMap()->IsBattleGroundOrArena()) || (bot->GetMap() && bot->GetMap()->IsBattleGroundOrArena()))
#endif
                return false;

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

    if (!IsMovingAllowed(target)
        && ai->hasRealPlayerMaster())
    {
#ifdef MANGOSBOT_ZERO
        if ((target->GetMap() && target->GetMap()->IsBattleGround()) || (bot->GetMap() && bot->GetMap()->IsBattleGround()))
#else
        if((target->GetMap() && target->GetMap()->IsBattleGroundOrArena()) || (bot->GetMap() && bot->GetMap()->IsBattleGroundOrArena()))
#endif
            return false;

        if (sServerFacade.UnitIsDead(bot) && sServerFacade.IsAlive(ai->GetMaster()))
        {
            bot->ResurrectPlayer(1.0f, false);
            ai->TellMasterNoFacing("I live, again!");
        }
        else
            ai->TellError("I am stuck while following");

        bot->CombatStop(true);
        bot->TeleportTo(target->GetMapId(), target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation());
        return false;
    }

    if (sServerFacade.IsDistanceLessOrEqualThan(sServerFacade.GetDistance2d(bot, target), sPlayerbotAIConfig.followDistance))
    {
        //ai->TellError("No need to follow");
        return false;
    }

    if (sServerFacade.IsFriendlyTo(target, bot) && bot->IsMounted() && AI_VALUE(list<ObjectGuid>, "all targets").empty())
        distance += angle;

    bot->HandleEmoteState(0);
    if (bot->IsSitState())
        bot->SetStandState(UNIT_STAND_STATE_STAND);

    if (bot->IsNonMeleeSpellCasted(true))
    {
        bot->CastStop();
        ai->InterruptSpell();
    }

    //if (sServerFacade.isMoving(bot))
    //    return false;

    AI_VALUE(LastMovement&, "last movement").Set(target);
    ClearIdleState();

    if (bot->GetMotionMaster()->GetCurrentMovementGeneratorType() == FOLLOW_MOTION_TYPE)
    {
        Unit *currentTarget = sServerFacade.GetChaseTarget(bot);
        if (currentTarget && currentTarget->GetObjectGuid() == target->GetObjectGuid()) return false;
    }

    //if(mm.GetCurrent()->GetMovementGeneratorType() != FOLLOW_MOTION_TYPE)
        mm.Clear();

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

bool MovementAction::ChaseTo(WorldObject* obj)
{
    if (bot->IsSitState())
        bot->SetStandState(UNIT_STAND_STATE_STAND);

    if (bot->IsNonMeleeSpellCasted(true))
    {
        bot->CastStop();
        ai->InterruptSpell();
    }

    MotionMaster &mm = *bot->GetMotionMaster();
    mm.Clear();

    mm.MoveChase((Unit*)obj);
    return true;
}

float MovementAction::MoveDelay(float distance)
{
    return distance / bot->GetSpeed(MOVE_RUN);
}

void MovementAction::WaitForReach(float distance)
{
    float delay = 1000.0f * MoveDelay(distance) + sPlayerbotAIConfig.reactDelay;

    if (delay > sPlayerbotAIConfig.maxWaitForMove)
        delay = sPlayerbotAIConfig.maxWaitForMove;

    Unit* target = *ai->GetAiObjectContext()->GetValue<Unit*>("current target");
    Unit* player = *ai->GetAiObjectContext()->GetValue<Unit*>("enemy player target");
    if ((player || target) && delay > sPlayerbotAIConfig.globalCoolDown)
        delay = sPlayerbotAIConfig.globalCoolDown;

    if (delay < 0)
        delay = 0;

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
    bool foundFlee = false;
    bool isTarget = false;
    time_t lastFlee = AI_VALUE(LastMovement&, "last movement").lastFlee;
    //HostileReference *ref = target->GetThreatManager().getCurrentVictim();
    HostileReference *ref = sServerFacade.GetThreatManager(target).getCurrentVictim();
    if (ref && ref->getTarget() == bot)
    {
        isTarget = true;
        Group *group = bot->GetGroup();
        if (group)
        {
            for (GroupReference *gref = group->GetFirstMember(); gref; gref = gref->next())
            {
                Player* player = gref->getSource();
                if (!player || player == bot) continue;
                if (ai->IsTank(player))
                {
                    float distanceToTank = sServerFacade.GetDistance2d(bot, player);
                    float distanceToTarget = sServerFacade.GetDistance2d(bot, target);
                    if (sServerFacade.IsDistanceGreaterThan(distanceToTank, distanceToTarget))
                    {
                        foundFlee = MoveTo(player, sPlayerbotAIConfig.followDistance);
                    }
                }

                if (!foundFlee && master)
                {
                    foundFlee = MoveTo(master, sPlayerbotAIConfig.followDistance);
                }

                if (foundFlee)
                {
                    if (!urand(0, 25))
                    {
                        vector<uint32> sounds;
                        sounds.push_back(304); // guard
                        sounds.push_back(306); // flee
                        ai->PlaySound(sounds[urand(0, sounds.size() - 1)]);
                    }
                    return true;
                }
            }
        }
    }

    if ((foundFlee || lastFlee) && bot->GetGroup())
    {
        uint32 fleeDelay = sPlayerbotAIConfig.returnDelay / 1000;
        time_t now = time(0);
        if (!lastFlee)
        {
            AI_VALUE(LastMovement&, "last movement").lastFlee = now;
        }
        else
        {
            if ((now - lastFlee) > urand(5, fleeDelay * 2))
            {
                AI_VALUE(LastMovement&, "last movement").lastFlee = 0;
            }
            else
                return false;
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
    if (result)
        AI_VALUE(LastMovement&, "last movement").lastFlee = time(0);
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

    if (bot->IsTaxiFlying()|| bot->IsFlying())
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

bool SetBehindTargetAction::isPossible()
{
    Unit* target = AI_VALUE(Unit*, "current target");
#ifdef MANGOS
    return target && !(target->getVictim() && target->getVictim()->GetObjectGuid() == bot->GetObjectGuid());
#endif
#ifdef CMANGOS
    return target && !(target->GetVictim() && target->GetVictim()->GetObjectGuid() == bot->GetObjectGuid());
#endif
}

bool MoveOutOfCollisionAction::Execute(Event event)
{
    float angle = M_PI * 2000 / (float)urand(1, 1000);
    float distance = sPlayerbotAIConfig.followDistance;
    return MoveTo(bot->GetMapId(), bot->GetPositionX() + cos(angle) * distance, bot->GetPositionY() + sin(angle) * distance, bot->GetPositionZ());
}

bool MoveOutOfCollisionAction::isUseful()
{
    return AI_VALUE2(bool, "collision", "self target") && ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest friendly players")->Get().size() < 15 &&
        ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest non bot players")->Get().size() > 0;
}

bool MoveRandomAction::Execute(Event event)
{
    //uint32 randnum = bot->GetGUIDLow();                            //Semi-random but fixed number for each bot.
    //uint32 cycle = floor(WorldTimer::getMSTime() / (1000*60));     //Semi-random number adds 1 each minute.

    //randnum = ((randnum + cycle) % 1000) + 1;

    uint32 randnum = urand(1, 2000);

    float angle = M_PI  * (float)randnum / 1000; //urand(1, 1000);
    float distance = urand(20,200);

    return MoveTo(bot->GetMapId(), bot->GetPositionX() + cos(angle) * distance, bot->GetPositionY() + sin(angle) * distance, bot->GetPositionZ());
}

bool MoveRandomAction::isUseful()
{    
    return !ai->hasRealPlayerMaster() && ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest friendly players")->Get().size() > urand(25, 100);
}
