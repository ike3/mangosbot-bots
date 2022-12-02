#pragma once

#include "../generic/ReactionStrategy.h"

namespace ai
{
    class WarriorReactionStrategy : public ReactionStrategy
    {
    public:
        WarriorReactionStrategy(PlayerbotAI* ai) : ReactionStrategy(ai) {}
        string getName() override { return "react"; }

    private:
        void InitReactionTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
