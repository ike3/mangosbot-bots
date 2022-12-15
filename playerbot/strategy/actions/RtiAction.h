#pragma once
#include "GenericActions.h"

namespace ai
{
    class RtiAction : public ChatCommandAction
    {
    public:
        RtiAction(PlayerbotAI* ai) : ChatCommandAction(ai, "rti") {}
        virtual bool Execute(Event& event) override;

    private:
        void AppendRti(ostringstream & out, string type);
    };

    class MarkRtiAction : public Action
    {
    public:
        MarkRtiAction(PlayerbotAI* ai) : Action(ai, "mark rti") {}
        virtual bool Execute(Event& event);
    };
}
