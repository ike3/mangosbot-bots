#pragma once
#include "../generic/NonCombatStrategy.h"

namespace ai
{
    class GenericDKNonCombatStrategy : public NonCombatStrategy
    {
    public:
        GenericDKNonCombatStrategy(PlayerbotAI* ai);
        string getName() override { return "nc"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class DKBuffDpsStrategy : public Strategy
    {
    public:
        DKBuffDpsStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "bdps"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };
}
