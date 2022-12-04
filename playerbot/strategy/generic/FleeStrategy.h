#pragma once
#include "../Strategy.h"

namespace ai
{
    class FleeStrategy : public Strategy
    {
    public:
        FleeStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "flee"; };
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "flee"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make the bot flee when it is in danger.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "flee from adds" }; }
#endif
    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class FleeFromAddsStrategy : public Strategy
    {
    public:
        FleeFromAddsStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "flee from adds"; };
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "flee from adds"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This a position strategy that will make the bot try to avoid adds the prevent aggro.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "flee", "follow", "stay", "runaway", "guard" }; }
#endif
    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
