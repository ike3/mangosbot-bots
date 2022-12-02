#pragma once
#include "GenericShamanStrategy.h"

namespace ai
{
    class ShamanTotemBarElementsStrategy : public GenericShamanStrategy
    {
    public:
        ShamanTotemBarElementsStrategy(PlayerbotAI* ai) : GenericShamanStrategy(ai) {}
        string getName() { return "totembar elements"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers);
    };
}
