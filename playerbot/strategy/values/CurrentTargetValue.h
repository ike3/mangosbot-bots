#pragma once
#include "../Value.h"

namespace ai
{
    class CurrentTargetValue : public UnitManualSetValue
	{
	public:
        CurrentTargetValue(PlayerbotAI* ai, string name = "current target") : UnitManualSetValue(ai, NULL, name) {}

        virtual Unit* Get();
        virtual void Set(Unit* unit);

    private:
        ObjectGuid selection;
    };
}
