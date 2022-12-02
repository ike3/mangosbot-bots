#pragma once
#include "GenericMageStrategy.h"

namespace ai
{
    class ArcaneMageStrategy : public GenericMageStrategy
    {
    public:
        ArcaneMageStrategy(PlayerbotAI* ai);
        string getName() override { return "arcane"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        NextAction** GetDefaultCombatActions() override;
    };

    class ArcaneMageAoeStrategy : public CombatStrategy
    {
    public:
        ArcaneMageAoeStrategy(PlayerbotAI* ai) : CombatStrategy(ai) {}
        string getName() override { return "arcane aoe"; }

    public:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };
}
