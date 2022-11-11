#include "botpch.h"
#include "../../playerbot.h"
#include "WarlockReactionStrategy.h"

using namespace ai;

class GenericWarlockReactionStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericWarlockReactionStrategyActionNodeFactory()
    {

    }
};

WarlockReactionStrategy::WarlockReactionStrategy(PlayerbotAI* ai) : ReactionStrategy(ai)
{
    actionNodeFactories.Add(new GenericWarlockReactionStrategyActionNodeFactory());
}

void WarlockReactionStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    ReactionStrategy::InitTriggers(triggers);

    // Add class specific reactions here
    // ...
}