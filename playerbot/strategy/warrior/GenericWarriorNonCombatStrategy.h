#pragma once
#include "../generic/NonCombatStrategy.h"

namespace ai
{
    class GenericWarriorNonCombatStrategy : public NonCombatStrategy
    {
    public:
        GenericWarriorNonCombatStrategy(PlayerbotAI* ai) : NonCombatStrategy(ai) {}
        string getName() override { return "nc"; }
	
    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
   };
}