#include "../generic/NonCombatStrategy.h"
#pragma once

namespace ai
{
    class StayStrategy : public Strategy
    {
    public:
        StayStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        virtual string getName() { return "stay"; }
        virtual void InitTriggers(std::list<TriggerNode*>& triggers);
        virtual NextAction** getDefaultActions();

        void OnStrategyAdded() override;
        void OnStrategyRemoved() override;
    };

    class SitStrategy : public NonCombatStrategy
    {
    public:
        SitStrategy(PlayerbotAI* ai) : NonCombatStrategy(ai) {}
        virtual string getName() { return "sit"; }
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };
}
