#pragma once
#include "GenericActions.h"

namespace ai
{
    class FocusHealSetTargetAction : public ChatCommandAction
    {
    public:
        FocusHealSetTargetAction(PlayerbotAI* ai, string name = "focus heal target") : ChatCommandAction(ai, name) {}
        virtual bool Execute(Event& event) override;
    };
}
