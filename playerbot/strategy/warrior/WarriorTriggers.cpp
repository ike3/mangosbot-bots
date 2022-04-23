#include "botpch.h"
#include "../../playerbot.h"
#include "WarriorTriggers.h"
#include "WarriorActions.h"

using namespace ai;

bool BloodrageBuffTrigger::IsActive()
{
    return AI_VALUE2(uint8, "health", "self target") >= sPlayerbotAIConfig.mediumHealth &&
        AI_VALUE2(uint8, "rage", "self target") < 20;
}

bool SunderArmorDebuffTrigger::IsActive()
{
    Unit* target = GetTarget();
    
    return target && !ai->HasAura("sunder armor", target, true);
}