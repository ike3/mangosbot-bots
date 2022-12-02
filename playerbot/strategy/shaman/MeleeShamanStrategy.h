#pragma once
#include "GenericShamanStrategy.h"

namespace ai
{
    class MeleeShamanStrategy : public GenericShamanStrategy
    {
    public:
        MeleeShamanStrategy(PlayerbotAI* ai);
        string getName() override { return "melee"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        NextAction** GetDefaultCombatActions() override;
    };

    class MeleeAoeShamanStrategy : public CombatStrategy
    {
    public:
        MeleeAoeShamanStrategy(PlayerbotAI* ai) : CombatStrategy(ai) {}
        string getName() override { return "melee aoe"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
