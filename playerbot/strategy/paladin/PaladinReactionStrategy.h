#pragma once
#include "../generic/ReactionStrategy.h"

namespace ai
{
    class PaladinReactionStrategy : public ReactionStrategy
    {
    public:
        PaladinReactionStrategy(PlayerbotAI* ai) : ReactionStrategy(ai) {}
        string getName() override { return "react"; }

    private:
        void InitReactionTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
