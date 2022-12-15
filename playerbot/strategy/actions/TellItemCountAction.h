#pragma once
#include "InventoryAction.h"

namespace ai
{
    class TellItemCountAction : public InventoryAction 
    {
    public:
        TellItemCountAction(PlayerbotAI* ai) : InventoryAction(ai, "c") {}
        virtual bool ExecuteCommand(Event& event) override;
    };
}