#pragma once
#include "../Strategy.h"

namespace ai
{
    class AttackEnemyPlayersStrategy : public Strategy
    {
    public:
        AttackEnemyPlayersStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "pvp"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
