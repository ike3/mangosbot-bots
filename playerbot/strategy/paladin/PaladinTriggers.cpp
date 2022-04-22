#include "botpch.h"
#include "../../playerbot.h"
#include "PaladinTriggers.h"
#include "PaladinActions.h"

using namespace ai;

bool SealTrigger::IsActive()
{
	Unit* target = GetTarget();
	return !ai->HasAura("seal of justice", target) &&
        !ai->HasAura("seal of command", target) &&
        !ai->HasAura("seal of vengeance", target) &&
		!ai->HasAura("seal of righteousness", target) &&
		!ai->HasAura("seal of light", target) &&
        !ai->HasAura("seal of wisdom", target) &&
        AI_VALUE2(bool, "combat", "self target");
}

bool CrusaderAuraTrigger::IsActive()
{
	Unit* target = GetTarget();
	return AI_VALUE2(bool, "mounted", "self target") && !ai->HasAura("crusader aura", target);
}

bool BlessingTrigger::IsActive()
{
    Unit* target = GetTarget();

    return SpellTrigger::IsActive() &&
        !((ai->HasMyAura("blessing of might", target) || ai->HasMyAura("greater blessing of might", target)) ||
            (ai->HasMyAura("blessing of wisdom", target) || ai->HasMyAura("greater blessing of wisdom", target)) ||
            (ai->HasMyAura("blessing of kings", target) || ai->HasMyAura("greater blessing of kings", target)) ||
            (ai->HasMyAura("blessing of sanctuary", target) || ai->HasMyAura("greater blessing of sanctuary", target)) ||
            (ai->HasMyAura("blessing of salvation", target) || ai->HasMyAura("greater blessing of salvation", target)) ||
            (ai->HasMyAura("blessing of light", target) || ai->HasMyAura("greater blessing of light", target))
            );
}

bool BlessingOnPartyTrigger::IsActive()
{
    Value<Unit*>* smallBless = context->GetValue<Unit*>("party member without my aura",
        "blessing of kings,blessing of might,blessing of wisdom,blessing of sanctuary,blessing of salvation,blessing of light");

    Value<Unit*>* bigBless = context->GetValue<Unit*>("party member without my aura",
        "greater blessing of kings,greater blessing of might,greater blessing of wisdom,greater blessing of sanctuary,greater blessing of salvation,greater blessing of light");

    if (!(smallBless || bigBless))
        return false;

    if (smallBless && !bigBless)
        return true;

    return true;
}
