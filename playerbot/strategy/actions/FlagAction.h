#pragma once
#include "../../LootObjectStack.h"
#include "GenericActions.h"

namespace ai
{
    class FlagAction : public ChatCommandAction
    {
    public:
        FlagAction(PlayerbotAI* ai) : ChatCommandAction(ai, "flag") {}
        virtual bool ExecuteCommand(Event& event) override;

    private:
        bool TellUsage();
    };
}
