#pragma once

#include "../Action.h"
#include "InventoryAction.h"

namespace ai
{
    class ResetAiAction : public Action {
    public:
        ResetAiAction(PlayerbotAI* ai, bool fullReset = true) : Action(ai, "reset ai"), fullReset(fullReset) {}
        virtual bool Execute(Event event);

    protected:
        bool fullReset;
    };

}
