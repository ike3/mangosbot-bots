#include "botpch.h"
#include "../../playerbot.h"
#include "DruidReactionStrategy.h"

using namespace ai;

void DruidReactionStrategy::InitReactionTriggers(std::list<TriggerNode*> &triggers)
{
    ReactionStrategy::InitReactionTriggers(triggers);

    // Add class specific reactions here
    // ...
}