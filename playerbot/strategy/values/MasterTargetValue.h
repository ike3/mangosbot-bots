#pragma once
#include "../Value.h"

namespace ai
{
    class MasterTargetValue : public UnitCalculatedValue
	{
	public:
        MasterTargetValue(PlayerbotAI* ai, string name = "master target") : UnitCalculatedValue(ai, name) {}

        virtual Unit* Calculate() { return ai->GetGroupMaster(); }
    };
}
