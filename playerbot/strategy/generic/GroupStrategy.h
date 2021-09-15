#include "../generic/NonCombatStrategy.h"
#pragma once

namespace ai
{
    class GroupStrategy : public NonCombatStrategy
    {
    public:
        GroupStrategy(PlayerbotAI* ai) : NonCombatStrategy(ai) {}
        virtual string getName() { return "group"; }
        virtual int GetType() { return STRATEGY_TYPE_GENERIC; }
        NextAction** getDefaultActions();

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };
}
