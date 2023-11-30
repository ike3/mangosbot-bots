#pragma once
#include "../../LootObjectStack.h"
#include "../Value.h"
#include "SubStrategyValue.h"

namespace ai
{
    class MoveStyleValue : public SubStrategyValue
	{
	public:
        MoveStyleValue(PlayerbotAI* ai, string defaultValue = "", string name = "move style", string allowedValues = "wait") : SubStrategyValue(ai, defaultValue, name, allowedValues) {}

        static bool WaitForEnemy(PlayerbotAI* ai) { return HasValue(ai, "wait"); }
        
	private:
        static bool HasValue(PlayerbotAI* ai, string value);
    };
}
