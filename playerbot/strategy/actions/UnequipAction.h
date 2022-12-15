#pragma once
#include "InventoryAction.h"

namespace ai
{
    class UnequipAction : public InventoryAction 
    {
    public:
        UnequipAction(PlayerbotAI* ai) : InventoryAction(ai, "unequip") {}
        virtual bool ExecuteCommand(Event& event) override;

    private:
        void UnequipItem(Item& item);
        void UnequipItem(FindItemVisitor* visitor);
    };
}