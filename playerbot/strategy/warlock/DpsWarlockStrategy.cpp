#include "botpch.h"
#include "../../playerbot.h"
#include "WarlockTriggers.h"
#include "WarlockMultipliers.h"
#include "DpsWarlockStrategy.h"
#include "WarlockActions.h"

using namespace ai;

class DpsWarlockStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    DpsWarlockStrategyActionNodeFactory()
    {
        creators["shadow bolt"] = &shadow_bolt;
    }
private:
    static ActionNode* shadow_bolt(PlayerbotAI* ai)
    {
        return new ActionNode ("shadow bolt",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("shoot"), NULL),
            /*C*/ NULL);
    }
};

DpsWarlockStrategy::DpsWarlockStrategy(PlayerbotAI* ai) : GenericWarlockStrategy(ai)
{
    actionNodeFactories.Add(new DpsWarlockStrategyActionNodeFactory());
}

NextAction** DpsWarlockStrategy::GetDefaultCombatActions()
{
    return NextAction::array(0, new NextAction("incinerate", 10.0f), new NextAction("shadow bolt", 10.0f), NULL);
}

void DpsWarlockStrategy::InitCombatTriggers(std::list<TriggerNode*> &triggers)
{
    GenericWarlockStrategy::InitCombatTriggers(triggers);

	triggers.push_back(new TriggerNode(
		"backlash",
		NextAction::array(0, new NextAction("shadow bolt", 20.0f), NULL)));
}

void DpsAoeWarlockStrategy::InitCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "medium aoe",
        NextAction::array(0, new NextAction("rain of fire", 37.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "corruption on attacker",
        NextAction::array(0, new NextAction("corruption on attacker", 27.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "curse of agony on attacker",
        NextAction::array(0, new NextAction("curse of agony on attacker", 28.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "siphon life on attacker",
        NextAction::array(0, new NextAction("siphon life on attacker", 29.0f), NULL)));
}

void DpsWarlockDebuffStrategy::InitCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "corruption",
        NextAction::array(0, new NextAction("seed of corruption", 21.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "corruption",
        NextAction::array(0, new NextAction("corruption", 21.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "curse of agony",
        NextAction::array(0, new NextAction("curse of agony", 22.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "siphon life",
        NextAction::array(0, new NextAction("siphon life", 23.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "unstable affliction",
        NextAction::array(0, new NextAction("unstable affliction", 25.0f), NULL)));
}
