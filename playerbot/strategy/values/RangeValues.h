#pragma once
#include "../Value.h"

namespace ai
{
    class RangeValue : public ManualSetValue<float>, public Qualified
	{
	public:
        RangeValue(PlayerbotAI* ai);
    };
}
