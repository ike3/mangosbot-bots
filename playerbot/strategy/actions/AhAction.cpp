#include "botpch.h"
#include "../../playerbot.h"
#include "AhAction.h"
#include "../../../ahbot/AhBot.h"
#include "../values/ItemCountValue.h"

using namespace std;
using namespace ai;

bool AhAction::Execute(Event event)
{
    string text = event.getParam();

    list<ObjectGuid> npcs = AI_VALUE(list<ObjectGuid>, "nearest npcs");
    for (list<ObjectGuid>::iterator i = npcs.begin(); i != npcs.end(); i++)
    {
        Unit* npc = ai->GetUnit(*i);
        if (!npc || !npc->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_AUCTIONEER))
            continue;

        return Execute(text, npc);
    }

    ai->TellError("Cannot find auctioneer nearby");
    return false;
}

bool AhAction::Execute(string text, Unit* auctioneer)
{
    if (text == "vendor")
    {
        list<Item*> items = AI_VALUE2(list<Item*>, "inventory items", "usage " + to_string(ITEM_USAGE_AH));
        
        bool postedItem = false;

        for (auto item : items)
        {
            ItemPrototype const* proto = item->GetProto();
            uint32 price = item->GetCount() * auctionbot.GetSellPrice(proto);

            postedItem |= PostItem(item, price, auctioneer);
        }

        return postedItem;
    }

    int pos = text.find(" ");
    if (pos == string::npos) return false;

    string priceStr = text.substr(0, pos);
    uint32 price = ChatHelper::parseMoney(priceStr);

    list<Item*> found = parseItems(text, ITERATE_ITEMS_IN_BAGS);
    if (found.empty())
        return false;

    Item* item = *found.begin();

    return PostItem(item, price, auctioneer);
}

bool AhAction::PostItem(Item* item, uint32 price, Unit* auctioneer)
{
    WorldPacket packet;
    packet << auctioneer->GetObjectGuid();
    packet << item->GetObjectGuid();
    packet << price * 95 / 100;
    packet << price;
#ifdef MANGOSBOT_ZERO
    packet << 8 * HOUR / MINUTE;
#endif
#ifdef MANGOSBOT_ONE
    packet << 12 * HOUR / MINUTE;
#endif

    bot->GetSession()->HandleAuctionSellItem(packet);

    ostringstream out;
    out << "Posting " << ChatHelper::formatItem(item->GetProto(), item->GetCount()) << " for " << ChatHelper::formatMoney(price) << " to the AH";
    ai->TellMaster(out.str());
    return true;
}
