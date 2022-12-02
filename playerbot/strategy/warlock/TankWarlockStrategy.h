#pragma once

#include "GenericWarlockStrategy.h"

namespace ai
{
    class TankWarlockStrategy : public GenericWarlockStrategy
    {
    public:
        TankWarlockStrategy(PlayerbotAI* ai);
        string getName() override { return "tank"; }

    private:
        NextAction** GetDefaultCombatActions() override;
    };
}
