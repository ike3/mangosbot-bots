#pragma once

#include "../Action.h"
#include "InventoryAction.h"

namespace ai
{
    class SellAction : public InventoryAction {
    public:
        SellAction(PlayerbotAI* ai, string name = "sell") : InventoryAction(ai, name) {}
        virtual bool Execute(Event& event);

        void Sell(FindItemVisitor* visitor);
        void Sell(Item* item);

    };
}