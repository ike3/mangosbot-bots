#include "botpch.h"
#include "../../playerbot.h"
#include "UnequipAction.h"

#include "../values/ItemCountValue.h"

using namespace ai;

vector<string> split(const string &s, char delim);

bool UnequipAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    string text = event.getParam();
    list<Item*> found = ai->InventoryParseItems(text, IterateItemsMask::ITERATE_ITEMS_IN_EQUIP);
    for (auto& item : found)
    {
        UnequipItem(requester, item);
    }

    return true;
}

void UnequipAction::UnequipItem(Player* requester, FindItemVisitor* visitor)
{
    IterateItemsMask mask = IterateItemsMask((uint8)IterateItemsMask::ITERATE_ITEMS_IN_BAGS | (uint8)IterateItemsMask::ITERATE_ITEMS_IN_EQUIP);
    ai->InventoryIterateItems(visitor, mask);
    list<Item*> items = visitor->GetResult();
	if (!items.empty()) UnequipItem(requester, *items.begin());
}

void UnequipAction::UnequipItem(Player* requester, Item* item)
{
    uint8 bagIndex = item->GetBagSlot();
    uint8 slot = item->GetSlot();
    uint8 dstBag = NULL_BAG;

    WorldPacket packet(CMSG_AUTOSTORE_BAG_ITEM, 3);
    packet << bagIndex << slot << dstBag;
    bot->GetSession()->HandleAutoStoreBagItemOpcode(packet);

    ostringstream out; out << chat->formatItem(item) << " unequipped";

    ai->TellPlayer(requester, out, PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
}