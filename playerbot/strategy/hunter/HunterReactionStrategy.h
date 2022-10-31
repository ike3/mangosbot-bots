#pragma once

#include "../generic/ReactionStrategy.h"

namespace ai
{
    class HunterReactionStrategy : public ReactionStrategy
    {
    public:
        HunterReactionStrategy(PlayerbotAI* ai);
        virtual string getName() { return "react"; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };
}
