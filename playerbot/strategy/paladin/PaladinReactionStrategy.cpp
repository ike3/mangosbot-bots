#include "botpch.h"
#include "../../playerbot.h"
#include "PaladinReactionStrategy.h"

using namespace ai;

void PaladinReactionStrategy::InitReactionTriggers(std::list<TriggerNode*> &triggers)
{
    ReactionStrategy::InitReactionTriggers(triggers);

    // Add class specific reactions here
    // ...
}