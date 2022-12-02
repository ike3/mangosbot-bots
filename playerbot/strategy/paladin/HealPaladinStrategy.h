#pragma once
#include "GenericPaladinStrategy.h"

namespace ai
{
    class HealPaladinStrategy : public GenericPaladinStrategy
    {
    public:
        HealPaladinStrategy(PlayerbotAI* ai);
        string getName() override { return "heal"; }
        int GetType() override { return STRATEGY_TYPE_HEAL | STRATEGY_TYPE_MELEE; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
