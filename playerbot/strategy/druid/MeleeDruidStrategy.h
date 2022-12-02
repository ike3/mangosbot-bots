#pragma once
#include "../generic/CombatStrategy.h"

namespace ai
{
    class MeleeDruidStrategy : public CombatStrategy
    {
    public:
        MeleeDruidStrategy(PlayerbotAI* ai);
        string getName() override { return "melee"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        NextAction** GetDefaultCombatActions() override;
    };
}
