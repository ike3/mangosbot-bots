#pragma once
#include "GenericActions.h"

namespace ai
{
    class TellTargetAction : public ChatCommandAction
    {
    public:
        TellTargetAction(PlayerbotAI* ai) : ChatCommandAction(ai, "tell target") {}
        virtual bool Execute(Event& event) override;
    };

    class TellAttackersAction : public ChatCommandAction
    {
    public:
        TellAttackersAction(PlayerbotAI* ai) : ChatCommandAction(ai, "tell attackers") {}
        virtual bool Execute(Event& event) override;
    };

    class TellPossibleAttackTargetsAction : public ChatCommandAction
    {
    public:
        TellPossibleAttackTargetsAction(PlayerbotAI* ai) : ChatCommandAction(ai, "tell possible attack targets") {}
        virtual bool Execute(Event& event) override;
    };
}
