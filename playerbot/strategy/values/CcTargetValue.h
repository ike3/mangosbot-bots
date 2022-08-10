#pragma once
#include "../Value.h"
#include "TargetValue.h"

namespace ai
{
   
    class CcTargetValue : public TargetValue, public Qualified
	{
	public:
        CcTargetValue(PlayerbotAI* ai, string name = "cc target") : TargetValue(ai, name) {}

    public:
        Unit* Calculate();
    };
}
