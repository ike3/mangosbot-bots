#pragma once
#include "../Strategy.h"

namespace ai
{
    class MoltenCoreDungeonStrategy : public Strategy
    {
    public:
        MoltenCoreDungeonStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "molten core"; }
        void InitTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class MagmadarFightStrategy : public Strategy
    {
    public:
        MagmadarFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "magmadar"; }
        void InitTriggers(std::list<TriggerNode*>& triggers) override;
    };
}