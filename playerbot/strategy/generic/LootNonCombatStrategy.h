#pragma once
#include "../Strategy.h"

namespace ai
{
    class LootNonCombatStrategy : public Strategy
    {
    public:
        LootNonCombatStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "loot"; }

    public:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class GatherStrategy : public Strategy
    {
    public:
        GatherStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "gather"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class RevealStrategy : public Strategy
    {
    public:
        RevealStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "reveal"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
