#pragma once
#include "../Value.h"
#include "TargetValue.h"

namespace ai
{

    class DefenseTargetValue : public TargetValue
	{
	public:
        DefenseTargetValue(PlayerbotAI* ai) : TargetValue(ai) {}

    public:
        Unit* Calculate();
    };
}
