#pragma once
#include "../Strategy.h"

namespace ai
{
    class DpsAssistStrategy : public Strategy
    {
    public:
        DpsAssistStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "dps assist"; }
		int GetType() override { return STRATEGY_TYPE_DPS; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class DpsAoeStrategy : public Strategy
    {
    public:
        DpsAoeStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "dps aoe"; }
        int GetType() override { return STRATEGY_TYPE_DPS; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
