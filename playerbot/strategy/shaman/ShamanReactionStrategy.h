#pragma once

#include "../generic/ReactionStrategy.h"

namespace ai
{
    class ShamanReactionStrategy : public ReactionStrategy
    {
    public:
        ShamanReactionStrategy(PlayerbotAI* ai);
        virtual string getName() { return "react"; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };
}
