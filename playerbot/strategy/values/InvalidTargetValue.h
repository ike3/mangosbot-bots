#pragma once
#include "../Value.h"

namespace ai
{
    class InvalidTargetValue : public BoolCalculatedValue, public Qualified
	{
	public:
        InvalidTargetValue(PlayerbotAI* ai, string name = "invalid target") : BoolCalculatedValue(ai, name) {}

	public:
        virtual bool Calculate();
    };
}
