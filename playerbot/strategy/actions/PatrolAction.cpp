#include "botpch.h"
#include "../../playerbot.h"
#include "PatrolAction.h"

#include "../../FleeManager.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"
#include "../values/Formations.h"
#include "../values/PositionValue.h"
#include "../values/PossibleRpgTargetsValue.h"

using namespace ai;

bool PatrolAction::Execute(Event event)
{
    float bx = bot->GetPositionX();
    float by = bot->GetPositionY();
    float bz = bot->GetPositionZ();
    float mapId = bot->GetMapId();

    ai::Position pos = context->GetValue<ai::PositionMap&>("position")->Get()["patrol"];
    if (pos.isSet() && pos.mapId == mapId)
    {
        float distanceToTarget = sServerFacade.GetDistance2d(bot, pos.x, pos.y);
        float distance = sPlayerbotAIConfig.meleeDistance;
        if (sServerFacade.IsDistanceGreaterThan(distanceToTarget, distance) &&
                sServerFacade.IsDistanceLessThan(distanceToTarget, sPlayerbotAIConfig.reactDistance))
        {
            float angle = bot->GetAngle(pos.x, pos.y);
            float needToGo = distanceToTarget - distance;

            float maxDistance = ai->GetRange("spell");
            if (needToGo > 0 && needToGo > maxDistance)
                needToGo = maxDistance;
            else if (needToGo < 0 && needToGo < -maxDistance)
                needToGo = -maxDistance;

            float dx = cos(angle) * needToGo + bx;
            float dy = sin(angle) * needToGo + by;
            float dz = bz + (pos.z - bz) * needToGo / distanceToTarget;
            if (abs(bz - dz) < sPlayerbotAIConfig.farDistance)
            {
                bool moved = MoveTo(pos.mapId, dx, dy, dz);
                if (moved)
                {
                    AI_VALUE(LastMovement&, "last movement").Set(pos.x, pos.y, pos.z, bot->GetOrientation());
                    ClearIdleState();
                    ai->SetNextCheckDelay(sPlayerbotAIConfig.maxWaitForMove);
                    return true;
                }
            }
        }
    }

    UpdateDestination();
    return true;
}

bool PatrolAction::isUseful()
{
    return (!context->GetValue<ObjectGuid>("rpg target")->Get() && !context->GetValue<Unit*>("grind target")->Get()) &&
            (!AI_VALUE(list<ObjectGuid>, "nearest non bot players").empty() &&
            AI_VALUE2(uint8, "health", "self target") > sPlayerbotAIConfig.mediumHealth &&
            (!AI_VALUE2(uint8, "mana", "self target") || AI_VALUE2(uint8, "mana", "self target") > sPlayerbotAIConfig.mediumMana));
}

void PatrolAction::UpdateDestination()
{
    float bx = bot->GetPositionX();
    float by = bot->GetPositionY();
    float bz = bot->GetPositionZ();
    float mapId = bot->GetMapId();

    ai::PositionMap& posMap = context->GetValue<ai::PositionMap&>("position")->Get();
    ai::Position pos = posMap["patrol"];

    float angle = urand(0, M_PI * 2000) / 1000.0f;
    float dist = sPlayerbotAIConfig.patrolDistance;
    float tx = bx + cos(angle) * dist;
    float ty = by + sin(angle) * dist;
    float tz = bot->GetMap()->GetTerrain()->GetHeightStatic(tx, ty, bz);
    if (abs(tz - bz) > sPlayerbotAIConfig.farDistance) return;

    Formation::UpdateAllowedPositionZ(bot, tx, ty, tz);
    pos.Set(tx, ty, tz, mapId);
    posMap[name] = pos;
}