#include "DungeonActions.h"
#include "../values/PositionValue.h"

using namespace ai;

bool MoveAwayFromGameObject::Execute(Event& event)
{
    // This has a maximum range equal to the sight distance on config file (default 60 yards)
    list<GameObject*> gameObjects;
    for (const ObjectGuid& gameObjectGuid : AI_VALUE2(list<ObjectGuid>, "nearest game objects no los", gameObjectID))
    {
        GameObject* gameObject = ai->GetGameObject(gameObjectGuid);
        if (gameObject)
        {
            gameObjects.push_back(gameObject);
        }
    }

    float angle = 0.0f;
    const WorldPosition initialPosition(bot);
    const float distance = frand(range, range * 1.5f);

    Unit* currentTarget = AI_VALUE(Unit*, "current target");
    if (currentTarget)
    {
        const int8 startDir = urand(0, 1) * 2 - 1;
        const WorldPosition targetPosition(currentTarget);
        angle = targetPosition.getAngleTo(initialPosition) + (0.5 * M_PI_F * startDir);
    }
    else
        angle = frand(0, M_PI_F * 2.0f);

    for (uint8 i = 0; i < 10; i++)
    {
        WorldPosition point = initialPosition + WorldPosition(0, distance + cos(angle), distance * sin(angle), 1.0f);
        point.setZ(point.getHeight());

        if (ai->HasStrategy("debug move", BotState::BOT_STATE_COMBAT))
        {
            Creature* wpCreature = bot->SummonCreature(1, point.getX(), point.getY(), point.getZ(), 0.0f, TEMPSPAWN_TIMED_DESPAWN, 5000.0f + angle * 1000.0f);
        }

        // Check if the point is not surrounded by other game objects
        if (i == 10 || !HasGameObjectNearby(point, gameObjects))
        {
            if (bot->IsWithinLOS(point.getX(), point.getY(), point.getZ() + bot->GetCollisionHeight()) && initialPosition.canPathTo(point, bot))
            {
                if (ai->HasStrategy("debug move", BotState::BOT_STATE_COMBAT))
                {
                    Creature* wpCreature = bot->SummonCreature(15631, point.getX(), point.getY(), point.getZ(), 0.0f, TEMPSPAWN_TIMED_DESPAWN, 5000.0f + angle * 1000.0f);
                }

                if (MoveTo(bot->GetMapId(), point.getX(), point.getY(), point.getZ()))
                {
                    return true;
                }
            }
        }

        angle = frand(0, M_PI_F * 2.0f);
    }

    return false;
}

bool MoveAwayFromGameObject::HasGameObjectNearby(const WorldPosition& point, const list<GameObject*>& gameObjects) const
{
    for (const GameObject* gameObject : gameObjects)
    {
        const float distance = gameObject->GetDistance(point.getX(), point.getY(), point.getZ());
        if (distance <= range)
        {
            return true;
        }
    }

    return false;
}
