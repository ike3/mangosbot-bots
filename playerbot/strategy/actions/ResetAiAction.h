#pragma once
#include "GenericActions.h"

namespace ai
{
    class ResetAiAction : public ChatCommandAction
    {
    public:
        ResetAiAction(PlayerbotAI* ai, bool fullReset = true) : ChatCommandAction(ai, "reset ai"), fullReset(fullReset) {}
        virtual bool ExecuteCommand(Event& event) override;

    protected:
        bool fullReset;
    };
}
