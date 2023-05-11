#pragma once
#include "../Value.h"
#include "TargetValue.h"

namespace ai
{
    class SpellCastUsefulValue : public BoolCalculatedValue, public Qualified
	{
	public:
        SpellCastUsefulValue(PlayerbotAI* ai, string name = "spell cast useful") : BoolCalculatedValue(ai, name), Qualified() {}
        virtual bool Calculate();
    };
}
