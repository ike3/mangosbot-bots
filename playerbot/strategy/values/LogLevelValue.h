#pragma once
#include "../Value.h"

namespace ai
{
    class LogLevelValue : public ManualSetValue<LogLevel>
	{
	public:
        LogLevelValue(PlayerbotAI* ai, string name = "log level") :
            ManualSetValue<LogLevel>(ai, LOG_LVL_DEBUG, name) {}
	};
}
