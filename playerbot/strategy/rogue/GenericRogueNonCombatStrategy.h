#pragma once
#include "../generic/NonCombatStrategy.h"

namespace ai
{
    class GenericRogueNonCombatStrategy : public NonCombatStrategy
    {
    public:
        GenericRogueNonCombatStrategy(PlayerbotAI* ai) : NonCombatStrategy(ai) {}
        string getName() override { return "nc"; }
    
    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}