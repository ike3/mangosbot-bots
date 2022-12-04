#pragma once
#include "PassTroughStrategy.h"

namespace ai
{
    class DuelStrategy : public PassTroughStrategy
    {
    public:
        DuelStrategy(PlayerbotAI* ai) : PassTroughStrategy(ai) {}
        string getName() override { return "duel"; }

#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "duel"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will allow bots to accept duels and attack their duel target.";
        }
        virtual vector<string> GetRelatedStrategies() { return { "start duel" }; }
#endif
    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override {}
    };

    class StartDuelStrategy : public Strategy
    {
    public:
        StartDuelStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "start duel"; }

#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "start duel"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will allow bots to start duels with other bots if they are the current [h:value|rpg target].";
        }
        virtual vector<string> GetRelatedStrategies() { return { "duel", "rpg", "rpg player" }; }
#endif
    };
}
