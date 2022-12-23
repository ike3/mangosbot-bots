#pragma once
#include "../Value.h"

namespace ai
{
    class ActionPossibleValue : public BoolCalculatedValue, public Qualified
	{
	public:
        ActionPossibleValue(PlayerbotAI* ai, string name = "action possible") : BoolCalculatedValue(ai, name) {}

        virtual bool Calculate();
    };
}
