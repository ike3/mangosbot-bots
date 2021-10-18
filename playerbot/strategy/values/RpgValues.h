#pragma once
#include "../Value.h"

namespace ai
{
    class NextRpgActionValue : public ManualSetValue<string>
	{
	public:
        NextRpgActionValue(PlayerbotAI* ai, string defaultValue = "", string name = "next rpg action") : ManualSetValue(ai, defaultValue, name) {};
    };
}
