#pragma once
#include "../Value.h"
#include "../ItemVisitors.h"
#include "../actions/InventoryAction.h"

namespace ai
{
    class InventoryItemValueBase : public InventoryAction
    {
    public:
        InventoryItemValueBase(PlayerbotAI* ai) : InventoryAction(ai, "empty") {}
        virtual bool Execute(Event event) { return false; }

    protected:
        list<Item*> Find(string qualifier);
    };

    class ItemCountValue : public Uint32CalculatedValue, public Qualified, InventoryItemValueBase
	{
	public:
        ItemCountValue(PlayerbotAI* ai, string name = "item count") : Uint32CalculatedValue(ai, name), InventoryItemValueBase(ai) {}

    public:
        virtual uint32 Calculate();
	};

    class InventoryItemValue : public CalculatedValue<list<Item*> >, public Qualified, InventoryItemValueBase
    {
    public:
        InventoryItemValue(PlayerbotAI* ai, string name = "inventory items") : CalculatedValue<list<Item*> >(ai, name), InventoryItemValueBase(ai) {}

    public:
        virtual list<Item*> Calculate();
    };

    class EquipedUsableTrinketValue : public CalculatedValue<list<Item*> >, public Qualified, InventoryItemValueBase
    {
    public:
        EquipedUsableTrinketValue(PlayerbotAI* ai) : CalculatedValue<list<Item*> >(ai), InventoryItemValueBase(ai) {}

    public:
        virtual list<Item*> Calculate();
    };
}
