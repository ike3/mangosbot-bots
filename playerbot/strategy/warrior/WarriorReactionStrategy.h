#pragma once

#include "../generic/ReactionStrategy.h"

namespace ai
{
    class WarriorReactionStrategy : public ReactionStrategy
    {
    public:
        WarriorReactionStrategy(PlayerbotAI* ai);
        virtual string getName() { return "react"; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };
}
