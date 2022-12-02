#pragma once
#include "../generic/ReactionStrategy.h"

namespace ai
{
    class DKReactionStrategy : public ReactionStrategy
    {
    public:
        DKReactionStrategy(PlayerbotAI* ai) : ReactionStrategy(ai) {}
        string getName() override { return "react"; }

    private:
        void InitReactionTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
