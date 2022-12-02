#pragma once

#include "GenericHunterStrategy.h"
#include "../generic/CombatStrategy.h"

namespace ai
{
    class DpsHunterStrategy : public GenericHunterStrategy
    {
    public:
        DpsHunterStrategy(PlayerbotAI* ai);
        virtual string getName() override { return "dps"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
        NextAction** GetDefaultCombatActions() override;
    };

    class DpsAoeHunterStrategy : public CombatStrategy
    {
    public:
        DpsAoeHunterStrategy(PlayerbotAI* ai) : CombatStrategy(ai) {}
        string getName() override { return "aoe"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };

    class DpsHunterDebuffStrategy : public CombatStrategy
    {
    public:
        DpsHunterDebuffStrategy(PlayerbotAI* ai) : CombatStrategy(ai) {}
        string getName() override { return "dps debuff"; }

    private:
        virtual void InitCombatTriggers(std::list<TriggerNode*> &triggers);
    };
}
