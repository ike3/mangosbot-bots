#pragma once
#include "../generic/ReactionStrategy.h"

namespace ai
{
    class PriestReactionStrategy : public ReactionStrategy
    {
    public:
        PriestReactionStrategy(PlayerbotAI* ai) : ReactionStrategy(ai) {}
        string getName() override { return "react"; }

    private:
        void InitReactionTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
