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
        creators["scatter shot"] = &scatter_shot;
        creators["concussive shot"] = &concussive_shot;
        creators["feign death"] = &feign_death;
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
    static ActionNode* scatter_shot(PlayerbotAI* ai)
    {
        return new ActionNode("scatter shot",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("wing clip"), NULL),
            /*C*/ NextAction::array(0, new NextAction("flee"), NULL));
    }
    static ActionNode* feign_death(PlayerbotAI* ai)
    {
        return new ActionNode("feign death",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("scatter shot"), NULL),
            /*C*/ NULL);
    }
    static ActionNode* concussive_shot(PlayerbotAI* ai)
    {
        return new ActionNode("concussive shot",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("intimidation"), NULL),
            /*C*/ NextAction::array(0, new NextAction("flee"), NULL));
    }
    ACTION_NODE_A(freezing_trap, "freezing trap", "remove feign death");
};

GenericHunterStrategy::GenericHunterStrategy(PlayerbotAI* ai) : CombatStrategy(ai)
{
    actionNodeFactories.Add(new GenericHunterStrategyActionNodeFactory());
}

NextAction** GenericHunterStrategy::GetDefaultCombatActions()
{
    return NextAction::array(0, new NextAction("auto shot", 10.0f), new NextAction("melee", 9.0f), NULL);
}

void GenericHunterStrategy::InitCombatTriggers(std::list<TriggerNode*> &triggers)
{
    CombatStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "hunters pet low health",
        NextAction::array(0, new NextAction("mend pet", ACTION_HIGH), NULL)));

    /*
    triggers.push_back(new TriggerNode(
        "switch to ranged",
        NextAction::array(0, new NextAction("switch to ranged", ACTION_HIGH), NULL)));

    triggers.push_back(new TriggerNode(
        "switch to melee",
        NextAction::array(0, new NextAction("switch to melee", ACTION_HIGH + 1), NULL)));
    */

    /*
    triggers.push_back(new TriggerNode(
        "no ammo",
        NextAction::array(0, new NextAction("switch to melee", ACTION_HIGH + 1), new NextAction("say::no ammo", ACTION_HIGH), NULL)));
    */

    triggers.push_back(new TriggerNode(
        "ranged light aoe",
        NextAction::array(0, new NextAction("frost trap", ACTION_HIGH + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "ranged light aoe",
        NextAction::array(0, new NextAction("explosive trap", ACTION_HIGH + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "high threat",
        NextAction::array(0, new NextAction("feign death", ACTION_INTERRUPT + 1), new NextAction("shadowmeld", ACTION_INTERRUPT), NULL)));

    triggers.push_back(new TriggerNode(
        "multiple attackers",
        NextAction::array(0, new NextAction("feign death", ACTION_INTERRUPT + 1), new NextAction("shadowmeld", ACTION_INTERRUPT), NULL)));    

    triggers.push_back(new TriggerNode(
        "enemy is close",
        NextAction::array(0, new NextAction("deterrence", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "stealthed nearby",
        NextAction::array(0, new NextAction("flare", 65.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy is close",
        NextAction::array(0, new NextAction("raptor strike", 10.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy ten yards",
        NextAction::array(0, new NextAction("concussive shot", 61.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy fifteen yards",
        NextAction::array(0, new NextAction("concussive shot", 61.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy is close",
        NextAction::array(0, new NextAction("feign death", 62.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "feign death",
        NextAction::array(0, new NextAction("remove feign death", 63.0f), new NextAction("freezing trap", 63.0f), NULL)));
}

void HunterBoostStrategy::InitCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "rapid fire",
        NextAction::array(0, new NextAction("rapid fire", 16.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "bestial wrath",
        NextAction::array(0, new NextAction("bestial wrath", 16.0f), NULL)));
}

void HunterCcStrategy::InitCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "scare beast",
        NextAction::array(0, new NextAction("scare beast on cc", ACTION_HIGH + 3), NULL)));

    triggers.push_back(new TriggerNode(
        "freezing trap",
        NextAction::array(0, new NextAction("freezing trap on cc", ACTION_HIGH + 3), NULL)));
}
