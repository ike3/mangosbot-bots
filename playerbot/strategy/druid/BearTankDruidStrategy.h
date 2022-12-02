#pragma once
#include "FeralDruidStrategy.h"

namespace ai
{
    class BearTankDruidStrategy : public FeralDruidStrategy
    {
    public:
        BearTankDruidStrategy(PlayerbotAI* ai);
        string getName() override { return "bear"; }
        int GetType() override { return STRATEGY_TYPE_TANK | STRATEGY_TYPE_MELEE; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        NextAction** GetDefaultCombatActions() override;
    };
}
