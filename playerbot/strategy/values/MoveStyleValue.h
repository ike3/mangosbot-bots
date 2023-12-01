#pragma once
#include "../../LootObjectStack.h"
#include "../Value.h"
#include "SubStrategyValue.h"

namespace ai
{
    class MoveStyleValue : public SubStrategyValue
	{
	public:
        MoveStyleValue(PlayerbotAI* ai, string defaultValue = "", string name = "move style", string allowedValues = "wait,noedge") : SubStrategyValue(ai, defaultValue, name, allowedValues) {}

        static bool WaitForEnemy(PlayerbotAI* ai) { return HasValue(ai, "wait"); }
        static bool CheckForEdges(PlayerbotAI* ai) { return HasValue(ai, "noedge"); }
        
	private:
        static bool HasValue(PlayerbotAI* ai, string value);
    };
}
