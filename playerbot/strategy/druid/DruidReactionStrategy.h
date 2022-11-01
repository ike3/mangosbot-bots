#pragma once

#include "../generic/ReactionStrategy.h"

namespace ai
{
    class DruidReactionStrategy : public ReactionStrategy
    {
    public:
        DruidReactionStrategy(PlayerbotAI* ai);
        virtual string getName() { return "react"; }

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };
}
