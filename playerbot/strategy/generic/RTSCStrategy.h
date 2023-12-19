#pragma once
#include "../Strategy.h"

namespace ai
{
    class RTSCStrategy : public Strategy
    {
    public:
        RTSCStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "rtsc"; }
    };

    class RTSCSJumptrategy : public Strategy
    {
    public:
        RTSCSJumptrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "rtsc jump"; }

    protected:
        virtual void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
