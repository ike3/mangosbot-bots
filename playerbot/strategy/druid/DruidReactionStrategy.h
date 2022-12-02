#pragma once
#include "../generic/ReactionStrategy.h"

namespace ai
{
    class DruidReactionStrategy : public ReactionStrategy
    {
    public:
        DruidReactionStrategy(PlayerbotAI* ai) : ReactionStrategy(ai) {}
        string getName() override { return "react"; }

    private:
        void InitReactionTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
