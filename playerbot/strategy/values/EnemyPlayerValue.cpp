#include "botpch.h"
#include "../../playerbot.h"
#include "EnemyPlayerValue.h"
#include "TargetValue.h"

using namespace ai;
using namespace std;

Unit* EnemyPlayerValue::Calculate()
{
    // Prioritize the duel opponent
    if(bot->duel && bot->duel->opponent && !sServerFacade.IsFriendlyTo(bot->duel->opponent, bot))
    {
        return bot->duel->opponent;
    }

    Unit* bestEnemyPlayer = nullptr;
    float bestEnemyPlayerDistance = 999999999.0f;
    uint32 bestEnemyPlayerHealth = 99999999;

    //const float maxPvPDistance = GetMaxAttackDistance(bot);
    //const bool inCannon = ai->IsInVehicle(false, true);
    const bool isMelee = !ai->IsRanged(bot);

    list<ObjectGuid> possibleAttackTargets = AI_VALUE(list<ObjectGuid>, "possible attack targets");
    for (const ObjectGuid& targetGuid : possibleAttackTargets)
    {
        Unit* target = ai->GetUnit(targetGuid);
        if(target)
        {
            Player* enemyPlayer = dynamic_cast<Player*>(target);
            if (enemyPlayer)
            {
                /*
                // Check if too far away (Do we need this?)
                uint32 const pvpDistance = (inCannon || bot->GetHealth() > enemyPlayer->GetHealth()) ? maxPvPDistance : 20.0f;
                if (!bot->IsWithinDist(enemyPlayer, pvpDistance, false))
                {
                    continue;
                }
                */

                // Prioritize an enemy player if it has a battleground flag
                if ((bot->GetTeam() == HORDE && enemyPlayer->HasAura(23333)) || 
                    (bot->GetTeam() == ALLIANCE && enemyPlayer->HasAura(23335)))
                {
                    bestEnemyPlayer = enemyPlayer;
                    break;
                }

                if (isMelee)
                {
                    // Score best enemy player based on lowest distance
                    const float distanceToEnemyPlayer = enemyPlayer->GetDistance(bot, false);
                    if (distanceToEnemyPlayer < bestEnemyPlayerDistance)
                    {
                        bestEnemyPlayerDistance = distanceToEnemyPlayer;
                        bestEnemyPlayer = enemyPlayer;
                    }
                }
                else
                {
                    // Score best enemy player based on lowest health
                    const uint32 enemyPlayerHealth = enemyPlayer->GetHealth();
                    if (enemyPlayerHealth < bestEnemyPlayerHealth)
                    {
                        bestEnemyPlayerHealth = enemyPlayerHealth;
                        bestEnemyPlayer = enemyPlayer;
                    }
                }
            }
        }
    }

    return bestEnemyPlayer;
}

/*
float EnemyPlayerValue::GetMaxAttackDistance(Player* bot)
{
    if (!bot->GetBattleGround())
        return 60.0f;

    if (bot->InBattleGround())
    {
        BattleGround* bg = bot->GetBattleGround();
        if (!bg)
            return 40.0f;

        BattleGroundTypeId bgType = bg->GetTypeId();

#ifdef MANGOSBOT_TWO
        if (bgType == BATTLEGROUND_RB)
            bgType = bg->GetTypeId(true);

        if (bgType == BATTLEGROUND_IC)
        {
            if (bot->GetPlayerbotAI()->IsInVehicle(false, true))
                return 120.0f;
        }
#endif
        if (bgType == BATTLEGROUND_AV)
        {
            bool strifeTime = bg->GetStartTime() < (uint32)(20 * MINUTE * IN_MILLISECONDS);
            return strifeTime ? 40.0f : 10.0f;
        }
    }

    return 40.0f;
}
*/