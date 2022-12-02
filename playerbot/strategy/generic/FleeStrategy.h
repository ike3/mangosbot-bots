#pragma once
#include "../Strategy.h"

namespace ai
{
    class FleeStrategy : public Strategy
    {
    public:
        FleeStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "flee"; };

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class FleeFromAddsStrategy : public Strategy
    {
    public:
        FleeFromAddsStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "flee from adds"; };

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
