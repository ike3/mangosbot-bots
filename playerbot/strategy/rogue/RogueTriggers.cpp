#include "botpch.h"
#include "../../playerbot.h"
#include "RogueTriggers.h"
#include "RogueActions.h"

using namespace ai;

bool SliceAndDiceTrigger::IsActive()
{
	Unit* target = GetTarget();
	bool hasComboPoints = AI_VALUE2(uint8, "combo", "current target") >= 3;// amount;
	return hasComboPoints && SpellTrigger::IsActive() && !ai->HasAura(spell, target);
}

bool RuptureTrigger::IsActive()
{
	Unit* target = GetTarget();
	bool hasComboPoints = AI_VALUE2(uint8, "combo", "current target") >= 4;// amount;
	return hasComboPoints && DebuffTrigger::IsActive();// && !ai->HasAura(spell, target);
}

bool EviscerateTrigger::IsActive()
{
	Unit* target = GetTarget();
	bool hasComboPoints = AI_VALUE2(uint8, "combo", "current target") >= 4;// amount;
	return hasComboPoints && SpellTrigger::IsActive();// && !ai->HasAura(spell, target);
}

bool RiposteCastTrigger::IsActive()
{
	Unit* target = GetTarget();
	if (!target)
		return false;

	bool isMelee = true;
	if (target->IsPlayer())
	{
		isMelee = !ai->IsRanged((Player*)target);
	}

	return SpellCanBeCastTrigger::IsActive() && isMelee;
}