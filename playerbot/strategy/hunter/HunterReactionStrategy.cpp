#include "botpch.h"
#include "../../playerbot.h"
#include "HunterReactionStrategy.h"

using namespace ai;

void HunterReactionStrategy::InitReactionTriggers(std::list<TriggerNode*> &triggers)
{
    ReactionStrategy::InitReactionTriggers(triggers);

    // Add class specific reactions here
    // ...
}