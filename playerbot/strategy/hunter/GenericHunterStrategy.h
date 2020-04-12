#pragma once

#include "../Strategy.h"
#include "../generic/RangedCombatStrategy.h"

namespace ai
{
    class AiObjectContext;

    class GenericHunterStrategy : public RangedCombatStrategy
    {
    public:
        GenericHunterStrategy(PlayerbotAI* ai);

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "hunter"; }
    };

    class HunterBoostStrategy : public Strategy
    {
    public:
        HunterBoostStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        virtual string getName() { return "boost"; }
        virtual NextAction** getDefaultActions();

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };
}

