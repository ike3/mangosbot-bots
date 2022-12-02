#pragma once
#include "../Strategy.h"

namespace ai
{
    class SayStrategy : public Strategy
    {
    public:
        SayStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "say"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };
}
