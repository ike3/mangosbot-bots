#include "botpch.h"
#include "../../playerbot.h"
#include "WarriorMultipliers.h"
#include "FuryWarriorStrategy.h"

using namespace ai;

class FuryWarriorStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    FuryWarriorStrategyActionNodeFactory()
    {
        creators["intercept"] = &intercept;
        creators["death wish"] = &death_wish;
        creators["piercing howl"] = &piercing_howl;
        creators["whirlwind"] = &whirlwind;
        creators["bloodthirst"] = &bloodthirst;
        creators["pummel"] = &pummel;
    }
private:
    ACTION_NODE_A(intercept, "intercept", "reach melee");
    ACTION_NODE_A(piercing_howl, "piercing howl", "hamstring");
    ACTION_NODE_A(whirlwind, "whirlwind", "cleave");
    ACTION_NODE_A(death_wish, "death_wish", "berserker rage");
    ACTION_NODE_A(bloodthirst, "bloodthirst", "melee");
    ACTION_NODE_A(pummel, "pummel", "intercept");
};

FuryWarriorStrategy::FuryWarriorStrategy(PlayerbotAI* ai) : GenericWarriorStrategy(ai)
{
    actionNodeFactories.Add(new FuryWarriorStrategyActionNodeFactory());
}

NextAction** FuryWarriorStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("melee", ACTION_NORMAL), NULL);
}

void FuryWarriorStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    GenericWarriorStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "enemy out of melee",
        NextAction::array(0, new NextAction("intercept", ACTION_MOVE + 9), NULL)));

    triggers.push_back(new TriggerNode(
        "berserker stance",
        NextAction::array(0, new NextAction("berserker stance", ACTION_HIGH + 9), NULL)));

    triggers.push_back(new TriggerNode(
        "bloodthirst",
        NextAction::array(0, new NextAction("bloodthirst", ACTION_HIGH + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "target critical health",
        NextAction::array(0, new NextAction("execute", ACTION_HIGH + 4), NULL)));

	triggers.push_back(new TriggerNode(
		"hamstring",
		NextAction::array(0, new NextAction("piercing howl", ACTION_HIGH + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "pummel on enemy healer",
        NextAction::array(0, new NextAction("pummel on enemy healer", ACTION_INTERRUPT), NULL)));

    triggers.push_back(new TriggerNode(
        "pummel",
        NextAction::array(0, new NextAction("pummel", ACTION_INTERRUPT), NULL)));

    triggers.push_back(new TriggerNode(
        "intercept on snare target",
        NextAction::array(0, new NextAction("intercept on snare target", ACTION_HIGH), NULL)));

    triggers.push_back(new TriggerNode(
        "bloodthirst",
        NextAction::array(0, new NextAction("bloodthirst", ACTION_HIGH + 2), NULL)));

    triggers.push_back(new TriggerNode(
        "medium rage available",
        NextAction::array(0, new NextAction("heroic strike", ACTION_HIGH), NULL)));

    triggers.push_back(new TriggerNode(
        "berserker rage",
        NextAction::array(0, new NextAction("berserker rage", ACTION_HIGH + 2), NULL)));

    triggers.push_back(new TriggerNode(
        "bloodrage",
        NextAction::array(0, new NextAction("bloodrage", ACTION_HIGH + 2), NULL)));

    triggers.push_back(new TriggerNode(
        "boost",
        NextAction::array(0, new NextAction("death wish", ACTION_HIGH + 2), NULL)));

    triggers.push_back(new TriggerNode(
        "medium aoe",
        NextAction::array(0, new NextAction("demoralizing shout", ACTION_HIGH + 2), NULL)));
}


void FuryWarrirorAoeStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "light aoe",
        NextAction::array(0, new NextAction("whirlwind", ACTION_HIGH + 2), NULL)));
}
