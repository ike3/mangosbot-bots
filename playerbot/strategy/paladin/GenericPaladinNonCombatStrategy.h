#pragma once

#include "../generic/NonCombatStrategy.h"

namespace ai
{
    class GenericPaladinNonCombatStrategy : public NonCombatStrategy
    {
    public:
        GenericPaladinNonCombatStrategy(PlayerbotAI* ai);
        string getName() override { return "nc"; }
    
    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
   };
}
