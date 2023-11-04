#pragma once
#include "GenericActions.h"

namespace ai
{
    class SetFocusHealTargetAction : public ChatCommandAction
    {
    public:
        SetFocusHealTargetAction(PlayerbotAI* ai, string name = "focus heal target") : ChatCommandAction(ai, name) {}
        bool Execute(Event& event) override;
    };

    class SetWaitForAttackTimeAction : public ChatCommandAction
    {
    public:
        SetWaitForAttackTimeAction(PlayerbotAI* ai, string name = "wait for attack time") : ChatCommandAction(ai, name) {}
        bool Execute(Event& event) override;
    };

    class SetFollowTargetAction : public ChatCommandAction
    {
    public:
        SetFollowTargetAction(PlayerbotAI* ai, string name = "follow target") : ChatCommandAction(ai, name) {}
        bool Execute(Event& event) override;
    };
}
