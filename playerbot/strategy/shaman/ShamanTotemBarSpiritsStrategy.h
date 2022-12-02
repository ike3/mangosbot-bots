#pragma once
#include "GenericShamanStrategy.h"

namespace ai
{
    class ShamanTotemBarSpiritsStrategy : public GenericShamanStrategy
    {
    public:
        ShamanTotemBarSpiritsStrategy(PlayerbotAI* ai) : GenericShamanStrategy(ai) {}
        string getName() override { return "totembar spirits"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
