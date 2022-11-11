#pragma once

#include "../generic/ReactionStrategy.h"

namespace ai
{
    class PaladinReactionStrategy : public ReactionStrategy
    {
    public:
        PaladinReactionStrategy(PlayerbotAI* ai);
        virtual string getName() { return "react"; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };
}
