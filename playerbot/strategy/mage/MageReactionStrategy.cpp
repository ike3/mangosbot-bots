#include "botpch.h"
#include "../../playerbot.h"
#include "MageReactionStrategy.h"

using namespace ai;

class GenericMageReactionStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericMageReactionStrategyActionNodeFactory()
    {

    }
};

MageReactionStrategy::MageReactionStrategy(PlayerbotAI* ai) : ReactionStrategy(ai)
{
    actionNodeFactories.Add(new GenericMageReactionStrategyActionNodeFactory());
}

void MageReactionStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    ReactionStrategy::InitTriggers(triggers);

    // Add class specific reactions here
    // ...
}