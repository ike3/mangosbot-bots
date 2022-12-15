#pragma once
#include "../Value.h"
#include "../ItemVisitors.h"

namespace ai
{
    class ItemCountValue : public Uint32CalculatedValue, public Qualified
	{
	public:
        ItemCountValue(PlayerbotAI* ai, string name = "item count") : Uint32CalculatedValue(ai, name) {}
        virtual uint32 Calculate();
	};

    class InventoryItemValue : public CalculatedValue<list<Item*> >, public Qualified
    {
    public:
        InventoryItemValue(PlayerbotAI* ai, string name = "inventory items") : CalculatedValue<list<Item*> >(ai, name) {}
        virtual list<Item*> Calculate();
    };

    class InventoryItemIdValue : public CalculatedValue<list<uint32> >, public Qualified
    {
    public:
        InventoryItemIdValue(PlayerbotAI* ai, string name = "inventory item ids") : CalculatedValue<list<uint32> >(ai, name) {}
        virtual list<uint32> Calculate() {list<uint32> retVal;  for (auto& item : AI_VALUE2(list<Item*>, "inventory items", getQualifier())) { ItemPrototype const* proto = item->GetProto();  retVal.push_back(proto->ItemId);} return retVal;};
    };

    class EquipedUsableTrinketValue : public CalculatedValue<list<Item*> >, public Qualified
    {
    public:
        EquipedUsableTrinketValue(PlayerbotAI* ai) : CalculatedValue<list<Item*> >(ai) {}
        virtual list<Item*> Calculate();
    };
}
