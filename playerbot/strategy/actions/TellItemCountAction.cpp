#include "botpch.h"
#include "../../playerbot.h"
#include "TellItemCountAction.h"
#include "../values/ItemCountValue.h"

using namespace ai;

bool TellItemCountAction::Execute(Event& event)
{
    Player* requester = event.getOwner() ? event.getOwner() : GetMaster();
    if(requester)
    {
        string text = event.getParam();

        if (text.find("@") == 0)
            return false;

        list<Item*> found = ai->InventoryParseItems(text);
        map<uint32, uint32> itemMap;
        map<uint32, bool> soulbound;
        for (list<Item*>::iterator i = found.begin(); i != found.end(); i++)
        {
            ItemPrototype const* proto = (*i)->GetProto();
            itemMap[proto->ItemId] += (*i)->GetCount();
            soulbound[proto->ItemId] = (*i)->IsSoulBound();
        }

        ai->TellPlayer(requester, "=== Inventory ===");
        for (map<uint32, uint32>::iterator i = itemMap.begin(); i != itemMap.end(); ++i)
        {
            ItemPrototype const* proto = sItemStorage.LookupEntry<ItemPrototype>(i->first);
            ai->InventoryTellItem(requester, proto, i->second, soulbound[i->first]);
        }

        return true;
    }

    return false;
}
