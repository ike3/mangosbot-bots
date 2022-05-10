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

bool ConcentrationAuraTrigger::IsActive()
{
    uint32 concAura = AI_VALUE2(uint32, "spell id", "concentration aura");
    uint32 devoAura = AI_VALUE2(uint32, "spell id", "devotion aura");
    if (!(concAura || devoAura))
        return false;

    if (bot->HasSpell(concAura))
        return !ai->HasAura("concentration aura", bot);

    if (bot->HasSpell(devoAura))
        return !ai->HasAura("devotion aura", bot);

    return false;
}

bool SanctityAuraTrigger::IsActive()
{
    uint32 sancAura = AI_VALUE2(uint32, "spell id", "sanctity aura");
    uint32 retrAura = AI_VALUE2(uint32, "spell id", "retribution aura");
    uint32 devoAura = AI_VALUE2(uint32, "spell id", "devotion aura");
    if (!(sancAura || devoAura || retrAura))
        return false;

    if (bot->HasSpell(sancAura))
        return !ai->HasAura("sanctity aura", bot);

    if (bot->HasSpell(retrAura))
        return !ai->HasAura("retribution aura", bot);

    if (bot->HasSpell(devoAura))
        return !ai->HasAura("devotion aura", bot);

    return false;
}

bool RetributionAuraTrigger::IsActive()
{
    uint32 retrAura = AI_VALUE2(uint32, "spell id", "retribution aura");
    uint32 devoAura = AI_VALUE2(uint32, "spell id", "devotion aura");
    if (!(devoAura || retrAura))
        return false;

    if (bot->HasSpell(retrAura))
        return !ai->HasAura("retribution aura", bot);

    if (bot->HasSpell(devoAura))
        return !ai->HasAura("devotion aura", bot);

    return false;
}

bool PaladinAuraTrigger::IsActive()
{
    // already has aura
    if (ai->HasMyAura("Devotion Aura", bot))
        return false;

    if (ai->HasMyAura("Retribution Aura", bot))
        return false;

    if (ai->HasMyAura("Concentration Aura", bot))
        return false;

    if (ai->HasMyAura("Sanctity Aura", bot))
        return false;

    if (ai->HasMyAura("Fire Resistance Aura", bot))
        return false;

    if (ai->HasMyAura("Frost Resistance Aura", bot))
        return false;

    if (ai->HasMyAura("Shadow Resistance Aura", bot))
        return false;

    //BotRoles roles = AI_VALUE()
}
