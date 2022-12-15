#pragma once
#include "GenericActions.h"

namespace ai
{
    class WtsAction : public ChatCommandAction 
    {
    public:
        WtsAction(PlayerbotAI* ai) : ChatCommandAction(ai, "wts") {}
        virtual bool ExecuteCommand(Event& event) override;
    };
}
