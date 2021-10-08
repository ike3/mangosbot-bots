#pragma once
#include "../Value.h"

namespace ai
{
    class RandomBotUpdateValue : public ManualSetValue<bool>
	{
	public:
        RandomBotUpdateValue(PlayerbotAI* ai, string name = "random bot update") : ManualSetValue<bool>(ai, false, name) {}
    };
}
