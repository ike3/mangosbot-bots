#include "botpch.h"
#include "../../playerbot.h"
#include "GenericHunterStrategy.h"
#include "HunterAiObjectContext.h"

using namespace ai;

class GenericHunterStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericHunterStrategyActionNodeFactory()
    {
        creators["rapid fire"] = &rapid_fire;
        creators["boost"] = &rapid_fire;
        creators["aspect of the pack"] = &aspect_of_the_pack;
        creators["wing clip"] = &wing_clip;
        creators["freezing trap"] = &freezing_trap;
    }
private:
    static ActionNode* rapid_fire(PlayerbotAI* ai)
    {
        return new ActionNode ("rapid fire",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("readiness"), NULL),
            /*C*/ NULL);
    }
    static ActionNode* aspect_of_the_pack(PlayerbotAI* ai)
    {
        return new ActionNode ("aspect of the pack",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("aspect of the cheetah"), NULL),
            /*C*/ NULL);
    }
    static ActionNode* wing_clip(PlayerbotAI* ai)
    {
        return new ActionNode("wing clip",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("raptor strike"), NULL),
            /*C*/ NextAction::array(0, new NextAction("flee"), NULL));
    }
    ACTION_NODE_A(freezing_trap, "freezing trap", "remove feign death");
};

GenericHunterStrategy::GenericHunterStrategy(PlayerbotAI* ai) : CombatStrategy(ai)
{
    actionNodeFactories.Add(new GenericHunterStrategyActionNodeFactory());
}

void GenericHunterStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    CombatStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "enemy five yards",
        NextAction::array(0, new NextAction("wing clip", 61.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy ten yards",
        NextAction::array(0, new NextAction("freezing trap", 60.0f), NULL)));

    /*triggers.push_back(new TriggerNode(
        "enemy is close",
        NextAction::array(0, new NextAction("freezing trap", ACTION_HIGH + 5), NULL)));*/

    triggers.push_back(new TriggerNode(
        "medium threat",
        NextAction::array(0, new NextAction("feign death", ACTION_INTERRUPT + 1), new NextAction("shadowmeld", ACTION_INTERRUPT), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy is close",
        NextAction::array(0, new NextAction("feign death", 63.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "feign death",
        NextAction::array(0, new NextAction("remove feign death", 62.0f), new NextAction("freezing trap", 61.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "hunters pet low health",
        NextAction::array(0, new NextAction("mend pet", ACTION_HIGH), NULL)));

    /*triggers.push_back(new TriggerNode(
        "switch to melee",
        NextAction::array(0, new NextAction("switch to melee", ACTION_HIGH + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "switch to ranged",
        NextAction::array(0, new NextAction("switch to ranged", ACTION_HIGH), NULL)));*/

    /*triggers.push_back(new TriggerNode(
        "no ammo",
        NextAction::array(0, new NextAction("switch to melee", ACTION_HIGH + 1), new NextAction("say::no ammo", ACTION_HIGH), NULL)));*/

    triggers.push_back(new TriggerNode(
        "light aoe",
        NextAction::array(0, new NextAction("frost trap", ACTION_HIGH + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "light aoe",
        NextAction::array(0, new NextAction("explosive trap", ACTION_HIGH + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "rapid fire",
        NextAction::array(0, new NextAction("rapid fire", 16.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "bestial wrath",
        NextAction::array(0, new NextAction("bestial wrath", 16.0f), NULL)));
}

NextAction** HunterBoostStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("bestial wrath", 15.0f), NULL);
}

void HunterBoostStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "rapid fire",
        NextAction::array(0, new NextAction("rapid fire", 16.0f), NULL)));
}

void HunterCcStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "scare beast",
        NextAction::array(0, new NextAction("scare beast on cc", ACTION_HIGH + 3), NULL)));

    triggers.push_back(new TriggerNode(
        "freezing trap",
        NextAction::array(0, new NextAction("freezing trap on cc", ACTION_HIGH + 3), NULL)));
}
