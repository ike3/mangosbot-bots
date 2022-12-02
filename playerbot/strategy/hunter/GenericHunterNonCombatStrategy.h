#pragma once
#include "../generic/NonCombatStrategy.h"

namespace ai
{
    class GenericHunterNonCombatStrategy : public NonCombatStrategy
    {
    public:
        GenericHunterNonCombatStrategy(PlayerbotAI* ai);
        string getName() override { return "nc"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class HunterPetStrategy : public NonCombatStrategy
    {
    public:
        HunterPetStrategy(PlayerbotAI* ai) : NonCombatStrategy(ai) {}
        string getName() override { return "pet"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
