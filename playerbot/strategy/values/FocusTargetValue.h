#pragma once
#include "../Value.h"

namespace ai
{
    class FocusHealTargetValue : public ManualSetValue<ObjectGuid>, public Qualified
	{
	public:
        FocusHealTargetValue(PlayerbotAI* ai) : ManualSetValue<ObjectGuid>(ai, ObjectGuid()), Qualified() {}
    };
}
