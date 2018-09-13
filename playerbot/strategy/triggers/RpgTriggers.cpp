#include "botpch.h"
#include "../../playerbot.h"
#include "RpgTriggers.h"

#include "../../PlayerbotAIConfig.h"
using namespace ai;

bool NoRpgTargetTrigger::IsActive()
{
    return !context->GetValue<Unit*>("rpg target")->Get() && !AI_VALUE(list<ObjectGuid>, "possible rpg targets").empty();
}

bool FarFromRpgTargetTrigger::IsActive()
{
    Unit* unit = context->GetValue<Unit*>("rpg target")->Get();
    if (!unit) return false;

    float distance = AI_VALUE2(float, "distance", "rpg target");
    return distance > sPlayerbotAIConfig.followDistance;
}
