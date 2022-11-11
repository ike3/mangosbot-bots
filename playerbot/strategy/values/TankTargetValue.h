#pragma once
#include "../Value.h"
#include "TargetValue.h"
#include "RtiTargetValue.h"

namespace ai
{
    class TankTargetValue : public RtiTargetValue
	{
	public:
        TankTargetValue(PlayerbotAI* ai, string type = "rti", string name = "tank target") : RtiTargetValue(ai, type, name) {}

    public:
        Unit* Calculate();
    };
}
