#pragma once
#include "GenericActions.h"

namespace ai
{
    class EquipAction : public ChatCommandAction
    {
    public:
        EquipAction(PlayerbotAI* ai, string name = "equip") : ChatCommandAction(ai, name) {}
        virtual bool Execute(Event& event) override;
        void EquipItems(ItemIds ids);

    private:
        void EquipItem(FindItemVisitor* visitor);
        uint8 GetSmallestBagSlot();
        void EquipItem(Item& item);
    };

    class EquipUpgradesAction : public EquipAction
    {
    public:
        EquipUpgradesAction(PlayerbotAI* ai, string name = "equip upgrades") : EquipAction(ai, name) {}
        virtual bool Execute(Event& event) override;
    };
}
