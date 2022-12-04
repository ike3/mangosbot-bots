#pragma once
#include "../Strategy.h"

namespace ai
{
    class FollowMasterStrategy : public Strategy
    {
    public:
        FollowMasterStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        int GetType() override { return STRATEGY_TYPE_NONCOMBAT; }
        string getName() override { return "follow"; }
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "follow"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make the bot stay near the master";
        }
        virtual vector<string> GetRelatedStrategies() { return { "stay", "runaway","flee from adds", "guard" }; }
#endif
    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
    };
}
