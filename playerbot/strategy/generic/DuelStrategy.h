#pragma once
#include "PassTroughStrategy.h"

namespace ai
{
    class DuelStrategy : public PassTroughStrategy
    {
    public:
        DuelStrategy(PlayerbotAI* ai) : PassTroughStrategy(ai) {}
        string getName() override { return "duel"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override {}
    };

    class StartDuelStrategy : public Strategy
    {
    public:
        StartDuelStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "start duel"; }
    };
}
