#pragma once

#include "GenericPaladinStrategy.h"

namespace ai
{
    class TankPaladinStrategy : public GenericPaladinStrategy
    {
    public:
        TankPaladinStrategy(PlayerbotAI* ai);
        string getName() override { return "tank"; }
        int GetType() override { return STRATEGY_TYPE_TANK | STRATEGY_TYPE_MELEE; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        NextAction** GetDefaultCombatActions() override;
    };
}
