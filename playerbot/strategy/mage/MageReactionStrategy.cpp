#include "botpch.h"
#include "../../playerbot.h"
#include "MageReactionStrategy.h"

using namespace ai;

void MageReactionStrategy::InitReactionTriggers(std::list<TriggerNode*> &triggers)
{
    ReactionStrategy::InitReactionTriggers(triggers);

    // Add class specific reactions here
    // ...
}