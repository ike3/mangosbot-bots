#pragma once

#include "GenericDruidStrategy.h"
#include "../generic/CombatStrategy.h"

namespace ai
{
    class CasterDruidStrategy : public GenericDruidStrategy
    {
    public:
        CasterDruidStrategy(PlayerbotAI* ai);
        string getName() override { return "caster"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_RANGED; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        NextAction** GetDefaultCombatActions() override;
    };

    class CasterDruidAoeStrategy : public CombatStrategy
    {
    public:
        CasterDruidAoeStrategy(PlayerbotAI* ai) : CombatStrategy(ai) {}
        string getName() override { return "caster aoe"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class CasterDruidDebuffStrategy : public CombatStrategy
    {
    public:
        CasterDruidDebuffStrategy(PlayerbotAI* ai) : CombatStrategy(ai) {}
        string getName() override { return "caster debuff"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
