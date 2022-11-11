#pragma once

#include "../generic/ReactionStrategy.h"

namespace ai
{
    class RogueReactionStrategy : public ReactionStrategy
    {
    public:
        RogueReactionStrategy(PlayerbotAI* ai);
        virtual string getName() { return "react"; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };
}
