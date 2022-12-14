#pragma once
#include "GenericActions.h"

namespace ai
{
    class ShareQuestAction : public ChatCommandAction
    {
    public:
        ShareQuestAction(PlayerbotAI* ai) : ChatCommandAction(ai, "share quest") {}
        virtual bool ExecuteCommand(Event& event) override;
    };
}
