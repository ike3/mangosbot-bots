#pragma once
#include "../Strategy.h"

namespace ai
{
    class TellTargetStrategy : public Strategy
    {
    public:
        TellTargetStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "tell target"; }
    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
