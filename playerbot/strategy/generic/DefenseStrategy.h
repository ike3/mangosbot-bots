#include "../generic/NonCombatStrategy.h"
#pragma once

namespace ai
{
    class DefenseStrategy : public NonCombatStrategy
    {
    public:
        DefenseStrategy(PlayerbotAI* ai) : NonCombatStrategy(ai) {}
        virtual string getName() { return "defense"; }
		virtual int GetType() { return STRATEGY_TYPE_DPS; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };
}
