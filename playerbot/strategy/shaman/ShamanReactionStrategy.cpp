#include "botpch.h"
#include "../../playerbot.h"
#include "ShamanReactionStrategy.h"

using namespace ai;

class GenericShamanReactionStrategyActionNodeFactory : public NamedObjectFactory<ActionNode>
{
public:
    GenericShamanReactionStrategyActionNodeFactory()
    {

    }
};

ShamanReactionStrategy::ShamanReactionStrategy(PlayerbotAI* ai) : ReactionStrategy(ai)
{
    actionNodeFactories.Add(new GenericShamanReactionStrategyActionNodeFactory());
}

void ShamanReactionStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    ReactionStrategy::InitTriggers(triggers);

    // Add class specific reactions here
    // ...
}