#pragma once
#include "../Strategy.h"

namespace ai
{
    class RacialsStrategy : public Strategy
    {
    public:
        RacialsStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "racials"; }
    
    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };
}