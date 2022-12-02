#include "botpch.h"
#include "../../playerbot.h"
#include "HunterMultipliers.h"
#include "HunterBuffStrategies.h"

using namespace ai;

void HunterBuffDpsStrategy::InitNonCombatTriggers(std::list<TriggerNode*> &triggers)
{
	triggers.push_back(new TriggerNode(
		"aspect of the hawk", 
		NextAction::array(0, new NextAction("aspect of the hawk", 10.0f), NULL)));
}

void HunterBuffDpsStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitNonCombatTriggers(triggers);
}

void HunterNatureResistanceStrategy::InitNonCombatTriggers(std::list<TriggerNode*> &triggers)
{
	triggers.push_back(new TriggerNode(
		"aspect of the wild", 
		NextAction::array(0, new NextAction("aspect of the wild", 10.0f), NULL)));
}

void HunterNatureResistanceStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitNonCombatTriggers(triggers);
}

void HunterBuffSpeedStrategy::InitNonCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "aspect of the pack",
        NextAction::array(0, new NextAction("aspect of the pack", 10.0f), NULL)));
}

void HunterBuffSpeedStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitNonCombatTriggers(triggers);
}

void HunterBuffManaStrategy::InitNonCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "aspect of the viper",
        NextAction::array(0, new NextAction("aspect of the viper", 10.0f), NULL)));
}

void HunterBuffManaStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitNonCombatTriggers(triggers);
}
