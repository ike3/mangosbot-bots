#pragma once
#include "../generic/NonCombatStrategy.h"

namespace ai
{
    class GenericMageNonCombatStrategy : public NonCombatStrategy
    {
    public:
        GenericMageNonCombatStrategy(PlayerbotAI* ai);
        string getName() override { return "nc"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class MageBuffManaStrategy : public Strategy
    {
    public:
        MageBuffManaStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "bmana"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class MageBuffDpsStrategy : public Strategy
    {
    public:
        MageBuffDpsStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "bdps"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class MageBuffStrategy : public Strategy
    {
    public:
        MageBuffStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "buff"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
