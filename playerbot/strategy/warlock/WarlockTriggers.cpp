#include "botpch.h"
#include "../../playerbot.h"
#include "WarlockTriggers.h"
#include "WarlockActions.h"

using namespace ai;

bool DemonArmorTrigger::IsActive() 
{
	Unit* target = GetTarget();
	return !ai->HasAura("demon skin", target) &&
		!ai->HasAura("demon armor", target) &&
		!ai->HasAura("fel armor", target);
}

bool SpellstoneTrigger::IsActive() 
{
    return BuffTrigger::IsActive() && AI_VALUE2(uint32, "item count", getName()) > 0;
}

bool InfernoTrigger::IsActive()
{
	return AI_VALUE(uint8, "attacker count") > 1 && bot->HasSpell(1122) && bot->HasItemCount(5565, 1) && !urand(0, 2);
}
