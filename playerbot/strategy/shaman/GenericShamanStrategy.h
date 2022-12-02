#pragma once
#include "../generic/CombatStrategy.h"

namespace ai
{
    class GenericShamanStrategy : public CombatStrategy
    {
    public:
        GenericShamanStrategy(PlayerbotAI* ai);

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class ShamanBuffDpsStrategy : public Strategy
    {
    public:
        ShamanBuffDpsStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "bdps"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class ShamanBuffManaStrategy : public Strategy
    {
    public:
        ShamanBuffManaStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "bmana"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class ShamanCureStrategy : public Strategy
    {
    public:
        ShamanCureStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "cure"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };
}
