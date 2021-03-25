#include "botpch.h"
#include "../../playerbot.h"
#include "BuyAction.h"
#include "../ItemVisitors.h"
#include "../values/ItemCountValue.h"
#include "../values/ItemUsageValue.h"

using namespace ai;

bool BuyAction::Execute(Event event)
{
    bool buyUsefull = false;
    ItemIds itemIds;
    string link = event.getParam();

    if (link == "vendor")
        buyUsefull = true;
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

        if (buyUsefull)
        {
            VendorItemData const* tItems = pCreature->GetVendorItems();

            if (!tItems)
                continue;

            for (auto& tItem : tItems->m_items)
            {
                ItemUsage usage = AI_VALUE2(ItemUsage, "item usage", tItem->item);
                if (usage == ITEM_USAGE_REPLACE || usage == ITEM_USAGE_EQUIP || usage == ITEM_USAGE_AMMO)
                    itemIds.insert(tItem->item);
                else
                {
                    ItemPrototype const* proto = sObjectMgr.GetItemPrototype(tItem->item);
                    if (!proto)
                        continue;

                    //temp needs to move to itemusage value
                    for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
                    {
                        uint32 entry = ai->GetBot()->GetQuestSlotQuestId(slot);
                        Quest const* quest = sObjectMgr.GetQuestTemplate(entry);
                        if (!quest)
                            continue;

                        for (int i = 0; i < 4; i++)
                        {
                            if (quest->ReqItemId[i] == tItem->item)
                            {
                                if (!ai->GetMaster() || !sPlayerbotAIConfig.syncQuestWithPlayer)
                                    if (AI_VALUE2(uint8, "item count", proto->Name1) < quest->ReqItemCount[i])
                                        itemIds.insert(tItem->item);
                            }
                        }
                    }
                }

            }

            if (itemIds.empty())
                return true;
        }

        if (itemIds.empty())
            return false;

        vendored = true;

        for (ItemIds::iterator i = itemIds.begin(); i != itemIds.end(); i++)
        {
            uint32 itemId = *i;
            const ItemPrototype* proto = sObjectMgr.GetItemPrototype(itemId);
            if (!proto)
                continue;

            VendorItemData const* tItems = pCreature->GetVendorItems();
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

    if (!vendored)
    {
        ai->TellError("There are no vendors nearby");
        return false;
    }

    return true;
}

bool BuyAction::BuyItem(VendorItemData const* tItems, ObjectGuid vendorguid, const ItemPrototype* proto)
{
    if (!tItems)
        return false;

    uint32 itemId = proto->ItemId;
    for (uint32 slot = 0; slot < tItems->GetItemCount(); slot++)
    {
        if (tItems->GetItem(slot)->item == itemId)
        {
            bool couldBuy = false;
#ifdef MANGOSBOT_TWO
            couldBuy = bot->BuyItemFromVendorSlot(vendorguid, slot, itemId, 1, NULL_BAG, NULL_SLOT);
#else
            couldBuy = bot->BuyItemFromVendor(vendorguid, itemId, 1, NULL_BAG, NULL_SLOT);
#endif
            if (couldBuy)
            {
                ostringstream out; out << "Buying " << ChatHelper::formatItem(proto);
                ai->TellMaster(out.str());
            }
            return true;
        }
    }

    return false;
}
