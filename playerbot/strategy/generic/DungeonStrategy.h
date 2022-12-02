#pragma once
#include "../Strategy.h"

namespace ai
{
    class DungeonStrategy : public Strategy
    {
    public:
        DungeonStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "dungeon"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };
}