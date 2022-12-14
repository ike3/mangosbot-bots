#pragma once
#include "../Strategy.h"

namespace ai
{
    class TravelStrategy : public Strategy
    {
    public:
        TravelStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "travel"; }

    public:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
        NextAction** GetDefaultNonCombatActions() override;
    };

    class TravelOnceStrategy : public TravelStrategy
    {
    public:
        TravelOnceStrategy(PlayerbotAI* ai) : TravelStrategy(ai) {}
        string getName() override { return "travel once"; }
    };

    class ExploreStrategy : public Strategy
    {
    public:
        ExploreStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        string getName() override { return "explore"; }
    };

    class MapStrategy : public Strategy
    {
    public:
        MapStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        string getName() override { return "map"; }
    };

    class MapFullStrategy : public Strategy
    {
    public:
        MapFullStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        string getName() override { return "map full"; }
    };
}
