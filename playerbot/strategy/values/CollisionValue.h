#pragma once
#include "../Value.h"

namespace ai
{
    class CollisionValue : public BoolCalculatedValue, public Qualified
	{
	public:
        CollisionValue(PlayerbotAI* ai, string name = "collision") : BoolCalculatedValue(ai, name), Qualified() {}

        virtual bool Calculate();
    };
}
