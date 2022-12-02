#pragma once

#include "GenericMageStrategy.h"
#include "../generic/CombatStrategy.h"

namespace ai
{
    class FrostMageStrategy : public GenericMageStrategy
    {
    public:
        FrostMageStrategy(PlayerbotAI* ai);
        string getName() override { return "frost"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        NextAction** GetDefaultCombatActions() override;
    };

    class FrostMageAoeStrategy : public CombatStrategy
    {
    public:
        FrostMageAoeStrategy(PlayerbotAI* ai) : CombatStrategy(ai) {}
        string getName() override { return "frost aoe"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
