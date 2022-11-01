#include "botpch.h"
#include "../../playerbot.h"
#include "HunterReactionStrategy.h"

using namespace ai;

class GenericHunterReactionStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericHunterReactionStrategyActionNodeFactory()
    {

    }
};

HunterReactionStrategy::HunterReactionStrategy(PlayerbotAI* ai) : ReactionStrategy(ai)
{
    actionNodeFactories.Add(new GenericHunterReactionStrategyActionNodeFactory());
}

void HunterReactionStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    ReactionStrategy::InitTriggers(triggers);

    // Add class specific reactions here
    // ...
}