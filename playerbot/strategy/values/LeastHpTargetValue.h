#pragma once
#include "../Value.h"
#include "TargetValue.h"

namespace ai
{
    class LeastHpTargetValue : public TargetValue
	{
	public:
        LeastHpTargetValue(PlayerbotAI* ai, string name = "least hp target") : TargetValue(ai, name) {}

    public:
        Unit* Calculate();
    };
}
