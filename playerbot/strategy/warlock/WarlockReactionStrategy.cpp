#include "botpch.h"
#include "../../playerbot.h"
#include "WarlockReactionStrategy.h"

using namespace ai;

void WarlockReactionStrategy::InitReactionTriggers(std::list<TriggerNode*> &triggers)
{
    ReactionStrategy::InitReactionTriggers(triggers);

    // Add class specific reactions here
    // ...
}