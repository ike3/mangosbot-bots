#include "botpch.h"
#include "../../playerbot.h"
#include "ItemUsageValue.h"

#include "../../../ahbot/AhBot.h"
#include "../../GuildTaskMgr.h"
#include "../../RandomItemMgr.h"
using namespace ai;

ItemUsage ItemUsageValue::Calculate()
{
    uint32 itemId = atoi(qualifier.c_str());
    if (!itemId)
        return ITEM_USAGE_NONE;

    const ItemPrototype* proto = sObjectMgr.GetItemPrototype(itemId);
    if (!proto)
        return ITEM_USAGE_NONE;

    if (IsItemUsefulForSkill(proto))
    {
        float stacks = CurrentStacks(proto);
        if (stacks < 1 && AI_VALUE(uint8, "bag space") < 50)
            return ITEM_USAGE_SKILL; //Buy more.
        if (stacks < 2)
            return ITEM_USAGE_KEEP; //Keep current amount.
    }

    if (proto->Class == ITEM_CLASS_KEY)
        return ITEM_USAGE_USE;

    if (proto->Class == ITEM_CLASS_CONSUMABLE)
    {
        string foodType = "";
        if (proto->Spells[0].SpellCategory == 11)
            foodType = "food";
        else if (proto->Spells[0].SpellCategory == 59 && bot->HasMana())
            foodType = "drink";
        else if (proto->Spells[0].SpellCategory == SPELL_EFFECT_ENERGIZE && bot->HasMana())
            foodType = "mana potion";
        else if (proto->Spells[0].SpellCategory == SPELL_EFFECT_HEAL)
            foodType = "healing potion";

        if (!foodType.empty() && bot->CanUseItem(proto) == EQUIP_ERR_OK)
        {
            float stacks = BetterStacks(proto, foodType);
            if (stacks < 2)
            {
                stacks += CurrentStacks(proto);

                if (stacks < 2) 
                    return ITEM_USAGE_USE; //Buy some to get to 2 stacks
                else if (stacks < 3)       //Keep the item if less than 3 stacks
                    return ITEM_USAGE_KEEP;
            }
        }
    }

    if (bot->GetGuildId() && sGuildTaskMgr.IsGuildTaskItem(itemId, bot->GetGuildId()))
        return ITEM_USAGE_GUILD_TASK;

    ItemUsage equip = QueryItemUsageForEquip(proto);
    if (equip != ITEM_USAGE_NONE)
        return equip;



    if ((proto->Class == ITEM_CLASS_ARMOR || proto->Class == ITEM_CLASS_WEAPON) && proto->Bonding != BIND_WHEN_PICKED_UP &&
        ai->HasSkill(SKILL_ENCHANTING) && proto->Quality >= ITEM_QUALITY_UNCOMMON)
        return ITEM_USAGE_DISENCHANT;

    //While sync is on, do not loot quest items that are also usefull for master. Master 
    if (!ai->GetMaster() || !sPlayerbotAIConfig.syncQuestWithPlayer || !IsItemUsefulForQuest(ai->GetMaster(), proto))
        if (IsItemUsefulForQuest(bot, proto))
            return ITEM_USAGE_QUEST;

    if (proto->Class == ITEM_CLASS_PROJECTILE && bot->CanUseItem(proto) == EQUIP_ERR_OK)
        if (bot->getClass() == CLASS_HUNTER || bot->getClass() == CLASS_ROGUE || bot->getClass() == CLASS_WARRIOR)
        {
            Item* const pItem = bot->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);
            if (pItem)
            {
                uint32 subClass = 0;
                switch (pItem->GetProto()->SubClass)
                {
                case ITEM_SUBCLASS_WEAPON_GUN:
                    subClass = ITEM_SUBCLASS_BULLET;
                    break;
                case ITEM_SUBCLASS_WEAPON_BOW:
                case ITEM_SUBCLASS_WEAPON_CROSSBOW:
                    subClass = ITEM_SUBCLASS_ARROW;
                    break;
                }

                if (proto->SubClass == subClass)
                {
                    float ammo = BetterStacks(proto, "ammo");
                    float needAmmo = (bot->getClass() == CLASS_HUNTER) ? 8 : 2;

                    if (ammo < needAmmo) //We already have enough of the current ammo.
                    {
                        ammo += CurrentStacks(proto);

                        if (ammo < needAmmo)         //Buy ammo to get to the proper supply
                            return ITEM_USAGE_AMMO;
                        else if (ammo < needAmmo + 1)
                            return ITEM_USAGE_KEEP;  //Keep the ammo until we have too much.
                    }
                }
            }
        }

    //Need to add something like free bagspace or item value.
    if (proto->SellPrice > 0)
        if (proto->Quality > ITEM_QUALITY_NORMAL)
            return ITEM_USAGE_AH;
        else
            return ITEM_USAGE_VENDOR;

    return ITEM_USAGE_NONE;
}

