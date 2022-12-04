#pragma once
#include "../Strategy.h"

namespace ai
{
    class DpsAssistStrategy : public Strategy
    {
    public:
        DpsAssistStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "dps assist"; }
		int GetType() override { return STRATEGY_TYPE_DPS; }

#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "dps assist"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make the bot assist others when picking a target to attack.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "dps aoe" }; }
#endif
    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class DpsAoeStrategy : public Strategy
    {
    public:
        DpsAoeStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "dps aoe"; }
        int GetType() override { return STRATEGY_TYPE_DPS; }

#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "dps aoe"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make the bot use aoe abilities when multiple targets are close to eachother.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "dps assist" }; }
#endif
    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
