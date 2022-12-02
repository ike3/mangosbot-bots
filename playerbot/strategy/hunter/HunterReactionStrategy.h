#pragma once
#include "../generic/ReactionStrategy.h"

namespace ai
{
    class HunterReactionStrategy : public ReactionStrategy
    {
    public:
        HunterReactionStrategy(PlayerbotAI* ai) : ReactionStrategy(ai) {}
        string getName() override { return "react"; }

    public:
        void InitReactionTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
