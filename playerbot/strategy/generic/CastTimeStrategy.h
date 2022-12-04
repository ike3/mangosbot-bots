#pragma once
#include "../Multiplier.h"
#include "../Strategy.h"

namespace ai
{
    class CastTimeMultiplier : public Multiplier
    {
    public:
        CastTimeMultiplier(PlayerbotAI* ai) : Multiplier(ai, "cast time") {}

    public:
        virtual float GetValue(Action* action);
    };

    class CastTimeStrategy : public Strategy
    {
    public:
        CastTimeStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "cast time"; }
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "cast time"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make bots less likely to cast long casttime spells when the target is at critical health.";
        }
        virtual vector<string> GetRelatedStrategies() { return { }; }
#endif
        void InitCombatMultipliers(std::list<Multiplier*>& multipliers);
    };
}
