#include "botpch.h"
#include "../../playerbot.h"
#include "PaladinReactionStrategy.h"

using namespace ai;

class GenericPaladinReactionStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericPaladinReactionStrategyActionNodeFactory()
    {

    }
};

PaladinReactionStrategy::PaladinReactionStrategy(PlayerbotAI* ai) : ReactionStrategy(ai)
{
    actionNodeFactories.Add(new GenericPaladinReactionStrategyActionNodeFactory());
}

void PaladinReactionStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    ReactionStrategy::InitTriggers(triggers);

    // Add class specific reactions here
    // ...
}