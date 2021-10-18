#pragma once
#include "../Value.h"
#include "TargetValue.h"

namespace ai
{
   
    class TankTargetValue : public TargetValue
	{
	public:
        TankTargetValue(PlayerbotAI* ai, string name = "tank target") : TargetValue(ai, name) {}

    public:
        Unit* Calculate();
    };
}
