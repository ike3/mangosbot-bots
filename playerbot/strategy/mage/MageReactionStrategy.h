#pragma once
#include "../generic/ReactionStrategy.h"

namespace ai
{
    class MageReactionStrategy : public ReactionStrategy
    {
    public:
        MageReactionStrategy(PlayerbotAI* ai) : ReactionStrategy(ai) {}
        string getName() override { return "react"; }

    public:
        void InitReactionTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
