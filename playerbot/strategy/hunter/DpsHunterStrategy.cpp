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
        creators["viper sting"] = &viper_sting;
        creators["black arrow"] = &black_arrow;
    }
private:
    ACTION_NODE_A(black_arrow, "black arrow", "concussive shot");
    static ActionNode* viper_sting(PlayerbotAI* ai)
    {
        return new ActionNode ("viper sting",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("mana potion", 10.0f), NULL),
            /*C*/ NULL);
    }
    static ActionNode* aimed_shot(PlayerbotAI* ai)
    {
        return new ActionNode ("aimed shot",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("chimera shot", 10.0f), NULL),
            /*C*/ NULL);
    }
    static ActionNode* chimera_shot(PlayerbotAI* ai)
    {
        return new ActionNode ("chimera shot",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("steady shot", 10.0f), NULL),
            /*C*/ NULL);
    }
    ACTION_NODE_A(steady_shot, "steady shot", "arcane shot");
    static ActionNode* explosive_shot(PlayerbotAI* ai)
    {
        return new ActionNode ("explosive shot",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("aimed shot"), NULL),
            /*C*/ NULL);
    }
};

DpsHunterStrategy::DpsHunterStrategy(PlayerbotAI* ai) : GenericHunterStrategy(ai)
{
    actionNodeFactories.Add(new DpsHunterStrategyActionNodeFactory());
}

NextAction** DpsHunterStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("explosive shot", 11.0f), new NextAction("auto shot", 10.0f), new NextAction("auto attack", 9.0f), NULL);
}

void DpsHunterStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    GenericHunterStrategy::InitTriggers(triggers);

#ifdef MANGOSBOT_TWO
    triggers.push_back(new TriggerNode(
        "black arrow",
        NextAction::array(0, new NextAction("black arrow", 15.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "black arrow",
        NextAction::array(0, new NextAction("black arrow", 15.0f), NULL)));
#endif

    /*triggers.push_back(new TriggerNode(
        "very often",
        NextAction::array(0, new NextAction("auto shot", 65.0f), NULL)));*/

    triggers.push_back(new TriggerNode(
        "black arrow on snare target",
        NextAction::array(0, new NextAction("black arrow", 61.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "low mana",
        NextAction::array(0, new NextAction("viper sting", 23.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "aimed shot",
        NextAction::array(0, new NextAction("aimed shot", 13.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "hunter's mark",
        NextAction::array(0, new NextAction("hunter's mark", 19.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy ten yards",
        NextAction::array(0, new NextAction("deterrence", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "concussive shot on snare target",
        NextAction::array(0, new NextAction("concussive shot", 62.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "scatter shot on snare target",
        NextAction::array(0, new NextAction("scatter shot", 61.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "counterattack",
        NextAction::array(0, new NextAction("counterattack", 36.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "wyvern sting",
        NextAction::array(0, new NextAction("wyvern sting", 26.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy five yards",
        NextAction::array(0, new NextAction("aspect of the monkey", ACTION_HIGH), NULL)));

    triggers.push_back(new TriggerNode(
        "intimidation on snare target",
        NextAction::array(0, new NextAction("intimidation", 25.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "silencing shot interrupt",
        NextAction::array(0, new NextAction("silencing shot", ACTION_INTERRUPT), NULL)));

    triggers.push_back(new TriggerNode(
        "silencing shot on enemy healer",
        NextAction::array(0, new NextAction("silencing shot on enemy healer", ACTION_INTERRUPT), NULL)));

    /*triggers.push_back(new TriggerNode(
        "has aggro",
        NextAction::array(0, new NextAction("concussive shot", 20.0f), NULL)));*/
}

void DpsAoeHunterStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "light aoe",
        NextAction::array(0, new NextAction("multi shot", ACTION_HIGH), NULL)));

    triggers.push_back(new TriggerNode(
        "multi shot",
        NextAction::array(0, new NextAction("multi shot", ACTION_HIGH), NULL)));

	triggers.push_back(new TriggerNode(
		"medium aoe",
		NextAction::array(0, new NextAction("volley", 10.0f), NULL)));

	triggers.push_back(new TriggerNode(
		"serpent sting on attacker",
		NextAction::array(0, new NextAction("serpent sting on attacker", 17.0f), NULL)));
}

void DpsHunterDebuffStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "no stings",
        NextAction::array(0, new NextAction("serpent sting", 18.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "viper sting",
        NextAction::array(0, new NextAction("viper sting", 24.0f), NULL)));
}
