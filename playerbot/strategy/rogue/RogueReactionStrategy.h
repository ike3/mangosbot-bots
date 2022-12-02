#pragma once
#include "../generic/ReactionStrategy.h"

namespace ai
{
    class RogueReactionStrategy : public ReactionStrategy
    {
    public:
        RogueReactionStrategy(PlayerbotAI* ai) : ReactionStrategy(ai) {}
        string getName() override { return "react"; }

    public:
        void InitReactionTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
