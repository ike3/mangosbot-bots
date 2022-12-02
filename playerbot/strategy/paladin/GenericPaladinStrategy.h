#pragma once

#include "../Strategy.h"
#include "PaladinAiObjectContext.h"
#include "../generic/CombatStrategy.h"

namespace ai
{
    class GenericPaladinStrategy : public CombatStrategy
    {
    public:
        GenericPaladinStrategy(PlayerbotAI* ai);
        virtual string getName() override { return "paladin"; }

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class PaladinCureStrategy : public Strategy
    {
    public:
        PaladinCureStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "cure"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class PaladinBoostStrategy : public Strategy
    {
    public:
        PaladinBoostStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "boost"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class PaladinCcStrategy : public Strategy
    {
    public:
        PaladinCcStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "cc"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
