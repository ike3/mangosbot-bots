#include "botpch.h"
#include "../../playerbot.h"
#include "BuyAction.h"
#include "../ItemVisitors.h"
#include "../values/ItemCountValue.h"
#include "../values/ItemUsageValue.h"
#include "../values/BudgetValues.h"

using namespace ai;

bool BuyAction::Execute(Event event)
{
    bool buyUseful = false;
    ItemIds itemIds;
    string link = event.getParam();

    if (link == "vendor")
        buyUseful = true;
    else
    {
        itemIds = chat->parseItems(link);

        Player* master = GetMaster();

        if (!master)
            return false;
    }

    list<ObjectGuid> vendors = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest npcs")->Get();
    bool vendored = false, result = false;
    for (list<ObjectGuid>::iterator i = vendors.begin(); i != vendors.end(); ++i)
    {
        ObjectGuid vendorguid = *i;
        Creature *pCreature = bot->GetNPCIfCanInteractWith(vendorguid, UNIT_NPC_FLAG_VENDOR);
        if (!pCreature)
            continue;

        vendored = true;

        if (buyUseful)
        {
            //Items are evaluated from high-level to low level.
            //For each item the bot checks again if an item is usefull.
            //Bot will buy until no usefull items are left.

            VendorItemData const* tItems = pCreature->GetVendorItems();
            VendorItemData const* vItems = {};
#ifndef MANGOSBOT_ZERO                
            vItems = pCreature->GetVendorTemplateItems();
#endif
            if (!tItems && !vItems)
                continue;
            
            VendorItemList m_items_sorted;
            
            if (tItems)
                m_items_sorted.insert(m_items_sorted.begin(), tItems->m_items.begin(), tItems->m_items.end());
            if (vItems)
                m_items_sorted.insert(m_items_sorted.begin(), vItems->m_items.begin(), vItems->m_items.end());
            

            std::remove_if(m_items_sorted.begin(), m_items_sorted.end(), [](VendorItem* i) {ItemPrototype const* proto = sObjectMgr.GetItemPrototype(i->item); return !proto; });

            if (m_items_sorted.empty())
                continue;

            std::sort(m_items_sorted.begin(), m_items_sorted.end(), [](VendorItem* i, VendorItem* j) {return sObjectMgr.GetItemPrototype(i->item)->ItemLevel > sObjectMgr.GetItemPrototype(j->item)->ItemLevel; });

            for (auto& tItem : m_items_sorted)
            {
                for (uint32 i=0; i<10; i++) //Buy 10 times or until no longer usefull/possible
                {
                    ItemUsage usage = AI_VALUE2(ItemUsage, "item usage", tItem->item);
                    ItemPrototype const* proto = sObjectMgr.GetItemPrototype(tItem->item);

                    uint32 price = proto->BuyPrice;

                    // reputation discount
                    price = uint32(floor(price * bot->GetReputationPriceDiscount(pCreature)));

                    NeedMoneyFor needMoneyFor = NeedMoneyFor::none;

                    switch (usage)
                    {
                    case ITEM_USAGE_REPLACE:
                    case ITEM_USAGE_EQUIP:
                        needMoneyFor = NeedMoneyFor::gear;
                        break;
                    case ITEM_USAGE_AMMO:
                        needMoneyFor = NeedMoneyFor::ammo;
                        break;
                    case ITEM_USAGE_QUEST:
                        needMoneyFor = NeedMoneyFor::anything;
                        break;
                    case ITEM_USAGE_USE:
                        needMoneyFor = NeedMoneyFor::consumables;
                        break;
                    }

                    if (needMoneyFor == NeedMoneyFor::none)
                        break;

                    if (AI_VALUE2(uint32, "free money for", uint32(needMoneyFor)) < price)
                        break;

                    if (!BuyItem(tItems, vendorguid, proto))
#ifndef MANGOSBOT_ZERO
                        if(!BuyItem(vItems, vendorguid, proto))
#endif
                        break;    

                    if (usage == ITEM_USAGE_REPLACE || usage == ITEM_USAGE_EQUIP) //Equip upgrades and stop buying this time.
                    {
                        ai->DoSpecificAction("equip upgrades");
                        break;
                    }
                } 
            }
        }
        else
        {
            if (itemIds.empty())
                return false;

            for (ItemIds::iterator i = itemIds.begin(); i != itemIds.end(); i++)
            {
                uint32 itemId = *i;
                const ItemPrototype* proto = sObjectMgr.GetItemPrototype(itemId);
                if (!proto)
                    continue;

                result |= BuyItem(pCreature->GetVendorItems(), vendorguid, proto);
#ifndef MANGOSBOT_ZERO
                result |= BuyItem(pCreature->GetVendorTemplateItems(), vendorguid, proto);
#endif

                if (!result)
                {
                    ostringstream out; out << "Nobody sells " << ChatHelper::formatItem(proto) << " nearby";
                    ai->TellMaster(out.str());
                }
            }
        }
    }

    if (!vendored)
    {
        ai->TellError("There are no vendors nearby");
        return false;
    }

    return true;
}

bool BuyAction::BuyItem(VendorItemData const* tItems, ObjectGuid vendorguid, const ItemPrototype* proto)
{
    uint32 oldCount = AI_VALUE2(uint32, "item count", proto->Name1);

    if (!tItems)
        return false;

    uint32 itemId = proto->ItemId;
    for (uint32 slot = 0; slot < tItems->GetItemCount(); slot++)
    {
        if (tItems->GetItem(slot)->item == itemId)
        {       
            uint32 botMoney = bot->GetMoney();
            if (ai->HasCheat(BotCheatMask::gold))
            {
                bot->SetMoney(10000000);
            }

#ifdef MANGOSBOT_TWO
            bot->BuyItemFromVendorSlot(vendorguid, slot, itemId, 1, NULL_BAG, NULL_SLOT);
#else
            bot->BuyItemFromVendor(vendorguid, itemId, 1, NULL_BAG, NULL_SLOT);
#endif
            if (ai->HasCheat(BotCheatMask::gold))
            {
                bot->SetMoney(botMoney);
            }

            if (oldCount < AI_VALUE2(uint32, "item count", proto->Name1)) //BuyItem Always returns false (unless unique) so we have to check the item counts.
            {
                ostringstream out; out << "Buying " << ChatHelper::formatItem(proto);
                ai->TellMaster(out.str());
                return true;
            }
 
            return false;
        }
    }

    return false;
}
