#pragma once
#include "PassTroughStrategy.h"

namespace ai
{
    class LfgStrategy : public PassTroughStrategy
    {
    public:
        LfgStrategy(PlayerbotAI* ai);
		virtual int GetType() { return STRATEGY_TYPE_NONCOMBAT; }
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "lfg"; }
    };
}
