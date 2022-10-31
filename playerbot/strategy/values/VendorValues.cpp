#include "VendorValues.h"
#include "ItemUsageValue.h"
#include "BudgetValues.h"
#include "../../PlayerbotAI.h"

using namespace ai;

bool VendorHasUsefulItemValue::Calculate()
{
    uint32 entry = stoi(this->getQualifier());
    static CreatureInfo const* cInfo = sObjectMgr.GetCreatureTemplate(entry);

    if (!cInfo)
        return false;

    VendorItemList vendorItems;
    VendorItemData const* vItems = sObjectMgr.GetNpcVendorItemList(entry);

    if (vItems)
        for (auto vItem : vItems->m_items)
                vendorItems.push_back(vItem);

    uint32 vendorId = cInfo->VendorTemplateId;

    if (vendorId)
    {
        vItems = sObjectMgr.GetNpcVendorTemplateItemList(vendorId);

        if (vItems)
            for (auto vItem : vItems->m_items)
                vendorItems.push_back(vItem);
    }

    unordered_map <ItemUsage, uint32> freeMoney;

    freeMoney[ITEM_USAGE_EQUIP] = freeMoney[ITEM_USAGE_REPLACE] = AI_VALUE2(uint32, "free money for", (uint32)NeedMoneyFor::gear);
    freeMoney[ITEM_USAGE_USE] = AI_VALUE2(uint32, "free money for", (uint32)NeedMoneyFor::consumables);
    freeMoney[ITEM_USAGE_SKILL] = freeMoney[ITEM_USAGE_DISENCHANT] = AI_VALUE2(uint32, "free money for", (uint32)NeedMoneyFor::tradeskill);
    freeMoney[ITEM_USAGE_AMMO] = AI_VALUE2(uint32, "free money for", (uint32)NeedMoneyFor::ammo);
    freeMoney[ITEM_USAGE_QUEST] = AI_VALUE2(uint32, "free money for", (uint32)NeedMoneyFor::anything);

    for (auto vendorItem : vendorItems)
    {
        ItemPrototype const* proto = sObjectMgr.GetItemPrototype(vendorItem->item);
#ifndef MANGOSBOT_ZERO
        if (vendorItem->ExtendedCost) //Needs to be replaced with check if bot has (free) currency for this item.
            continue;
#endif

        ItemUsage usage = AI_VALUE2_LAZY(ItemUsage, "item usage", vendorItem->item);

        if (freeMoney.find(usage) == freeMoney.end() || proto->BuyPrice > freeMoney[usage])
            continue;

        return true;
    }

    return false;
}