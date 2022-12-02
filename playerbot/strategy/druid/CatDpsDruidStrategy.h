#pragma once
#include "FeralDruidStrategy.h"

namespace ai
{
    class CatDpsDruidStrategy : public FeralDruidStrategy
    {
    public:
        CatDpsDruidStrategy(PlayerbotAI* ai);
        string getName() override { return "cat"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_MELEE; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        NextAction** GetDefaultCombatActions() override;
    };

    class CatAoeDruidStrategy : public CombatStrategy
    {
    public:
        CatAoeDruidStrategy(PlayerbotAI* ai) : CombatStrategy(ai) {}
        string getName() override { return "cat aoe"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
