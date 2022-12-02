#pragma once
#include "GenericWarlockStrategy.h"

namespace ai
{
    class DpsWarlockStrategy : public GenericWarlockStrategy
    {
    public:
        DpsWarlockStrategy(PlayerbotAI* ai);
        string getName() override { return "dps"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        NextAction** GetDefaultCombatActions() override;
    };

    class DpsAoeWarlockStrategy : public CombatStrategy
    {
    public:
        DpsAoeWarlockStrategy(PlayerbotAI* ai) : CombatStrategy(ai) {}
        string getName() override { return "aoe"; }

    private:
        virtual void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class DpsWarlockDebuffStrategy : public CombatStrategy
    {
    public:
        DpsWarlockDebuffStrategy(PlayerbotAI* ai) : CombatStrategy(ai) {}
        string getName() override { return "dps debuff"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
