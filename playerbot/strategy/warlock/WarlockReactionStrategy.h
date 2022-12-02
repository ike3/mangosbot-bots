#pragma once
#include "../generic/ReactionStrategy.h"

namespace ai
{
    class WarlockReactionStrategy : public ReactionStrategy
    {
    public:
        WarlockReactionStrategy(PlayerbotAI* ai) : ReactionStrategy(ai) {}
        string getName() override { return "react"; }

    private:
        virtual void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
    };
}