ItemUsage ItemUsageValue::QueryItemUsageForEquip(ItemPrototype const* item)
{
    bool shouldEquip = true;
    bool existingShouldEquip = true;

    if (bot->CanUseItem(item) != EQUIP_ERR_OK)
        return ITEM_USAGE_NONE;

    if (item->InventoryType == INVTYPE_NON_EQUIP)
        return ITEM_USAGE_NONE;

    Item* pItem = Item::CreateItem(item->ItemId, 1, bot);
    if (!pItem)
        return ITEM_USAGE_NONE;

    uint16 dest;
    InventoryResult result = bot->CanEquipItem(NULL_SLOT, dest, pItem, true, false);
    pItem->RemoveFromUpdateQueueOf(bot);
    delete pItem;

    if (result != EQUIP_ERR_OK)
        return ITEM_USAGE_NONE;

    if (item->Class == ITEM_CLASS_QUIVER)
        if (bot->getClass() != CLASS_HUNTER)
            return ITEM_USAGE_NONE;

    if (item->Class == ITEM_CLASS_WEAPON && !sRandomItemMgr.CanEquipWeapon(bot->getClass(), item))
        shouldEquip = false;

    if (item->Class == ITEM_CLASS_ARMOR && !sRandomItemMgr.CanEquipArmor(bot->getClass(), bot->getLevel(), item))
        shouldEquip = false;

    Item* existingItem = bot->GetItemByPos(dest);
    if (!existingItem)
        if (shouldEquip)
            return ITEM_USAGE_EQUIP;
        else
            return ITEM_USAGE_BAD_EQUIP;

    const ItemPrototype* oldItem = existingItem->GetProto();

    if (item->Class == ITEM_CLASS_CONTAINER)
    {
        if (item->SubClass != ITEM_SUBCLASS_CONTAINER)
            return ITEM_USAGE_NONE; //Todo add logic for non-bag containers. We want to look at professions/class and only replace if non-bag is larger than bag.

        if (GetSmallestBagSize() >= item->ContainerSlots)
            return ITEM_USAGE_NONE;

        return ITEM_USAGE_EQUIP;
    }

    if (item->Class == ITEM_CLASS_QUIVER)
        if (!oldItem || oldItem->ContainerSlots < item->ContainerSlots)
            return ITEM_USAGE_EQUIP;
        else
            ITEM_USAGE_NONE;

    if (oldItem->Class == ITEM_CLASS_WEAPON && !sRandomItemMgr.CanEquipWeapon(bot->getClass(), oldItem))
        existingShouldEquip = false;

    if (oldItem->Class == ITEM_CLASS_ARMOR && !sRandomItemMgr.CanEquipArmor(bot->getClass(), bot->getLevel(), oldItem))
        existingShouldEquip = false;

    if (oldItem->ItemId != item->ItemId && //Item is not identical
        (shouldEquip || !existingShouldEquip) && //New item is optimal or old item was already sub-optimal
        (oldItem->ItemLevel + oldItem->Quality * 5 < item->ItemLevel + item->Quality * 5 // Item is upgrade
            ))
    {
        switch (item->Class)
        {
        case ITEM_CLASS_ARMOR:
            if (oldItem->SubClass <= item->SubClass) {
                if (shouldEquip)
                    if(CurrentItem(item) && CurrentItem(item)->GetUInt32Value(ITEM_FIELD_DURABILITY) == 0)
                        return ITEM_USAGE_BROKEN_EQUIP;
                    else
                        return ITEM_USAGE_REPLACE;
                else
                    return ITEM_USAGE_BAD_EQUIP;
            }
            break;
        default:
            if (shouldEquip)
                if (CurrentItem(item) && CurrentItem(item)->GetUInt32Value(ITEM_FIELD_DURABILITY) == 0)
                    return ITEM_USAGE_BROKEN_EQUIP;
                else
                    return ITEM_USAGE_EQUIP;
            else
                return ITEM_USAGE_BAD_EQUIP;
        }
    }

    //Item is not better but current item is broken and new one is not.
    if (existingItem->GetUInt32Value(ITEM_FIELD_DURABILITY) == 0 && CurrentItem(item) && CurrentItem(item)->GetUInt32Value(ITEM_FIELD_DURABILITY) > 0)
        return ITEM_USAGE_EQUIP;

    return ITEM_USAGE_NONE;
}

