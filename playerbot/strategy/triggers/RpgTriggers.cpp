#include "botpch.h"
#include "../../playerbot.h"
#include "RpgTriggers.h"

#include "../../PlayerbotAIConfig.h"
using namespace ai;

bool NoRpgTargetTrigger::IsActive()
{
    return !context->GetValue<ObjectGuid>("rpg target")->Get() && !AI_VALUE(list<ObjectGuid>, "possible rpg targets").empty();
}

bool FarFromRpgTargetTrigger::IsActive()
{
    ObjectGuid unit = context->GetValue<ObjectGuid>("rpg target")->Get();
    if (!unit) return false;

    float distance = AI_VALUE2(float, "distance", "rpg target");
	if (sPlayerbotAIConfig.RandombotsWalkingRPGInDoors)
	{
		if (!bot->GetTerrain()->IsOutdoors(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ()))
		{
#ifdef MANGOSBOT_ONE
			bot->m_movementInfo->AddMovementFlag(MOVEFLAG_WALK_MODE);
#else
            bot->m_movementInfo.AddMovementFlag(MOVEFLAG_WALK_MODE);
#endif
		}
	}
    return distance > sPlayerbotAIConfig.followDistance;
}
