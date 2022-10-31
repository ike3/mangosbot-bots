#pragma once

#include "../generic/ReactionStrategy.h"

namespace ai
{
    class DKReactionStrategy : public ReactionStrategy
    {
    public:
        DKReactionStrategy(PlayerbotAI* ai);
        virtual string getName() { return "react"; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };
}
