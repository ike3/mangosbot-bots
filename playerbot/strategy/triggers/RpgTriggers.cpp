#include "botpch.h"
#include "../../playerbot.h"
#include "RpgTriggers.h"

#include "../../PlayerbotAIConfig.h"
using namespace ai;

bool NoRpgTargetTrigger::IsActive()
{
    return !context->GetValue<ObjectGuid>("rpg target")->Get();
}

bool FarFromRpgTargetTrigger::IsActive()
{
    ObjectGuid unit = context->GetValue<ObjectGuid>("rpg target")->Get();
    if (!unit) return false;

    float distance = AI_VALUE2(float, "distance", "rpg target");
	if (sPlayerbotAIConfig.RandombotsWalkingRPGInDoors)
	{
#ifdef CMANGOS
		if (!bot->GetTerrain()->IsOutdoors(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ()))
#endif
#ifdef MANGOS
		if (!bot->GetMap()->GetTerrain()->IsOutdoors(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ()))
#endif

		{
            bot->m_movementInfo.AddMovementFlag(MOVEFLAG_WALK_MODE);
		}
	}
    return distance > sPlayerbotAIConfig.followDistance;
}
