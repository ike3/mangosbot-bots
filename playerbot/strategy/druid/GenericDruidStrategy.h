#pragma once
#include "../generic/CombatStrategy.h"

namespace ai
{
    class AiObjectContext;

    class GenericDruidStrategy : public CombatStrategy
    {
    public:
        GenericDruidStrategy(PlayerbotAI* ai);

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class DruidCureStrategy : public Strategy
    {
    public:
        DruidCureStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "cure"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class DruidBoostStrategy : public Strategy
    {
    public:
        DruidBoostStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "boost"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class DruidCcStrategy : public Strategy
    {
    public:
        DruidCcStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "cc"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
