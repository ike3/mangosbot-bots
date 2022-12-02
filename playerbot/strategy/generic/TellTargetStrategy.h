#pragma once
#include "../Strategy.h"

namespace ai
{
    class TellTargetStrategy : public Strategy
    {
    public:
        TellTargetStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "TellTarget"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
