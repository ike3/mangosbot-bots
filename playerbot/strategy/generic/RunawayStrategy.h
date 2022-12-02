#pragma once
#include "../Strategy.h"

namespace ai
{
    class RunawayStrategy : public Strategy
    {
    public:
        RunawayStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "runaway"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
