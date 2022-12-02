#pragma once

#include "../Strategy.h"
#include "../generic/NonCombatStrategy.h"

namespace ai
{
    class PriestNonCombatStrategy : public NonCombatStrategy
    {
    public:
        PriestNonCombatStrategy(PlayerbotAI* ai);
        string getName() override { return "nc"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class PriestBuffStrategy : public NonCombatStrategy
    {
    public:
        PriestBuffStrategy(PlayerbotAI* ai) : NonCombatStrategy(ai) {}
        string getName() override { return "buff"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class PriestShadowResistanceStrategy : public NonCombatStrategy
    {
    public:
        PriestShadowResistanceStrategy(PlayerbotAI* ai) : NonCombatStrategy(ai) {}
        string getName() override { return "rshadow"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
