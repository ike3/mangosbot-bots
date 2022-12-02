#pragma once
#include "GenericShamanStrategy.h"

namespace ai
{
    class ShamanTotemBarAncestorsStrategy : public GenericShamanStrategy
    {
    public:
        ShamanTotemBarAncestorsStrategy(PlayerbotAI* ai) : GenericShamanStrategy(ai) {}
        string getName() override { return "totembar ancestors"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
