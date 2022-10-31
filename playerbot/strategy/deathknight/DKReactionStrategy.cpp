#include "botpch.h"
#include "../../playerbot.h"
#include "DKReactionStrategy.h"

using namespace ai;

class GenericDKReactionStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericDKReactionStrategyActionNodeFactory()
    {

    }
};

DKReactionStrategy::DKReactionStrategy(PlayerbotAI* ai) : ReactionStrategy(ai)
{
    actionNodeFactories.Add(new GenericDKReactionStrategyActionNodeFactory());
}

void DKReactionStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    ReactionStrategy::InitTriggers(triggers);

    // Add class specific reactions here
    // ...
}