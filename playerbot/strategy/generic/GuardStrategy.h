#pragma once
#include "NonCombatStrategy.h"

namespace ai
{
    class GuardStrategy : public NonCombatStrategy
    {
    public:
        GuardStrategy(PlayerbotAI* ai) : NonCombatStrategy(ai) {}
        string getName() override { return "guard"; }

    private:
        NextAction** GetDefaultNonCombatActions() override;
    };
}
