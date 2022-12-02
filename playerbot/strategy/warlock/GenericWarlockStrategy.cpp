#include "botpch.h"
#include "../../playerbot.h"
#include "WarlockMultipliers.h"
#include "GenericWarlockStrategy.h"

using namespace ai;

class GenericWarlockStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericWarlockStrategyActionNodeFactory()
    {
        //creators["summon voidwalker"] = &summon_voidwalker;
        creators["banish"] = &banish;
    }
private:
    //static ActionNode* summon_voidwalker(PlayerbotAI* ai)
    //{
    //    return new ActionNode ("summon voidwalker",
    //        /*P*/ NULL,
    //        /*A*/ NextAction::array(0, new NextAction("drain soul"), NULL),
    //        /*C*/ NULL);
    //}
    static ActionNode* banish(PlayerbotAI* ai)
    {
        return new ActionNode ("banish",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("fear"), NULL),
            /*C*/ NULL);
    }
};

GenericWarlockStrategy::GenericWarlockStrategy(PlayerbotAI* ai) : CombatStrategy(ai)
{
    actionNodeFactories.Add(new GenericWarlockStrategyActionNodeFactory());
}

NextAction** GenericWarlockStrategy::GetDefaultCombatActions()
{
    return NextAction::array(0, new NextAction("shoot", 10.0f), NULL);
}

void GenericWarlockStrategy::InitCombatTriggers(std::list<TriggerNode*> &triggers)
{
    CombatStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "enemy out of spell",
        NextAction::array(0, new NextAction("reach spell", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "shadow trance",
        NextAction::array(0, new NextAction("shadow bolt", 20.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "low health",
        NextAction::array(0, new NextAction("drain life", 40.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "medium mana",
        NextAction::array(0, new NextAction("life tap", ACTION_HIGH + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "low mana",
        NextAction::array(0, new NextAction("dark pact", ACTION_HIGH + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "death coil interrupt",
        NextAction::array(0, new NextAction("death coil", ACTION_INTERRUPT + 3), NULL)));

    triggers.push_back(new TriggerNode(
        "death coil on enemy healer",
        NextAction::array(0, new NextAction("death coil on enemy healer", ACTION_INTERRUPT + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "death coil on snare target",
        NextAction::array(0, new NextAction("death coil on snare target", 61.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "shadowfury interrupt",
        NextAction::array(0, new NextAction("shadowfury", ACTION_INTERRUPT + 4), NULL)));

    triggers.push_back(new TriggerNode(
        "shadowfury on snare target",
        NextAction::array(0, new NextAction("shadowfury on snare target", 62.0f), NULL)));

	triggers.push_back(new TriggerNode(
		"target critical health",
		NextAction::array(0, new NextAction("drain soul", 30.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "immolate",
        NextAction::array(0, new NextAction("immolate", 13.0f), new NextAction("conflagrate", 12.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "amplify curse",
        NextAction::array(0, new NextAction("amplify curse", 41.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "target critical health",
        NextAction::array(0, new NextAction("shadowburn", 41.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy ten yards",
        NextAction::array(0, new NextAction("howl of terror", 61.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "inferno",
        NextAction::array(0, new NextAction("inferno", 50.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "medium threat",
        NextAction::array(0, new NextAction("soul shatter", 55.0f), NULL)));
}

void WarlockBoostStrategy::InitCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "amplify curse",
        NextAction::array(0, new NextAction("amplify curse", 41.0f), NULL)));
}

void WarlockCcStrategy::InitCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "banish",
        NextAction::array(0, new NextAction("banish on cc", 52.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "fear",
        NextAction::array(0, new NextAction("fear on cc", 53.0f), NULL)));
}
