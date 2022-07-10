#include "botpch.h"
#include "../../playerbot.h"
#include "InvalidTargetValue.h"
#include "AttackersValue.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"

using namespace ai;

bool InvalidTargetValue::Calculate()
{
    Unit* target = AI_VALUE(Unit*, qualifier);
    Unit* enemy = AI_VALUE(Unit*, "enemy player target");
    bool validTarget = AttackersValue::IsValidTarget(target, bot);
    if (target && enemy && target == enemy && validTarget)
        return false;

    if (target && qualifier == "current target")
    {
        return !validTarget;
    }

    return !validTarget;
}
