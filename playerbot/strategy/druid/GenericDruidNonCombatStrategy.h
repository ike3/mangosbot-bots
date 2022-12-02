#pragma once
#include "../generic/NonCombatStrategy.h"

namespace ai
{
    class GenericDruidNonCombatStrategy : public NonCombatStrategy
    {
    public:
        GenericDruidNonCombatStrategy(PlayerbotAI* ai);
        string getName() override { return "nc"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class GenericDruidBuffStrategy : public NonCombatStrategy
    {
    public:
        GenericDruidBuffStrategy(PlayerbotAI* ai);
        string getName() override { return "buff"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
