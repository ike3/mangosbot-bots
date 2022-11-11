#pragma once

#include "../generic/ReactionStrategy.h"

namespace ai
{
    class MageReactionStrategy : public ReactionStrategy
    {
    public:
        MageReactionStrategy(PlayerbotAI* ai);
        virtual string getName() { return "react"; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };
}
