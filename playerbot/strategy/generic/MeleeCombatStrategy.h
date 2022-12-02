#include "CombatStrategy.h"
#include "../generic/CombatStrategy.h"
#pragma once

namespace ai
{
    class MeleeCombatStrategy : public CombatStrategy
    {
    public:
        MeleeCombatStrategy(PlayerbotAI* ai) : CombatStrategy(ai) {}
        
        virtual int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_MELEE; }
        virtual string getName() override { return "close"; }

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class SetBehindCombatStrategy : public CombatStrategy
    {
    public:
        SetBehindCombatStrategy(PlayerbotAI* ai) : CombatStrategy(ai) {}
        string getName() override { return "behind"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
