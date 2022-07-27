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
        !(ai->HasMyAura("blessing of might", target) ||
            ai->HasMyAura("blessing of wisdom", target) ||
            ai->HasMyAura("blessing of kings", target) ||
            ai->HasMyAura("blessing of sanctuary", target) ||
            ai->HasMyAura("blessing of salvation", target) ||
            ai->HasMyAura("blessing of light", target)
            );
}

bool BlessingOnPartyTrigger::IsActive()
{
    vector<std::string> altBlessings;
    vector<std::string> haveBlessings;
    altBlessings.push_back("blessing of might");
    altBlessings.push_back("blessing of wisdom");
    altBlessings.push_back("blessing of kings");
    altBlessings.push_back("blessing of salvation");
    altBlessings.push_back("blessing of light");

    for (auto blessing : altBlessings)
    {
        if (AI_VALUE2(uint32, "spell id", blessing))
        {
            haveBlessings.push_back(blessing);
            haveBlessings.push_back("greater " + blessing);
        }
    }
    if (haveBlessings.empty())
        return false;

    std::string blessList = "";
    for (auto blessing : haveBlessings)
    {
        blessList += blessing;
        if (blessing != haveBlessings[haveBlessings.size() - 1])
            blessList += ",";
    }

    return AI_VALUE2(Unit*, "party member without my aura", blessList);
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
    vector<std::string> altAuras;
    vector<std::string> haveAuras;
    altAuras.push_back("devotion aura");
    altAuras.push_back("retribution aura");
    altAuras.push_back("concentration aura");
    altAuras.push_back("sanctity aura");
    altAuras.push_back("shadow resistance aura");
    altAuras.push_back("fire resistance aura");
    altAuras.push_back("frost resistance aura");
    altAuras.push_back("crusader aura");

    for (auto aura : altAuras)
    {
        if (AI_VALUE2(uint32, "spell id", aura))
            haveAuras.push_back(aura);
    }
    if (haveAuras.empty())
        return false;

    bool hasAura = false;
    for (auto aura : haveAuras)
    {
        if (ai->HasMyAura(aura, bot))
        {
            hasAura = true;
            break;
        }
    }
    if (hasAura)
        return false;

    bool needAura = false;
    for (auto aura : haveAuras)
    {
        if (!ai->HasAura(aura, bot))
        {
            needAura = true;
            break;
        }
    }
    return needAura;
}

bool HammerOfJusticeOnEnemyTrigger::IsActive()
{
    Unit* target = GetTarget();
    if (!target)
        return false;

    uint8 targetHP = AI_VALUE2(uint8, "health", GetTargetName());
    uint8 selfHP = AI_VALUE2(uint8, "health", "self target");
    uint8 selfMP = AI_VALUE2(uint8, "mana", "self target");
    bool isMoving = AI_VALUE2(bool, "moving", GetTargetName());

    if (isMoving && target->IsPlayer())
        return true;

    if (targetHP < 10)
        return true;

    if (selfHP < sPlayerbotAIConfig.lowHealth && selfMP > 10)
        return true;

    return false;
}
