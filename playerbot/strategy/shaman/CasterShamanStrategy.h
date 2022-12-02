#pragma once
#include "GenericShamanStrategy.h"

namespace ai
{
    class CasterShamanStrategy : public GenericShamanStrategy
    {
    public:
        CasterShamanStrategy(PlayerbotAI* ai);
        string getName() override { return "caster"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_RANGED; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        NextAction** GetDefaultCombatActions() override;
    };

    class CasterAoeShamanStrategy : public CombatStrategy
    {
    public:
        CasterAoeShamanStrategy(PlayerbotAI* ai) : CombatStrategy(ai) {}
        string getName() override { return "caster aoe"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
