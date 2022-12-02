#include "botpch.h"
#include "../../playerbot.h"
#include "PriestReactionStrategy.h"

using namespace ai;

void PriestReactionStrategy::InitReactionTriggers(std::list<TriggerNode*> &triggers)
{
    ReactionStrategy::InitReactionTriggers(triggers);

    // Add class specific reactions here
    // ...
}