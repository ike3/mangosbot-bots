#pragma once
#include "../Value.h"
#include "TargetValue.h"

namespace ai
{
    class DuelTargetValue : public TargetValue
	{
	public:
        DuelTargetValue(PlayerbotAI* ai, string name = "duel target") : TargetValue(ai, name) {}

    public:
        Unit* Calculate();
    };
}
