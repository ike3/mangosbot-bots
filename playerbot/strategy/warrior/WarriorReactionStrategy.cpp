#include "botpch.h"
#include "../../playerbot.h"
#include "WarriorReactionStrategy.h"

using namespace ai;

class GenericWarriorReactionStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericWarriorReactionStrategyActionNodeFactory()
    {

    }
};

WarriorReactionStrategy::WarriorReactionStrategy(PlayerbotAI* ai) : ReactionStrategy(ai)
{
    actionNodeFactories.Add(new GenericWarriorReactionStrategyActionNodeFactory());
}

void WarriorReactionStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    ReactionStrategy::InitTriggers(triggers);

    // Add class specific reactions here
    // ...
}