//Return smaltest bag size equipped
uint32 ItemUsageValue::GetSmallestBagSize()
{
    int8 curSlot = 0;
    int8 curSlots = 0;
    for (uint8 bag = INVENTORY_SLOT_BAG_START + 1; bag < INVENTORY_SLOT_BAG_END; ++bag)
    {
        const Bag* const pBag = (Bag*)bot->GetItemByPos(INVENTORY_SLOT_BAG_0, bag);
        if (pBag)
        {
            if (curSlot > 0 && curSlots < pBag->GetBagSize())
                continue;

            curSlot = pBag->GetSlot();
            curSlots = pBag->GetBagSize();
        }
        else
            return 0;
    }

    return curSlots;
}

bool ItemUsageValue::IsItemUsefulForQuest(Player* player, ItemPrototype const* proto)
{
    for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        uint32 entry = player->GetQuestSlotQuestId(slot);
        Quest const* quest = sObjectMgr.GetQuestTemplate(entry);
        if (!quest)
            continue;

        for (int i = 0; i < 4; i++)
        {
            if (quest->ReqItemId[i] != proto->ItemId)
                continue;

            if (player->GetPlayerbotAI() && AI_VALUE2(uint32, "item count", proto->Name1) >= quest->ReqItemCount[i])
                continue;

            return true;
        }
    }

    return false;
}

