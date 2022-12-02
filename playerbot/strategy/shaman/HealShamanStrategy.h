#pragma once
#include "GenericShamanStrategy.h"

namespace ai
{
    class HealShamanStrategy : public GenericShamanStrategy
    {
    public:
        HealShamanStrategy(PlayerbotAI* ai);
        string getName() override { return "heal"; }
        int GetType() override { return STRATEGY_TYPE_HEAL; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
