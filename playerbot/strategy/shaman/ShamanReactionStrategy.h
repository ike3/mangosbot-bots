#pragma once
#include "../generic/ReactionStrategy.h"

namespace ai
{
    class ShamanReactionStrategy : public ReactionStrategy
    {
    public:
        ShamanReactionStrategy(PlayerbotAI* ai) : ReactionStrategy(ai) {}
        string getName() override { return "react"; }

    private:
        virtual void InitReactionTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
