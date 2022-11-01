#pragma once

#include "../generic/ReactionStrategy.h"

namespace ai
{
    class WarlockReactionStrategy : public ReactionStrategy
    {
    public:
        WarlockReactionStrategy(PlayerbotAI* ai);
        virtual string getName() { return "react"; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };
}
