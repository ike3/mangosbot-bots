#include "botpch.h"
#include "../../playerbot.h"
#include "EquipAction.h"

#include "../values/ItemCountValue.h"
#include "../values/ItemUsageValue.h"

using namespace ai;

bool EquipAction::Execute(Event& event)
{
    string text = event.getParam();
    ItemIds ids = chat->parseItems(text);
    EquipItems(ids);
    return true;
}

void EquipAction::EquipItems(ItemIds ids)
{
    for (ItemIds::iterator i =ids.begin(); i != ids.end(); i++)
    {
        FindItemByIdVisitor visitor(*i);
        EquipItem(&visitor);        
    }
}

void EquipAction::EquipItem(FindItemVisitor* visitor)
{
    ai->InventoryIterateItems(visitor);
    list<Item*> items = visitor->GetResult();
	if (!items.empty()) EquipItem(**items.begin());
}

//Return the bag slot with smallest bag
uint8 EquipAction::GetSmallestBagSlot()
{
    int8 curBag = 0;
    uint32 curSlots = 0;
    for (uint8 bag = INVENTORY_SLOT_BAG_START; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*)bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
        {
            if (curBag > 0 && curSlots < pBag->GetBagSize())
                continue;
            
            curBag = bag;
            curSlots = pBag->GetBagSize();
        }
        else
            return bag;
    }

    return curBag;
}

void EquipAction::EquipItem(Item& item)
{
    uint8 bagIndex = item.GetBagSlot();
    uint8 slot = item.GetSlot();
    uint32 itemId = item.GetProto()->ItemId;

    if (item.GetProto()->InventoryType == INVTYPE_AMMO)
    {
        bot->SetAmmo(itemId);
    }
    else
    {
        bool equipedBag = false;
        if (item.GetProto()->Class == ITEM_CLASS_CONTAINER || item.GetProto()->Class == ITEM_CLASS_QUIVER)
        {
            Bag* pBag = (Bag*)&item;
            uint8 newBagSlot = GetSmallestBagSlot();
            if (newBagSlot > 0)
            {
                uint16 src = ((bagIndex << 8) | slot);

                if (newBagSlot == item.GetBagSlot()) //The new bag is in the slots of the old bag. Move it to the pack first.
                {
                    uint16 dst = ((INVENTORY_SLOT_BAG_0 << 8) | INVENTORY_SLOT_ITEM_START);
                    bot->SwapItem(src, dst);
                    src = dst;
                }

                uint16 dst = ((INVENTORY_SLOT_BAG_0 << 8) | newBagSlot);
                bot->SwapItem(src, dst);
                equipedBag = true;
            }
        }

        if (!equipedBag) 
        {
            WorldPacket packet(CMSG_AUTOEQUIP_ITEM, 2);
            packet << bagIndex << slot;
            bot->GetSession()->HandleAutoEquipItemOpcode(packet);
        }
    }

    sPlayerbotAIConfig.logEvent(ai, "EquipAction", item.GetProto()->Name1, to_string(item.GetProto()->ItemId));

    ostringstream out; out << "equipping " << chat->formatItem(item.GetProto());

    ai->TellMaster(out, PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
}

bool EquipUpgradesAction::Execute(Event& event)
{
    if (!sPlayerbotAIConfig.autoEquipUpgradeLoot && !sRandomPlayerbotMgr.IsRandomBot(bot))
        return false;

    if (event.getSource() == "trade status")
    {
        WorldPacket p(event.getPacket());
        p.rpos(0);
        uint32 status;
        p >> status;

        if (status != TRADE_STATUS_TRADE_ACCEPT)
            return false;
    }


    list<Item*> items;

    FindItemUsageVisitor visitor(bot, ITEM_USAGE_EQUIP);
    ai->InventoryIterateItems(&visitor, ITERATE_ITEMS_IN_BAGS);
    visitor.SetUsage(ITEM_USAGE_REPLACE);
    ai->InventoryIterateItems(&visitor, ITERATE_ITEMS_IN_BAGS);
    visitor.SetUsage(ITEM_USAGE_BAD_EQUIP);
    ai->InventoryIterateItems(&visitor, ITERATE_ITEMS_IN_BAGS);
    items = visitor.GetResult();

    bool didEquip = false;

    for (auto& item : items)
    {
        ItemUsage usage = AI_VALUE2(ItemUsage, "item usage", ItemQualifier(item).GetQualifier());
        if (usage == ITEM_USAGE_EQUIP || usage == ITEM_USAGE_REPLACE || usage == ITEM_USAGE_BAD_EQUIP)
        {
            sLog.outDetail("Bot #%d <%s> auto equips item %d (%s)", bot->GetGUIDLow(), bot->GetName(), item->GetProto()->ItemId, usage == 1 ? "no item in slot" : usage == 2 ? "replace" : usage == 3 ? "wrong item but empty slot" : "");
            EquipItem(*item);   
            didEquip = true;
        }
    }

    return didEquip;
}
