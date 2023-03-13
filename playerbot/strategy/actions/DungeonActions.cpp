#include "DungeonActions.h"
#include "../values/PositionValue.h"
#include "../AiObjectContext.h"
#include "../../PlayerbotAI.h"

using namespace ai;

bool MoveAwayFromHazard::Execute(Event& event)
{
    list<HazardPosition> hazards = AI_VALUE(list<HazardPosition>, "hazards");

    // Get the closest hazard to move away from
    const HazardPosition* closestHazard = nullptr;
    float closestHazardDistance = 9999.0f;
    for (const HazardPosition& hazard : hazards)
    {
        const WorldPosition& hazardPosition = hazard.first;
        const float distance = bot->GetDistance(hazardPosition.getX(), hazardPosition.getY(), hazardPosition.getZ());
        if (distance < closestHazardDistance)
        {
            closestHazardDistance = distance;
            closestHazard = &hazard;
        }
    }

    if (closestHazard)
    {
        // Check if the bot is inside the closest hazard
        const float hazardRadius = closestHazard->second;
        if (closestHazardDistance <= hazardRadius)
        {
            float angle = 0.0f;
            const WorldPosition initialPosition(closestHazard->first);
            const float distance = frand(hazardRadius, hazardRadius * 1.5f);

            Unit* currentTarget = AI_VALUE(Unit*, "current target");
            if (currentTarget)
            {
                const int8 startDir = urand(0, 1) * 2 - 1;
                const WorldPosition targetPosition(currentTarget);
                angle = targetPosition.getAngleTo(initialPosition) + (0.5 * M_PI_F * startDir);
            }
            else
            {
                angle = frand(0, M_PI_F * 2.0f);
            }

            const uint8 attempts = 10;
            float angleIncrement = (float)((2 * M_PI) / attempts);

            for (uint8 i = 0; i < attempts; i++)
            {
                WorldPosition point = initialPosition + WorldPosition(0, distance * cos(angle), distance * sin(angle), 1.0f);
                point.setZ(point.getHeight());

                // Check if the point is not near other hazards
                if (!IsHazardNearby(point, hazards))
                {
                    if (bot->IsWithinLOS(point.getX(), point.getY(), point.getZ() + bot->GetCollisionHeight()) && initialPosition.canPathTo(point, bot))
                    {
                        if (ai->HasStrategy("debug move", BotState::BOT_STATE_COMBAT))
                        {
                            Creature* wpCreature = bot->SummonCreature(15631, point.getX(), point.getY(), point.getZ(), 0.0f, TEMPSPAWN_TIMED_DESPAWN, 5000.0f + angle * 1000.0f);
                        }

                        if (MoveTo(bot->GetMapId(), point.getX(), point.getY(), point.getZ(), false, IsReaction(), false, true))
                        {
                            if (IsReaction())
                            {
                                WaitForReach(point.distance(initialPosition));
                            }

                            return true;
                        }
                    }
                }

                if (ai->HasStrategy("debug move", BotState::BOT_STATE_COMBAT))
                {
                    Creature* wpCreature = bot->SummonCreature(1, point.getX(), point.getY(), point.getZ(), 0.0f, TEMPSPAWN_TIMED_DESPAWN, 5000.0f + angle * 1000.0f);
                }

                angle += angleIncrement;
            }
        }
    }

    return false;
}

bool MoveAwayFromHazard::isPossible()
{
    if (MovementAction::isPossible())
    {
        return ai->CanMove();
    }

    return false;
}

bool MoveAwayFromHazard::IsHazardNearby(const WorldPosition& point, const list<HazardPosition>& hazards) const
{
    for (const HazardPosition& hazard : hazards)
    {
        const float hazardRange = hazard.second;
        const WorldPosition& hazardPosition = hazard.first;
        const float distance = point.distance(hazardPosition);
        if (distance < hazardRange)
        {
            return true;
        }
    }

    return false;
}

