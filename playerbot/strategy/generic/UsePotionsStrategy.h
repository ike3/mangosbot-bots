#pragma once
#include "../Strategy.h"

namespace ai
{
    class UsePotionsStrategy : public Strategy
    {
    public:
        UsePotionsStrategy(PlayerbotAI* ai);
        string getName() override { return "potions"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
    };
}
