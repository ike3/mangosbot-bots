#pragma once
#include "../Multiplier.h"
#include "../Strategy.h"

namespace ai
{
    class ThreatMultiplier : public Multiplier
    {
    public:
        ThreatMultiplier(PlayerbotAI* ai) : Multiplier(ai, "threat") {}

    public:
        float GetValue(Action* action) override;
    };

    class ThreatStrategy : public Strategy
    {
    public:
        ThreatStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "threat"; }

    private:
        void InitCombatMultipliers(std::list<Multiplier*> &multipliers) override;
    };
}