bool ItemUsageValue::IsItemUsefulForSkill(ItemPrototype const * proto)
{
    switch (proto->ItemId)
    {
    case 2901: //Mining pick
        return ai->HasSkill(SKILL_MINING);
    case 5956: //Blacksmith Hammer
        return ai->HasSkill(SKILL_BLACKSMITHING) || ai->HasSkill(SKILL_ENGINEERING);
    case 6219: //Arclight Spanner
        return ai->HasSkill(SKILL_ENGINEERING);
    case 16207: //Runed Arcanite Rod
        return ai->HasSkill(SKILL_ENCHANTING);
    case 7005: //Skinning Knife
        return ai->HasSkill(SKILL_SKINNING);
    case 4471: //Flint and Tinder
        return ai->HasSkill(SKILL_COOKING);
    case 4470: //Simple Wood
        return ai->HasSkill(SKILL_COOKING);
    case 6256: //Fishing Rod
        return ai->HasSkill(SKILL_FISHING);
    }    

    switch (proto->Class)
    {
    case ITEM_CLASS_TRADE_GOODS:
    case ITEM_CLASS_MISC:
    case ITEM_CLASS_REAGENT:
        {
            if (ai->HasSkill(SKILL_TAILORING) && auctionbot.IsUsedBySkill(proto, SKILL_TAILORING))
                return true;
            if (ai->HasSkill(SKILL_LEATHERWORKING) && auctionbot.IsUsedBySkill(proto, SKILL_LEATHERWORKING))
                return true;
            if (ai->HasSkill(SKILL_ENGINEERING) && auctionbot.IsUsedBySkill(proto, SKILL_ENGINEERING))
                return true;
            if (ai->HasSkill(SKILL_BLACKSMITHING) && auctionbot.IsUsedBySkill(proto, SKILL_BLACKSMITHING))
                return true;
            if (ai->HasSkill(SKILL_ALCHEMY) && auctionbot.IsUsedBySkill(proto, SKILL_ALCHEMY))
                return true;
            if (ai->HasSkill(SKILL_ENCHANTING) && auctionbot.IsUsedBySkill(proto, SKILL_ENCHANTING))
                return true;
            if (ai->HasSkill(SKILL_FISHING) && auctionbot.IsUsedBySkill(proto, SKILL_FISHING))
                return true;
            if (ai->HasSkill(SKILL_FIRST_AID) && auctionbot.IsUsedBySkill(proto, SKILL_FIRST_AID))
                return true;
            if (ai->HasSkill(SKILL_COOKING) && auctionbot.IsUsedBySkill(proto, SKILL_COOKING))
                return true;
#ifndef MANGOSBOT_ZERO
            if (ai->HasSkill(SKILL_JEWELCRAFTING) && auctionbot.IsUsedBySkill(proto, SKILL_JEWELCRAFTING))
                return true;
#endif
            if (ai->HasSkill(SKILL_MINING) &&
                    (
                            auctionbot.IsUsedBySkill(proto, SKILL_MINING) ||
                            auctionbot.IsUsedBySkill(proto, SKILL_BLACKSMITHING) ||
#ifndef MANGOSBOT_ZERO
                            auctionbot.IsUsedBySkill(proto, SKILL_JEWELCRAFTING) ||
#endif
                            auctionbot.IsUsedBySkill(proto, SKILL_ENGINEERING)
                    ))
                return true;
            if (ai->HasSkill(SKILL_SKINNING) &&
                    (auctionbot.IsUsedBySkill(proto, SKILL_SKINNING) || auctionbot.IsUsedBySkill(proto, SKILL_LEATHERWORKING)))
                return true;
            if (ai->HasSkill(SKILL_HERBALISM) &&
                    (auctionbot.IsUsedBySkill(proto, SKILL_HERBALISM) || auctionbot.IsUsedBySkill(proto, SKILL_ALCHEMY)))
                return true;

            return false;
        }
    case ITEM_CLASS_RECIPE:
        {
            if (bot->HasSpell(proto->Spells[2].SpellId))
                break;

            switch (proto->SubClass)
            {
            case ITEM_SUBCLASS_LEATHERWORKING_PATTERN:
                return ai->HasSkill(SKILL_LEATHERWORKING);
            case ITEM_SUBCLASS_TAILORING_PATTERN:
                return ai->HasSkill(SKILL_TAILORING);
            case ITEM_SUBCLASS_ENGINEERING_SCHEMATIC:
                return ai->HasSkill(SKILL_ENGINEERING);
            case ITEM_SUBCLASS_BLACKSMITHING:
                return ai->HasSkill(SKILL_BLACKSMITHING);
            case ITEM_SUBCLASS_COOKING_RECIPE:
                return ai->HasSkill(SKILL_COOKING);
            case ITEM_SUBCLASS_ALCHEMY_RECIPE:
                return ai->HasSkill(SKILL_ALCHEMY);
            case ITEM_SUBCLASS_FIRST_AID_MANUAL:
                return ai->HasSkill(SKILL_FIRST_AID);
            case ITEM_SUBCLASS_ENCHANTING_FORMULA:
                return ai->HasSkill(SKILL_ENCHANTING);
            case ITEM_SUBCLASS_FISHING_MANUAL:
                return ai->HasSkill(SKILL_FISHING);
            }
        }
    }
    return false;
}

Item* ItemUsageValue::CurrentItem(ItemPrototype const* proto)
{
    Item* bestItem = nullptr;
    list<Item*> found = AI_VALUE2(list < Item*>, "inventory items", chat->formatItem(proto));

    for (auto item : found)
    {
        if (bestItem && item->GetUInt32Value(ITEM_FIELD_DURABILITY) < bestItem->GetUInt32Value(ITEM_FIELD_DURABILITY))
            continue;

        if (bestItem && item->GetCount() < bestItem->GetCount())
            continue;

        bestItem = item;
    }

    return bestItem;
}


float ItemUsageValue::CurrentStacks(ItemPrototype const* proto)
{
    uint32 maxStack = proto->GetMaxStackSize();

    list<Item*> found = AI_VALUE2(list < Item*>, "inventory items", chat->formatItem(proto));

    float itemCount = 0;

    for (auto stack : found)
    {
        itemCount += stack->GetCount();
    }

    return itemCount / maxStack;
}

float ItemUsageValue::BetterStacks(ItemPrototype const* proto, string itemType)
{
    list<Item*> items = AI_VALUE2(list<Item*>, "inventory items", itemType);

    float stacks = 0;

    for (auto& otherItem : items)
    {
        const ItemPrototype* otherProto = otherItem->GetProto();

        if (otherProto->Class != proto->Class || otherProto->SubClass != proto->SubClass)
            continue;

        if (otherProto->ItemLevel < proto->ItemLevel)
            continue;

        if (otherProto->ItemId == proto->ItemId)
            continue;

        stacks += CurrentStacks(otherProto);
    }

    return stacks;
}