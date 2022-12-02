#pragma once

#include "../Strategy.h"
#include "../generic/CombatStrategy.h"

namespace ai
{
    class GenericMageStrategy : public CombatStrategy
    {
    public:
        GenericMageStrategy(PlayerbotAI* ai);
        virtual string getName() override { return "mage"; }

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class MageCureStrategy : public Strategy
    {
    public:
        MageCureStrategy(PlayerbotAI* ai);
        string getName() override { return "cure"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class MageBoostStrategy : public Strategy
    {
    public:
        MageBoostStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "boost"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class MageCcStrategy : public Strategy
    {
    public:
        MageCcStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() { return "cc"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