bool MoveAwayFromCreature::Execute(Event& event)
{
    // Get the active attacking creatures
    list<Creature*> creatures;
    size_t closestCreatureIdx = 0;
    float closestCreatureDistance = 9999.0f;
    list<ObjectGuid> attackers = AI_VALUE(list<ObjectGuid>, "attackers");
    for (ObjectGuid& attackerGuid : attackers)
    {
        // Check against the given creature id
        if (attackerGuid.GetEntry() == creatureID)
        {
            Creature* creature = ai->GetCreature(attackerGuid);
            if (creature)
            {
                creatures.push_back(creature);

                // Get the closest creature to the bot
                const float distance = bot->GetDistance(creature);
                if (distance < closestCreatureDistance)
                {
                    closestCreatureDistance = distance;
                    closestCreatureIdx = creatures.size() - 1;
                }
            }
        }
    }

    list<HazardPosition> hazards = AI_VALUE(list<HazardPosition>, "hazards");

    // Get the closest creature reference
    auto it = creatures.begin();
    advance(it, closestCreatureIdx);
    Creature* closestCreature = *it;
    // Remove the closest creature from the list to prevent checking it twice
    creatures.erase(it);

    // Generate the initial angle directly behind the bot looking at the closest creature
    const WorldPosition botPosition(bot);
    const WorldPosition creaturePosition(closestCreature);
    float angle = creaturePosition.getAngleTo(botPosition);

    const uint8 attempts = 20;
    float angleIncrement = (float)((2 * M_PI) / attempts);

    const float sizeFactor = bot->GetCombatReach() + closestCreature->GetCombatReach();
    const float distance = (range + sizeFactor);

    for (uint8 i = 0; i < attempts; i++)
    {
        WorldPosition point = creaturePosition + WorldPosition(0, distance * cos(angle), distance * sin(angle), 1.0f);
        point.setZ(point.getHeight());

        // Check if the point is not near other game objects
        if (!HasCreaturesNearby(point, creatures) && !IsHazardNearby(point, hazards))
        {
            if (bot->IsWithinLOS(point.getX(), point.getY(), point.getZ() + bot->GetCollisionHeight()) && botPosition.canPathTo(point, bot))
            {
                if (ai->HasStrategy("debug move", BotState::BOT_STATE_COMBAT))
                {
                    Creature* wpCreature = bot->SummonCreature(15631, point.getX(), point.getY(), point.getZ(), 0.0f, TEMPSPAWN_TIMED_DESPAWN, 5000.0f + angle * 1000.0f);
                }

                if (MoveTo(bot->GetMapId(), point.getX(), point.getY(), point.getZ(), false, IsReaction(), false, true))
                {
                    if (IsReaction())
                    {
                        WaitForReach(point.distance(botPosition));
                    }

                    return true;
                }
            }
        }

        if (ai->HasStrategy("debug move", BotState::BOT_STATE_COMBAT))
        {
            Creature* wpCreature = bot->SummonCreature(1, point.getX(), point.getY(), point.getZ(), 0.0f, TEMPSPAWN_TIMED_DESPAWN, 5000.0f + angle * 1000.0f);
        }

        angle += angleIncrement;
    }

    return false;
}

bool MoveAwayFromCreature::isPossible()
{
    if (MovementAction::isPossible())
    {
        return ai->CanMove();
    }

    return false;
}

bool MoveAwayFromCreature::HasCreaturesNearby(const WorldPosition& point, const list<Creature*>& creatures) const
{
    for (const Creature* creature : creatures)
    {
        const float distance = creature->GetDistance(point.getX(), point.getY(), point.getZ());
        if (distance <= range)
        {
            return true;
        }
    }

    return false;
}

bool MoveAwayFromCreature::IsHazardNearby(const WorldPosition& point, const list<HazardPosition>& hazards) const
{
    for (const HazardPosition& hazard : hazards)
    {
        const float hazardRange = hazard.second;
        const WorldPosition& hazardPosition = hazard.first;
        const float distance = point.distance(hazardPosition);
        if (distance < hazardRange)
        {
            return true;
        }
    }

    return false;
}