#pragma once
#include "../Strategy.h"

namespace ai
{
    class DungeonStrategy : public Strategy
    {
    public:
        DungeonStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "dungeon"; }
        void InitTriggers(std::list<TriggerNode*>& triggers) override;
    };
}