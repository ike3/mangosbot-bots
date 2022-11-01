#pragma once

#include "GenericShamanStrategy.h"

namespace ai
{
    class ShamanTotemBarSpiritsStrategy : public GenericShamanStrategy
    {
    public:
        ShamanTotemBarSpiritsStrategy(PlayerbotAI* ai);

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "totembar spirits"; }
    };
}
