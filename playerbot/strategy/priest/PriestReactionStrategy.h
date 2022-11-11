#pragma once

#include "../generic/ReactionStrategy.h"

namespace ai
{
    class PriestReactionStrategy : public ReactionStrategy
    {
    public:
        PriestReactionStrategy(PlayerbotAI* ai);
        virtual string getName() { return "react"; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };
}
