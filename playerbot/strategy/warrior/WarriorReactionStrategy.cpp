#include "botpch.h"
#include "../../playerbot.h"
#include "WarriorReactionStrategy.h"

using namespace ai;

void WarriorReactionStrategy::InitReactionTriggers(std::list<TriggerNode*> &triggers)
{
    ReactionStrategy::InitReactionTriggers(triggers);

    // Add class specific reactions here
    // ...
}