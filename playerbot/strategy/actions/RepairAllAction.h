#pragma once
#include "GenericActions.h"

namespace ai
{
    class RepairAllAction : public ChatCommandAction
    {
    public:
        RepairAllAction(PlayerbotAI* ai) : ChatCommandAction(ai, "repair") {}
        virtual bool ExecuteCommand(Event& event) override;
    };
}