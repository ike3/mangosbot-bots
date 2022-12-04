#pragma once
#include "PassTroughStrategy.h"

namespace ai
{
    class DeadStrategy : public PassTroughStrategy
    {
    public:
        DeadStrategy(PlayerbotAI* ai) : PassTroughStrategy(ai) {}
        string getName() override { return "dead"; }

#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "dead"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will includes various behavior when the bot is dead.\n The main goal is to revive in a safe location.";
        }
        virtual vector<string> GetRelatedStrategies() { return { }; }
#endif
    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override {}
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override {}
        void InitDeadTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
