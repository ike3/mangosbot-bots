#pragma once
#include "InventoryAction.h"

namespace ai
{
    class WtsAction : public InventoryAction 
    {
    public:
        WtsAction(PlayerbotAI* ai) : InventoryAction(ai, "wts") {}
        virtual bool ExecuteCommand(Event& event) override;
    };
}
