#pragma once
#include "../Strategy.h"

namespace ai
{
    class MarkRtiStrategy : public Strategy
    {
    public:
        MarkRtiStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "mark rti"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
