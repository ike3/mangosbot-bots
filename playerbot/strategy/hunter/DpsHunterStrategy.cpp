#include "botpch.h"
#include "../../playerbot.h"

#include "HunterMultipliers.h"
#include "DpsHunterStrategy.h"

using namespace ai;

class DpsHunterStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    DpsHunterStrategyActionNodeFactory()
    {
        creators["aimed shot"] = &aimed_shot;
        creators["chimera shot"] = &chimera_shot;
        creators["steady shot"] = &steady_shot;
        creators["explosive shot"] = &explosive_shot;
        creators["black arrow"] = &black_arrow;
        creators["tame beast"] = &tame_beast;
    }
private:
    ACTION_NODE_A(black_arrow, "black arrow", "concussive shot");

    static ActionNode* aimed_shot(PlayerbotAI* ai)
    {
        return new ActionNode("aimed shot",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("chimera shot", 10.0f), NULL),
            /*C*/ NULL);
    }

    static ActionNode* chimera_shot(PlayerbotAI* ai)
    {
        return new ActionNode("chimera shot",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("steady shot", 10.0f), NULL),
            /*C*/ NULL);
    }

    ACTION_NODE_A(steady_shot, "steady shot", "arcane shot");

    static ActionNode* explosive_shot(PlayerbotAI* ai)
    {
        return new ActionNode("explosive shot",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("aimed shot"), NULL),
            /*C*/ NULL);
    }

    static ActionNode* tame_beast(PlayerbotAI* ai)
    {
        return new ActionNode("tame beast",
            /*P*/ NULL,
            /*A*/ NULL,
            /*C*/ NULL);
    }
};

DpsHunterStrategy::DpsHunterStrategy(PlayerbotAI* ai) : GenericHunterStrategy(ai)
{
    actionNodeFactories.Add(new DpsHunterStrategyActionNodeFactory());
}

NextAction** DpsHunterStrategy::GetDefaultCombatActions()
{
    return NextAction::array(0, new NextAction("explosive shot", 11.0f), new NextAction("auto shot", 10.0f), new NextAction("melee", 9.0f), NULL);
}

void DpsHunterStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    GenericHunterStrategy::InitCombatTriggers(triggers);

#ifdef MANGOSBOT_TWO
    triggers.push_back(new TriggerNode(
        "black arrow",
        NextAction::array(0, new NextAction("black arrow", 15.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "black arrow",
        NextAction::array(0, new NextAction("black arrow", 15.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "black arrow on snare target",
        NextAction::array(0, new NextAction("black arrow", 59.0f), NULL)));
#endif

    triggers.push_back(new TriggerNode(
        "aimed shot",
        NextAction::array(0, new NextAction("aimed shot", 19.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy five yards",
        NextAction::array(0, new NextAction("aspect of the monkey", ACTION_HIGH), NULL)));

    triggers.push_back(new TriggerNode(
        "wyvern sting",
        NextAction::array(0, new NextAction("wyvern sting", 26.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "counterattack",
        NextAction::array(0, new NextAction("counterattack", 36.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "silencing shot interrupt",
        NextAction::array(0, new NextAction("silencing shot", ACTION_INTERRUPT), NULL)));

    triggers.push_back(new TriggerNode(
        "silencing shot on enemy healer",
        NextAction::array(0, new NextAction("silencing shot on enemy healer", ACTION_INTERRUPT), NULL)));

    triggers.push_back(new TriggerNode(
        "no beast",
        NextAction::array(0, new NextAction("tame beast", 1000.0f), NULL)));
}

void DpsAoeHunterStrategy::InitCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "ranged light aoe",
        NextAction::array(0, new NextAction("multi shot", ACTION_HIGH), NULL)));

    triggers.push_back(new TriggerNode(
        "multi shot",
        NextAction::array(0, new NextAction("multi shot", ACTION_HIGH), NULL)));

	triggers.push_back(new TriggerNode(
		"ranged medium aoe",
		NextAction::array(0, new NextAction("volley", 10.0f), NULL)));

	triggers.push_back(new TriggerNode(
		"serpent sting on attacker",
		NextAction::array(0, new NextAction("serpent sting on attacker", 17.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "viper sting on attacker",
        NextAction::array(0, new NextAction("viper sting on attacker", 18.0f), NULL)));
}

void DpsHunterDebuffStrategy::InitCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "hunter's mark",
        NextAction::array(0, new NextAction("hunter's mark", 13.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "no stings",
        NextAction::array(0, new NextAction("serpent sting", 23.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "no stings",
        NextAction::array(0, new NextAction("viper sting", 24.0f), NULL)));
}
