#pragma once
#include "../Strategy.h"

namespace ai
{
    class OnyxiasLairDungeonStrategy : public Strategy
    {
    public:
        OnyxiasLairDungeonStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "onyxia's lair"; }
        void InitTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class OnyxiaFightStrategy : public Strategy
    {
    public:
        OnyxiaFightStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "onyxia"; }
        void InitTriggers(std::list<TriggerNode*>& triggers) override;
    };
}