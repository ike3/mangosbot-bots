#include "botpch.h"
#include "../../playerbot.h"
#include "DungeonTriggers.h"
#include "../values/PositionValue.h"
#include "ServerFacade.h"
#include "../AiObjectContext.h"
#include "../values/HazardsValue.h"
#include "../actions/MovementActions.h"

using namespace ai;

bool EnterDungeonTrigger::IsActive()
{
    // Don't trigger if strategy already set
    if (!ai->HasStrategy(dungeonStrategy, BotState::BOT_STATE_COMBAT))
    {
        // If the bot is ready
        if (bot->IsInWorld() && !bot->IsBeingTeleported())
        {
            // If the bot is on the specified dungeon
            Map* map = bot->GetMap();
            if (map && (map->IsDungeon() || map->IsRaid()))
            {
                return map->GetId() == mapID;
            }
        }
    }

    return false;
}

bool LeaveDungeonTrigger::IsActive()
{
    // Don't trigger if strategy already unset
    if (ai->HasStrategy(dungeonStrategy, BotState::BOT_STATE_COMBAT))
    {
        // If the bot is ready
        if (bot->IsInWorld() && !bot->IsBeingTeleported())
        {
            // If the bot is not on the specified dungeon
            return bot->GetMapId() != mapID;
        }
    }

    return false;
}

bool StartBossFightTrigger::IsActive()
{
    // Don't trigger if strategy already set
    if (!ai->HasStrategy(bossStrategy, BotState::BOT_STATE_COMBAT))
    {
        // If the bot is ready
        if (bot->IsInWorld() && !bot->IsBeingTeleported())
        {
            AiObjectContext* context = ai->GetAiObjectContext();
            const list<ObjectGuid> attackers = AI_VALUE(list<ObjectGuid>, "attackers");
            for (const ObjectGuid& attackerGuid : attackers)
            {
                Unit* attacker = ai->GetUnit(attackerGuid);
                if (attacker && attacker->GetEntry() == bossID)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool EndBossFightTrigger::IsActive()
{
    // Don't trigger if strategy already unset
    if (ai->HasStrategy(bossStrategy, BotState::BOT_STATE_COMBAT))
    {
        // We consider the fight is over if not in combat
        return !ai->IsStateActive(BotState::BOT_STATE_COMBAT);
    }

    return false;
}

bool CloseToGameObjectHazard::IsActive()
{
    // If the bot is ready
    bool closeToHazard = false;
    if (bot->IsInWorld() && !bot->IsBeingTeleported())
    {
        AiObjectContext* context = ai->GetAiObjectContext();

        // This has a maximum range equal to the sight distance on config file (default 60 yards)
        const list<ObjectGuid>& gameObjects = AI_VALUE2(list<ObjectGuid>, "nearest game objects no los", gameObjectID);
        for (const ObjectGuid& gameObjectGuid : gameObjects)
        {
            GameObject* gameObject = ai->GetGameObject(gameObjectGuid);
            if (gameObject)
            {
                const float distance = bot->GetDistance(gameObject) + gameObject->GetObjectBoundingRadius();
                if (distance <= radius)
                {
                    closeToHazard = true;
                }

                // Cache the hazards
                Hazard hazard(gameObjectGuid, expirationTime, radius);
                SET_AI_VALUE(Hazard, "add hazard", std::move(hazard));
            }
        }
    }

    // Don't trigger if the bot is moving
    if (closeToHazard)
    {
        const Action* lastExecutedAction = ai->GetLastExecutedAction(BotState::BOT_STATE_COMBAT);
        if (lastExecutedAction)
        {
            const MovementAction* movementAction = dynamic_cast<const MovementAction*>(lastExecutedAction);
            if (movementAction)
            {
                closeToHazard = false;
            }
        }
    }

    return closeToHazard;
}

bool CloseToCreature::IsActive()
{
    // If the bot is ready
    if (bot->IsInWorld() && !bot->IsBeingTeleported())
    {
        AiObjectContext* context = ai->GetAiObjectContext();

        // Iterate through the active attackers
        list<ObjectGuid> attackers = AI_VALUE(list<ObjectGuid>, "attackers");
        for (ObjectGuid& attackerGuid : attackers)
        {
            // Check against the given creature id
            if (attackerGuid.GetEntry() == creatureID)
            {
                Creature* creature = ai->GetCreature(attackerGuid);
                if (creature)
                {
                    // Check if the bot is not being targeted by the creature
                    if (!creature->GetVictim() || (creature->GetVictim()->GetObjectGuid() != bot->GetObjectGuid()))
                    {
                        // See if the creature is within the specified distance
                        if (bot->IsWithinDist(creature, range))
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}