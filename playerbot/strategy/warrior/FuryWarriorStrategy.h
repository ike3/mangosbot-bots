#pragma once
#include "GenericWarriorStrategy.h"

namespace ai
{
    class FuryWarriorStrategy : public GenericWarriorStrategy
    {
    public:
        FuryWarriorStrategy(PlayerbotAI* ai);
        string getName() override { return "fury"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        NextAction** GetDefaultCombatActions() override;
    };
}
