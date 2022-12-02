#pragma once
#include "../Strategy.h"

namespace ai
{
    class FollowMasterStrategy : public Strategy
    {
    public:
        FollowMasterStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        int GetType() override { return STRATEGY_TYPE_NONCOMBAT; }
        string getName() override { return "follow"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
    };
}
