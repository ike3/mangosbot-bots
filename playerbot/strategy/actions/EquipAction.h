#pragma once

#include "../Action.h"
#include "InventoryAction.h"

namespace ai
{
    class EquipAction : public InventoryAction {
    public:
        EquipAction(PlayerbotAI* ai, string name = "equip") : InventoryAction(ai, name) {}
        virtual bool Execute(Event event);
        void EquipItems(ItemIds ids);

    private:
        void EquipItem(FindItemVisitor* visitor);
        void EquipItem(Item& item);
    };

}
