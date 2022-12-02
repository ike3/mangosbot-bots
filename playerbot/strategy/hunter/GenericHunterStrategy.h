#pragma once
#include "../generic/CombatStrategy.h"

namespace ai
{
    class AiObjectContext;

    class GenericHunterStrategy : public CombatStrategy
    {
    public:
        GenericHunterStrategy(PlayerbotAI* ai);
        virtual string getName() override { return "hunter"; }

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class HunterBoostStrategy : public Strategy
    {
    public:
        HunterBoostStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "boost"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        NextAction** GetDefaultCombatActions() override;
    };

    class HunterCcStrategy : public Strategy
    {
    public:
        HunterCcStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "cc"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}