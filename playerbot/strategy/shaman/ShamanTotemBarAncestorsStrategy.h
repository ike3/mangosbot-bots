#pragma once

#include "GenericShamanStrategy.h"

namespace ai
{
    class ShamanTotemBarAncestorsStrategy : public GenericShamanStrategy
    {
    public:
        ShamanTotemBarAncestorsStrategy(PlayerbotAI* ai);

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "totembar ancestors"; }
    };
}
