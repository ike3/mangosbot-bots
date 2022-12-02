#include "botpch.h"
#include "../../playerbot.h"
#include "UsePotionsStrategy.h"

using namespace ai;

class UsePotionsStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    UsePotionsStrategyActionNodeFactory()
    {
        creators["healthstone"] = &healthstone;
        creators["healing potion"] = &healing_potion;
    }
private:
    static ActionNode* healthstone(PlayerbotAI* ai)
    {
        return new ActionNode("healthstone",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("healing potion"), NULL),
            /*C*/ NULL);
    }
    static ActionNode* healing_potion(PlayerbotAI* ai)
    {
        return new ActionNode("healing potion",
            /*P*/ NULL,
            /*A*/ NextAction::array(0, new NextAction("use bandage"), NULL),
            /*C*/ NULL);
    }
};

UsePotionsStrategy::UsePotionsStrategy(PlayerbotAI* ai) : Strategy(ai)
{
    actionNodeFactories.Add(new UsePotionsStrategyActionNodeFactory());
}

void UsePotionsStrategy::InitCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "critical health",
        NextAction::array(0, new NextAction("healthstone", ACTION_HIGH + 2), NULL)));

    triggers.push_back(new TriggerNode(
        "low health",
        NextAction::array(0, new NextAction("use bandage", ACTION_HIGH + 3), NULL)));

    triggers.push_back(new TriggerNode(
        "low mana",
        NextAction::array(0,  new NextAction("mana potion", ACTION_HIGH + 1), NULL)));

    triggers.push_back(new TriggerNode(
        "low mana",
        NextAction::array(0, new NextAction("dark rune", ACTION_HIGH), NULL)));
}

void UsePotionsStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    InitCombatTriggers(triggers);
}
