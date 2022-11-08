#pragma once
#include "../Value.h"

namespace ai
{
    class WaitForAttackTimeValue : public ManualSetValue<uint8>, public Qualified
	{
	public:
        WaitForAttackTimeValue(PlayerbotAI* ai) : ManualSetValue<uint8>(ai, 10), Qualified() {}
    };
}
