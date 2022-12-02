#pragma once
#include "../generic/CombatStrategy.h"

namespace ai
{
    class GenericWarlockStrategy : public CombatStrategy
    {
    public:
        GenericWarlockStrategy(PlayerbotAI* ai);
        virtual string getName() override { return "warlock"; }

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        virtual NextAction** GetDefaultCombatActions() override;
    };

    class WarlockBoostStrategy : public Strategy
    {
    public:
        WarlockBoostStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        string getName() override { return "boost"; }

    private:
        virtual void InitCombatTriggers(std::list<TriggerNode*> &triggers);
    };

    class WarlockCcStrategy : public Strategy
    {
    public:
        WarlockCcStrategy(PlayerbotAI* ai) : Strategy(ai) {};
        string getName() override { return "cc"; }

    public:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
