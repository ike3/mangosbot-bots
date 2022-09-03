#include "botpch.h"
#include "../../playerbot.h"
#include "AhAction.h"
#include "../../../ahbot/AhBot.h"
#include "../values/ItemCountValue.h"
#include "../../RandomItemMgr.h"
#include "../values/BudgetValues.h"

using namespace std;
using namespace ai;



bool AhAction::Execute(Event event)
{
    string text = event.getParam();

    list<ObjectGuid> npcs = AI_VALUE(list<ObjectGuid>, "nearest npcs");
    for (list<ObjectGuid>::iterator i = npcs.begin(); i != npcs.end(); i++)
    {
        Unit* npc = bot->GetNPCIfCanInteractWith(*i, UNIT_NPC_FLAG_AUCTIONEER);
        if (!npc)
            continue;
    
        if (!sRandomPlayerbotMgr.m_ahActionMutex.try_lock()) //Another bot is using the Auction right now. Try again later.
            return false;

        bool doneAuction = Execute(text, npc);

        sRandomPlayerbotMgr.m_ahActionMutex.unlock();
        
        return doneAuction;
    }

    ai->TellError("Cannot find auctioneer nearby");
    return false;
}

bool AhAction::Execute(string text, Unit* auctioneer)
{
    uint32 time;
#ifdef MANGOSBOT_ZERO
    time = 8 * HOUR / MINUTE;
#endif
#ifdef MANGOSBOT_ONE
    time = 12 * HOUR / MINUTE;
#endif

    if (text == "vendor")
    {
        AuctionHouseEntry const* auctionHouseEntry = bot->GetSession()->GetCheckedAuctionHouseForAuctioneer(auctioneer->GetObjectGuid());
        if (!auctionHouseEntry)
            return false;

        list<Item*> items = AI_VALUE2(list<Item*>, "inventory items", "usage " + to_string(ITEM_USAGE_AH));
        
        bool postedItem = false;

        for (auto item : items)
        {
            uint32 deposit = AuctionHouseMgr::GetAuctionDeposit(auctionHouseEntry, time, item);

            uint32 freeMoney = AI_VALUE2(uint32, "free money for", (uint32)NeedMoneyFor::ah);

            if (deposit > freeMoney)
                return false;

            ItemPrototype const* proto = item->GetProto();

            uint32 price = item->GetCount() * auctionbot.GetSellPrice(proto);

            postedItem |= PostItem(item, price, auctioneer, time);

            if (!urand(0, 5))
                break;
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

    return PostItem(item, price, auctioneer, time);
}

bool AhAction::PostItem(Item* item, uint32 price, Unit* auctioneer, uint32 time)
{
    ObjectGuid itemGuid = item->GetObjectGuid();

    WorldPacket packet;
    packet << auctioneer->GetObjectGuid();
    packet << itemGuid;
    packet << price * 95 / 100;
    packet << price;
    packet << time;

    bot->GetSession()->HandleAuctionSellItem(packet);

    if (bot->GetItemByGuid(itemGuid))
        return false;

    ostringstream out;
    out << "Posting " << ChatHelper::formatItem(item->GetProto(), item->GetCount()) << " for " << ChatHelper::formatMoney(price) << " to the AH";
    ai->TellMasterNoFacing(out.str());
    return true;
}


bool AhBidAction::Execute(string text, Unit* auctioneer)
{
    AuctionHouseEntry const* auctionHouseEntry = bot->GetSession()->GetCheckedAuctionHouseForAuctioneer(auctioneer->GetObjectGuid());
    if (!auctionHouseEntry)
        return false;

    // always return pointer
    AuctionHouseObject* auctionHouse = sAuctionMgr.GetAuctionsMap(auctionHouseEntry);

    if (!auctionHouse)
        return false;

    AuctionHouseObject::AuctionEntryMap const& map = auctionHouse->GetAuctions();

    AuctionEntry* auction = nullptr;

    vector<pair<AuctionEntry*, uint32>> auctionPowers;

    if (text == "vendor")
    {
        unordered_map <ItemUsage, int32> freeMoney;

        freeMoney[ITEM_USAGE_EQUIP] = freeMoney[ITEM_USAGE_REPLACE] = freeMoney[ITEM_USAGE_BAD_EQUIP] = AI_VALUE2(uint32, "free money for", (uint32)NeedMoneyFor::gear);
        freeMoney[ITEM_USAGE_USE] = AI_VALUE2(uint32, "free money for", (uint32)NeedMoneyFor::consumables);
        freeMoney[ITEM_USAGE_SKILL] = freeMoney[ITEM_USAGE_DISENCHANT] = AI_VALUE2(uint32, "free money for", (uint32)NeedMoneyFor::tradeskill);
        freeMoney[ITEM_USAGE_AMMO] = AI_VALUE2(uint32, "free money for", (uint32)NeedMoneyFor::ammo);
        freeMoney[ITEM_USAGE_QUEST] = freeMoney[ITEM_USAGE_AH] = freeMoney[ITEM_USAGE_VENDOR] = AI_VALUE2(uint32, "free money for", (uint32)NeedMoneyFor::anything);

        uint32 checkNumAuctions = urand(50, 250);

        for (uint32 i = 0; i < checkNumAuctions; i++)
        {
            auto curAuction = std::next(std::begin(map), urand(0, map.size()));

            auction = curAuction->second;

            if (!auction)
                continue;

            if (auction->owner == bot->GetGUIDLow())
                continue;

            int32 cost = std::min(auction->buyout, uint32(std::max(auction->bid, auction->startbid) * frand(1.05f, 1.25f)));

            ItemUsage usage = AI_VALUE2(ItemUsage, "item usage", auction->itemTemplate);

            if (freeMoney.find(usage) == freeMoney.end() || cost > freeMoney[usage])
                continue;

            uint32 power = 1;
                
            switch (usage)
            {
            case ITEM_USAGE_EQUIP:
            case ITEM_USAGE_REPLACE:
            case ITEM_USAGE_BAD_EQUIP:
                power = sRandomItemMgr.GetLiveStatWeight(bot, auction->itemTemplate);
                break;
            case ITEM_USAGE_AH:
                if (cost >= auctionbot.GetSellPrice(sObjectMgr.GetItemPrototype(auction->itemTemplate)))
                    continue;
                power = 1000;
                break;
            case ITEM_USAGE_VENDOR:
                if (cost >= (int32)sObjectMgr.GetItemPrototype(auction->itemTemplate)->SellPrice)
                    continue;
                power = 1000;
                break;
            }

            power *= 1000;
            power /= cost;

            auctionPowers.push_back(make_pair(auction, power));
        }

        std::sort(auctionPowers.begin(), auctionPowers.end(), [](std::pair<AuctionEntry*, uint32> i, std::pair<AuctionEntry*, uint32> j) {return i > j; });

        bool bidItems = false;

        for (auto auctionPower : auctionPowers)
        {
            auction = auctionPower.first;

            if (!auction)
                continue;

            ItemUsage usage = AI_VALUE2(ItemUsage, "item usage", auction->itemTemplate);

            int32 price = std::min(auction->buyout, uint32(std::max(auction->bid, auction->startbid) * frand(1.05f, 1.25f)));

            if (freeMoney.find(usage) == freeMoney.end() || price > freeMoney[usage])
                if (!urand(0, 5))
                    break;
                else
                continue;

            bidItems |= BidItem(auction, price, auctioneer);

            if (!urand(0, 5))
                break;

            freeMoney[ITEM_USAGE_EQUIP] = freeMoney[ITEM_USAGE_REPLACE] = freeMoney[ITEM_USAGE_BAD_EQUIP] = AI_VALUE2(uint32, "free money for", (uint32)NeedMoneyFor::gear);
            freeMoney[ITEM_USAGE_USE] = AI_VALUE2(uint32, "free money for", (uint32)NeedMoneyFor::consumables);
            freeMoney[ITEM_USAGE_SKILL] = freeMoney[ITEM_USAGE_DISENCHANT] = AI_VALUE2(uint32, "free money for", (uint32)NeedMoneyFor::tradeskill);
            freeMoney[ITEM_USAGE_AMMO] = AI_VALUE2(uint32, "free money for", (uint32)NeedMoneyFor::ammo);
            freeMoney[ITEM_USAGE_QUEST] = freeMoney[ITEM_USAGE_AH] = freeMoney[ITEM_USAGE_VENDOR] = AI_VALUE2(uint32, "free money for", (uint32)NeedMoneyFor::anything);
        }

        return bidItems;
    }

    int pos = text.find(" ");
    if (pos == string::npos) return false;

    string priceStr = text.substr(0, pos);
    uint32 price = ChatHelper::parseMoney(priceStr);

    for (auto curAuction : map)
    {
        auction = curAuction.second;

        if (auction->owner == bot->GetGUIDLow())
            continue;

        ItemPrototype const* proto = sObjectMgr.GetItemPrototype(auction->itemTemplate);

        if (!proto)
            continue;

        if(!proto->Name1)
            continue;
        
        if (!strstri(proto->Name1, text.c_str()))
            continue;

        if (price && auction->bid + 5 > price)
            continue;

        uint32 cost = std::min(auction->buyout, uint32(std::max(auction->bid, auction->startbid) * frand(1.05f, 1.25f)));

        uint32 power = auction->itemCount;
        power *= 1000;
        power /= cost;

        auctionPowers.push_back(make_pair(auction, power));
    }

    if (auctionPowers.empty())
        return false;

    std::sort(auctionPowers.begin(), auctionPowers.end(), [](std::pair<AuctionEntry*, uint32> i, std::pair<AuctionEntry*, uint32> j) {return i > j; });

    auction = auctionPowers.begin()->first;

    uint32 cost = std::min(auction->buyout, uint32(std::max(auction->bid, auction->startbid) * frand(1.05f, 1.25f)));

    return BidItem(auction, price, auctioneer);
}

bool AhBidAction::BidItem(AuctionEntry* auction, uint32 price, Unit* auctioneer)
{
    WorldPacket packet;
    packet << auctioneer->GetObjectGuid();
    packet << auction->Id;
    packet << price;

    uint32 oldMoney = bot->GetMoney();

    bot->GetSession()->HandleAuctionPlaceBid(packet);

    if (bot->GetMoney() < oldMoney)
    {
        ItemPrototype const* proto = sObjectMgr.GetItemPrototype(auction->itemTemplate);
        ostringstream out;
        out << "Bidding " << ChatHelper::formatMoney(price) << " on " << ChatHelper::formatItem(proto, auction->itemCount) << " on the AH";
        ai->TellMasterNoFacing(out.str());
        return true;
    }
    return false;
}