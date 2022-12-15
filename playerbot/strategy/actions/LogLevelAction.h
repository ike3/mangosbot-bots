#pragma once
#include "GenericActions.h"

namespace ai
{
    class LogLevelAction : public ChatCommandAction
    {
    public:
        LogLevelAction(PlayerbotAI* ai) : ChatCommandAction(ai, "log") {}
        virtual bool Execute(Event& event) override;

    public:
        static string logLevel2string(LogLevel level);
        static LogLevel string2logLevel(string level);
    };
}