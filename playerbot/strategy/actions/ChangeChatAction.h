#pragma once
#include "GenericActions.h"

namespace ai
{
    class ChangeChatAction : public ChatCommandAction
    {
    public:
        ChangeChatAction(PlayerbotAI* ai) : ChatCommandAction(ai, "chat") {}
        virtual bool Execute(Event& event) override;
    };
}