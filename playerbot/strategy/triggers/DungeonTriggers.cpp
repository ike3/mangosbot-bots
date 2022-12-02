#include "DungeonTriggers.h"
#include "../values/PositionValue.h"

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
                if (attacker->GetEntry() == bossID)
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

bool CloseToGameObject::IsActive()
{
    // If the bot is ready
    if (bot->IsInWorld() && !bot->IsBeingTeleported())
    {
        AiObjectContext* context = ai->GetAiObjectContext();

        // This has a maximum range equal to the sight distance on config file (default 60 yards)
        list<ObjectGuid> gameObjects = AI_VALUE2(list<ObjectGuid>, "nearest game objects no los", gameObjectID);
        for (ObjectGuid& gameObjectGuid : gameObjects)
        {
            GameObject* gameObject = ai->GetGameObject(gameObjectGuid);
            if (gameObject)
            {
                if (bot->IsWithinDist(gameObject, range))
                {
                    return true;
                }
            }
        }
    }

    return false;
}
