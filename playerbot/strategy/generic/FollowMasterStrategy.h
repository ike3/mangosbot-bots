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
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "follow"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will make the bot stay near the master";
        }
        virtual vector<string> GetRelatedStrategies() { return { "stay", "runaway","flee from adds", "guard", "free"}; }
#endif
    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitDeadTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;

        virtual void OnStrategyAdded(BotState state) override;
        virtual void OnStrategyRemoved(BotState state) override;
    };

    class FreeStrategy : public Strategy
    {
    public:
        FreeStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        int GetType() override { return STRATEGY_TYPE_NONCOMBAT; }
        string getName() override { return "free"; }
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "free"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy will allow the bot to move freely";
        }
        virtual vector<string> GetRelatedStrategies() { return { "follow", "stay", "runaway","flee from adds", "guard" }; }
#endif
    };

    class FollowJumpStrategy : public Strategy
    {
    public:
        FollowJumpStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        string getName() override { return "follow jump"; }
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "follow jump"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy makes bot jump when following.\n";
        }
        virtual vector<string> GetRelatedStrategies() { return { "follow" }; }
#endif
    private:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
    };
}
