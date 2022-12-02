#include "botpch.h"
#include "../../playerbot.h"
#include "HunterMultipliers.h"
#include "GenericHunterNonCombatStrategy.h"

using namespace ai;

class GenericHunterNonCombatStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericHunterNonCombatStrategyActionNodeFactory()
    {
        creators["rapid fire"] = &rapid_fire;
        creators["boost"] = &rapid_fire;
        creators["aspect of the pack"] = &aspect_of_the_pack;
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
    ACTION_NODE_A(freezing_trap, "freezing trap", "remove feign death");
};

GenericHunterNonCombatStrategy::GenericHunterNonCombatStrategy(PlayerbotAI* ai) : NonCombatStrategy(ai)
{
    actionNodeFactories.Add(new GenericHunterNonCombatStrategyActionNodeFactory());
}

void GenericHunterNonCombatStrategy::InitNonCombatTriggers(std::list<TriggerNode*> &triggers)
{
    NonCombatStrategy::InitNonCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "trueshot aura",
        NextAction::array(0, new NextAction("trueshot aura", 2.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("apply oil", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "low ammo",
        NextAction::array(0, new NextAction("say::low ammo", ACTION_NORMAL), NULL)));

    /*triggers.push_back(new TriggerNode(
        "no ammo",
        NextAction::array(0, new NextAction("switch to melee", ACTION_NORMAL + 1), new NextAction("say::no ammo", ACTION_NORMAL), NULL)));

    triggers.push_back(new TriggerNode(
        "has ammo",
        NextAction::array(0, new NextAction("switch to ranged", ACTION_NORMAL), NULL)));*/

    triggers.push_back(new TriggerNode(
        "feign death",
        NextAction::array(0, new NextAction("remove feign death", 91.f), new NextAction("freezing trap", 90.f), NULL)));

    triggers.push_back(new TriggerNode(
        "player has flag",
        NextAction::array(0, new NextAction("aspect of the cheetah", ACTION_EMERGENCY + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "has attackers",
        NextAction::array(0, new NextAction("remove aspect of the cheetah", ACTION_EMERGENCY + 1), NULL)));
}

void HunterPetStrategy::InitNonCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "no pet",
        NextAction::array(0, new NextAction("call pet", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "pet not happy",
        NextAction::array(0, new NextAction("feed pet", 2.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "hunters pet low health",
        NextAction::array(0, new NextAction("mend pet", 2.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "hunters pet dead",
        NextAction::array(0, new NextAction("revive pet", 1.0f), NULL)));
}
