#pragma once
#include "../generic/MeleeCombatStrategy.h"

namespace ai
{
    class GenericDKStrategy : public MeleeCombatStrategy
    {
    public:
        GenericDKStrategy(PlayerbotAI* ai);
        virtual string getName() override { return "DK"; }

    protected:
        virtual void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
