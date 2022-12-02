#include "botpch.h"
#include "../../playerbot.h"
#include "PaladinMultipliers.h"
#include "PaladinBuffStrategies.h"

using namespace ai;

void PaladinBuffWisdomStrategy::InitNonCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "blessing on party",
        NextAction::array(0, new NextAction("blessing of wisdom on party", 11.0f), NULL)));
}

void PaladinBuffWisdomStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitNonCombatTriggers(triggers);
}

void PaladinBuffSpeedStrategy::InitNonCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "crusader aura",
        NextAction::array(0, new NextAction("crusader aura", 40.0f), NULL)));
}

void PaladinBuffSpeedStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitNonCombatTriggers(triggers);
}

void PaladinBuffMightStrategy::InitNonCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "blessing on party",
        NextAction::array(0, new NextAction("blessing of might on party", 11.0f), NULL)));
}

void PaladinBuffMightStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitNonCombatTriggers(triggers);
}

void PaladinShadowResistanceStrategy::InitNonCombatTriggers(std::list<TriggerNode*> &triggers)
{
	triggers.push_back(new TriggerNode(
		"paladin aura",
		NextAction::array(0, new NextAction("shadow resistance aura", 12.0f), NULL)));
}

void PaladinShadowResistanceStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitNonCombatTriggers(triggers);
}

void PaladinFrostResistanceStrategy::InitNonCombatTriggers(std::list<TriggerNode*> &triggers)
{
	triggers.push_back(new TriggerNode(
		"paladin aura",
		NextAction::array(0, new NextAction("frost resistance aura", 12.0f), NULL)));
}

void PaladinFrostResistanceStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitNonCombatTriggers(triggers);
}

void PaladinFireResistanceStrategy::InitNonCombatTriggers(std::list<TriggerNode*> &triggers)
{
	triggers.push_back(new TriggerNode(
		"paladin aura",
		NextAction::array(0, new NextAction("fire resistance aura", 12.0f), NULL)));
}

void PaladinFireResistanceStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitNonCombatTriggers(triggers);
}

void PaladinBuffArmorStrategy::InitNonCombatTriggers(std::list<TriggerNode*> &triggers)
{
	triggers.push_back(new TriggerNode(
		"paladin aura",
		NextAction::array(0, new NextAction("devotion aura", 12.0f), NULL)));
}

void PaladinBuffArmorStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitNonCombatTriggers(triggers);
}

void PaladinBuffConcentrationStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "paladin aura",
        NextAction::array(0, new NextAction("concentration aura", 12.0f), NULL)));
}

void PaladinBuffConcentrationStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitNonCombatTriggers(triggers);
}

void PaladinBuffSanctityStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "paladin aura",
        NextAction::array(0, new NextAction("sanctity aura", 12.0f), NULL)));
}

void PaladinBuffSanctityStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitNonCombatTriggers(triggers);
}

void PaladinBuffAoeStrategy::InitNonCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "paladin aura",
        NextAction::array(0, new NextAction("retribution aura", 12.0f), NULL)));
}

void PaladinBuffAoeStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitNonCombatTriggers(triggers);
}

void PaladinBuffThreatStrategy::InitNonCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "righteous fury",
        NextAction::array(0, new NextAction("righteous fury", ACTION_HIGH + 8), NULL)));
}

void PaladinBuffThreatStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitNonCombatTriggers(triggers);
}

void PaladinBuffKingsStrategy::InitNonCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "blessing on party",
        NextAction::array(0, new NextAction("blessing of kings on party", 11.0f), NULL)));
}

void PaladinBuffKingsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitNonCombatTriggers(triggers);
}