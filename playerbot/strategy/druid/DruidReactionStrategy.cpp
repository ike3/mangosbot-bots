#include "botpch.h"
#include "../../playerbot.h"
#include "DruidReactionStrategy.h"

using namespace ai;

class GenericDruidReactionStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericDruidReactionStrategyActionNodeFactory()
    {

    }
};

DruidReactionStrategy::DruidReactionStrategy(PlayerbotAI* ai) : ReactionStrategy(ai)
{
    actionNodeFactories.Add(new GenericDruidReactionStrategyActionNodeFactory());
}

void DruidReactionStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    ReactionStrategy::InitTriggers(triggers);

    // Add class specific reactions here
    // ...
}