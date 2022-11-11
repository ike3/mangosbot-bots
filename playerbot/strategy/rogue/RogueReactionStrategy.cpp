#include "botpch.h"
#include "../../playerbot.h"
#include "RogueReactionStrategy.h"

using namespace ai;

class GenericRogueReactionStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericRogueReactionStrategyActionNodeFactory()
    {

    }
};

RogueReactionStrategy::RogueReactionStrategy(PlayerbotAI* ai) : ReactionStrategy(ai)
{
    actionNodeFactories.Add(new GenericRogueReactionStrategyActionNodeFactory());
}

void RogueReactionStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    ReactionStrategy::InitTriggers(triggers);

    // Add class specific reactions here
    // ...
}