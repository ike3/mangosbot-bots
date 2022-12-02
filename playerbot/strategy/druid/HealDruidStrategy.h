#pragma once
#include "GenericDruidStrategy.h"

namespace ai
{
    class HealDruidStrategy : public GenericDruidStrategy
    {
    public:
        HealDruidStrategy(PlayerbotAI* ai);
        string getName() override { return "heal"; }
        int GetType() override { return STRATEGY_TYPE_HEAL; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
