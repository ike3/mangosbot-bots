#include "botpch.h"
#include "../../playerbot.h"
#include "PriestReactionStrategy.h"

using namespace ai;

class GenericPriestReactionStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericPriestReactionStrategyActionNodeFactory()
    {

    }
};

PriestReactionStrategy::PriestReactionStrategy(PlayerbotAI* ai) : ReactionStrategy(ai)
{
    actionNodeFactories.Add(new GenericPriestReactionStrategyActionNodeFactory());
}

void PriestReactionStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    ReactionStrategy::InitTriggers(triggers);

    // Add class specific reactions here
    // ...
}