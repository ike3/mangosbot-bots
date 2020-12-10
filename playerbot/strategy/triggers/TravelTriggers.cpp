#include "botpch.h"
#include "../../playerbot.h"
#include "TravelTriggers.h"

#include "../../PlayerbotAIConfig.h"
using namespace ai;

bool NoTravelTargetTrigger::IsActive()
{
    return !context->GetValue<ObjectGuid>("travel target")->Get();
}

bool FarFromTravelTargetTrigger::IsActive()
{
    ObjectGuid unit = context->GetValue<ObjectGuid>("travel target")->Get();
    if (!unit) return false;

    float distance = AI_VALUE2(float, "distance", "travel target");
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
    return distance > sPlayerbotAIConfig.tooCloseDistance;
}
