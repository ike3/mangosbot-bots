#pragma once
#include "../Strategy.h"

namespace ai
{
    class AttackEnemyPlayersStrategy : public Strategy
    {
    public:
        AttackEnemyPlayersStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "pvp"; }
#ifndef GenerateBotHelp
        virtual string GetHelpName() { return "pvp"; } //Must equal iternal name
        virtual string GetHelpDescription() {
            return "This strategy detect nearby enemy players and makes the bot attack them.";
        }
        virtual vector<string> GetRelatedStrategies() { return {}; }
#endif
    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
