#pragma once
#include "NonCombatStrategy.h"

namespace ai
{
    class GuardStrategy : public NonCombatStrategy
    {
    public:
        GuardStrategy(PlayerbotAI* ai) : NonCombatStrategy(ai) {}
        string getName() override { return "guard"; }
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "guard"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This a position strategy that will make the bot stay in a location until they have something to attack.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "follow", "stay", "runaway", "flee from adds" }; }
#endif
    private:
        NextAction** GetDefaultNonCombatActions() override;
    };
}
