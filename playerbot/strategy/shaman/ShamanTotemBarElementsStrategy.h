#pragma once

#include "GenericShamanStrategy.h"

namespace ai
{
    class ShamanTotemBarElementsStrategy : public GenericShamanStrategy
    {
    public:
        ShamanTotemBarElementsStrategy(PlayerbotAI* ai);

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "totembar elements"; }
    };
}
