#pragma once

#include "GenericPaladinStrategy.h"

namespace ai
{
    class DpsPaladinStrategy : public GenericPaladinStrategy
    {
    public:
        DpsPaladinStrategy(PlayerbotAI* ai);
        string getName() override { return "dps"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        NextAction** GetDefaultCombatActions() override;
    };
}
