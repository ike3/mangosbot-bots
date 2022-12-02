#pragma once

#include "../Strategy.h"
#include "../generic/NonCombatStrategy.h"

namespace ai
{
    class ShamanNonCombatStrategy : public NonCombatStrategy
    {
    public:
        ShamanNonCombatStrategy(PlayerbotAI* ai) : NonCombatStrategy(ai) {}
        string getName() override { return "nc"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
