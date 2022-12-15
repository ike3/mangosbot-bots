#pragma once
#include "GenericActions.h"

namespace ai
{
	class TeleportAction : public ChatCommandAction
    {
	public:
		TeleportAction(PlayerbotAI* ai) : ChatCommandAction(ai, "teleport") {}
        virtual bool ExecuteCommand(Event& event) override;
    };
}