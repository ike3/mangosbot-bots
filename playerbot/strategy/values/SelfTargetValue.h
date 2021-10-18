#pragma once
#include "../Value.h"

namespace ai
{
    class SelfTargetValue : public UnitCalculatedValue
	{
	public:
        SelfTargetValue(PlayerbotAI* ai, string name = "self target") : UnitCalculatedValue(ai, name) {}

        virtual Unit* Calculate() { return ai->GetBot(); }
    };
}
