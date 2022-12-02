#pragma once
#include "GenericWarriorStrategy.h"

namespace ai
{
    class ArmsWarriorStrategy : public GenericWarriorStrategy
    {
    public:
        ArmsWarriorStrategy(PlayerbotAI* ai);
        string getName() override { return "arms"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        NextAction** GetDefaultCombatActions() override;
    };
}
