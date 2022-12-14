#pragma once
#include "GenericActions.h"
#include "../ItemVisitors.h"

namespace ai
{
    class InventoryAction : public ChatCommandAction 
    {
    public:
        InventoryAction(PlayerbotAI* ai, string name) : ChatCommandAction(ai, name) {}

    protected:
        virtual bool ExecuteCommand(Event& event) override { return false; }
        void IterateItems(IterateItemsVisitor* visitor, IterateItemsMask mask = ITERATE_ITEMS_IN_BAGS);
        void TellItems(map<uint32, int> items, map<uint32, bool> soulbound);
        void TellItem(ItemPrototype const * proto, int count, bool soulbound);
        list<Item*> parseItems(string text, IterateItemsMask mask = ITERATE_ALL_ITEMS);
        uint32 GetItemCount(FindItemVisitor* visitor, IterateItemsMask mask = ITERATE_ITEMS_IN_BAGS);
        string parseOutfitName(string outfit);
        ItemIds parseOutfitItems(string outfit);
        ItemIds FindOutfitItems(string name);

    private:
        void IterateItemsInBags(IterateItemsVisitor* visitor);
        void IterateItemsInEquip(IterateItemsVisitor* visitor);
        void IterateItemsInBank(IterateItemsVisitor* visitor);
    };
}
