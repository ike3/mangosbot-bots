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
        creators["scatter shot"] = &scatter_shot;
        creators["concussive shot"] = &concussive_shot;
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

    static ActionNode* concussive_shot(PlayerbotAI* ai)
    {
        return new ActionNode("concussive shot",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("intimidation"), NULL),
            /*C*/ NextAction::array(0, new NextAction("flee"), NULL));
    }
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

    /*
    triggers.push_back(new TriggerNode(
        "switch to ranged",
        NextAction::array(0, new NextAction("switch to ranged", ACTION_HIGH), NULL)));

    triggers.push_back(new TriggerNode(
        "switch to melee",
        NextAction::array(0, new NextAction("switch to melee", ACTION_HIGH + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "no ammo",
        NextAction::array(0, new NextAction("switch to melee", ACTION_HIGH + 1), new NextAction("say::no ammo", ACTION_HIGH), NULL)));
    */

    triggers.push_back(new TriggerNode(
        "dispel enrage",
        NextAction::array(0, new NextAction("tranquilizing shot", ACTION_INTERRUPT + 4), NULL)));

    // MOVE THIS TO AOE
    // Traps must be on ACTION_INTERRUPT or higher due to its movements and chained actions
    triggers.push_back(new TriggerNode(
        "explosive trap",
        NextAction::array(0, new NextAction("explosive trap on target", ACTION_INTERRUPT + 3), NULL)));

    // MOVE THIS TO PVE
    // Traps must be on ACTION_INTERRUPT or higher due to its movements and chained actions
    triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("immolation trap on target", ACTION_INTERRUPT + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "high threat",
        NextAction::array(0, new NextAction("feign death", ACTION_EMERGENCY + 1), 
                             new NextAction("shadowmeld", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy is close",
        NextAction::array(0, new NextAction("deterrence", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "hunters pet low health",
        NextAction::array(0, new NextAction("mend pet", ACTION_HIGH), NULL)));

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
        "feign death",
        NextAction::array(0, new NextAction("remove feign death", ACTION_HIGH), NULL)));
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
        "freezing trap",
        NextAction::array(0, new NextAction("freezing trap on cc", ACTION_INTERRUPT + 5), NULL)));

    triggers.push_back(new TriggerNode(
        "scare beast",
        NextAction::array(0, new NextAction("scare beast on cc", ACTION_INTERRUPT + 4), NULL)));

    triggers.push_back(new TriggerNode(
        "frost trap",
        NextAction::array(0, new NextAction("frost trap on target", ACTION_INTERRUPT + 2), NULL)));

    // MOVE THIS TO PVP
    triggers.push_back(new TriggerNode(
        "enemy is close",
        NextAction::array(0, new NextAction("freezing trap in place", ACTION_INTERRUPT), NULL)));
}
