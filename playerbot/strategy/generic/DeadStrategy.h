#pragma once
#include "PassTroughStrategy.h"

namespace ai
{
    class DeadStrategy : public PassTroughStrategy
    {
    public:
        DeadStrategy(PlayerbotAI* ai) : PassTroughStrategy(ai) {}
        string getName() override { return "dead"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override {}
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override {}
        void InitDeadTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
