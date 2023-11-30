#pragma once
#include "../../LootObjectStack.h"
#include "GenericActions.h"

namespace ai
{
    class MoveStyleAction : public ChatCommandAction
    {
    public:
        MoveStyleAction(PlayerbotAI* ai) : ChatCommandAction(ai, "move style") {}
        virtual bool Execute(Event& event) override;
    };
}
