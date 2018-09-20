#include "../botpch.h"
#include "playerbot.h"
#include "PlayerbotAIConfig.h"
#include "RandomItemMgr.h"

#include "../../../shared/Database/DBCStore.h"
#include "../../modules/Bots/ahbot/AhBot.h"
#include "DatabaseEnv.h"
#include "PlayerbotAI.h"

#include "../../modules/Bots/ahbot/AhBotConfig.h"

char * strstri (const char* str1, const char* str2);

uint64 BotEquipKey::GetKey()
{
    return level + 100 * clazz + 10000 * slot + 1000000 * quality;
}

class RandomItemGuildTaskPredicate : public RandomItemPredicate
{
public:
    virtual bool Apply(ItemPrototype const* proto)
    {
        if (proto->Bonding == BIND_WHEN_PICKED_UP ||
                proto->Bonding == BIND_QUEST_ITEM ||
                proto->Bonding == BIND_WHEN_USE)
            return false;

        if (proto->Quality < ITEM_QUALITY_NORMAL)
            return false;

        if ((proto->Class == ITEM_CLASS_ARMOR || proto->Class == ITEM_CLASS_WEAPON) && proto->Quality >= ITEM_QUALITY_RARE)
            return true;

        if (proto->Class == ITEM_CLASS_TRADE_GOODS || proto->Class == ITEM_CLASS_CONSUMABLE)
            return true;

        return false;
    }
};

class RandomItemGuildTaskRewardPredicate : public RandomItemPredicate
{
public:
    RandomItemGuildTaskRewardPredicate(bool equip, bool rare) { this->equip = equip; this->rare = rare;}

    virtual bool Apply(ItemPrototype const* proto)
    {
        if (proto->Bonding == BIND_WHEN_PICKED_UP ||
                proto->Bonding == BIND_QUEST_ITEM ||
                proto->Bonding == BIND_WHEN_USE)
            return false;

        if (proto->Class == ITEM_CLASS_QUEST)
            return false;

        if (equip)
        {
            uint32 desiredQuality = rare ? ITEM_QUALITY_RARE : ITEM_QUALITY_UNCOMMON;
            if (proto->Quality < desiredQuality)
                return false;

            if (proto->Class == ITEM_CLASS_ARMOR || proto->Class == ITEM_CLASS_WEAPON)
                return true;
        }
        else
        {
            if (proto->Quality < ITEM_QUALITY_UNCOMMON)
                return false;

            if (proto->Class == ITEM_CLASS_TRADE_GOODS || proto->Class == ITEM_CLASS_CONSUMABLE)
                return true;
        }

        return false;
    }

private:
    bool equip;
    bool rare;
};

RandomItemMgr::RandomItemMgr()
{
    predicates[RANDOM_ITEM_GUILD_TASK] = new RandomItemGuildTaskPredicate();
    predicates[RANDOM_ITEM_GUILD_TASK_REWARD_EQUIP_GREEN] = new RandomItemGuildTaskRewardPredicate(true, false);
    predicates[RANDOM_ITEM_GUILD_TASK_REWARD_EQUIP_BLUE] = new RandomItemGuildTaskRewardPredicate(true, true);
    predicates[RANDOM_ITEM_GUILD_TASK_REWARD_TRADE] = new RandomItemGuildTaskRewardPredicate(false, false);

    viableSlots[EQUIPMENT_SLOT_HEAD].insert(INVTYPE_HEAD);
    viableSlots[EQUIPMENT_SLOT_NECK].insert(INVTYPE_NECK);
    viableSlots[EQUIPMENT_SLOT_SHOULDERS].insert(INVTYPE_SHOULDERS);
    viableSlots[EQUIPMENT_SLOT_BODY].insert(INVTYPE_BODY);
    viableSlots[EQUIPMENT_SLOT_CHEST].insert(INVTYPE_CHEST);
    viableSlots[EQUIPMENT_SLOT_CHEST].insert(INVTYPE_ROBE);
    viableSlots[EQUIPMENT_SLOT_WAIST].insert(INVTYPE_WAIST);
    viableSlots[EQUIPMENT_SLOT_LEGS].insert(INVTYPE_LEGS);
    viableSlots[EQUIPMENT_SLOT_FEET].insert(INVTYPE_FEET);
    viableSlots[EQUIPMENT_SLOT_WRISTS].insert(INVTYPE_WRISTS);
    viableSlots[EQUIPMENT_SLOT_HANDS].insert(INVTYPE_HANDS);
    viableSlots[EQUIPMENT_SLOT_FINGER1].insert(INVTYPE_FINGER);
    viableSlots[EQUIPMENT_SLOT_FINGER2].insert(INVTYPE_FINGER);
    viableSlots[EQUIPMENT_SLOT_TRINKET1].insert(INVTYPE_TRINKET);
    viableSlots[EQUIPMENT_SLOT_TRINKET2].insert(INVTYPE_TRINKET);
    viableSlots[EQUIPMENT_SLOT_MAINHAND].insert(INVTYPE_WEAPON);
    viableSlots[EQUIPMENT_SLOT_MAINHAND].insert(INVTYPE_2HWEAPON);
    viableSlots[EQUIPMENT_SLOT_MAINHAND].insert(INVTYPE_WEAPONMAINHAND);
    viableSlots[EQUIPMENT_SLOT_OFFHAND].insert(INVTYPE_WEAPON);
    viableSlots[EQUIPMENT_SLOT_OFFHAND].insert(INVTYPE_2HWEAPON);
    viableSlots[EQUIPMENT_SLOT_OFFHAND].insert(INVTYPE_SHIELD);
    viableSlots[EQUIPMENT_SLOT_OFFHAND].insert(INVTYPE_WEAPONMAINHAND);
    viableSlots[EQUIPMENT_SLOT_OFFHAND].insert(INVTYPE_HOLDABLE);
    viableSlots[EQUIPMENT_SLOT_RANGED].insert(INVTYPE_RANGED);
    viableSlots[EQUIPMENT_SLOT_RANGED].insert(INVTYPE_THROWN);
    viableSlots[EQUIPMENT_SLOT_RANGED].insert(INVTYPE_RANGEDRIGHT);
    viableSlots[EQUIPMENT_SLOT_RANGED].insert(INVTYPE_RELIC);
    viableSlots[EQUIPMENT_SLOT_TABARD].insert(INVTYPE_TABARD);
    viableSlots[EQUIPMENT_SLOT_BACK].insert(INVTYPE_CLOAK);

}

void RandomItemMgr::Init()
{
    BuildEquipCache();
}

RandomItemMgr::~RandomItemMgr()
{
    for (map<RandomItemType, RandomItemPredicate*>::iterator i = predicates.begin(); i != predicates.end(); ++i)
        delete i->second;

    predicates.clear();
}

bool RandomItemMgr::HandleConsoleCommand(ChatHandler* handler, char const* args)
{
    if (!args || !*args)
    {
        sLog.outError( "Usage: rnditem");
        return false;
    }

    return false;
}

RandomItemList RandomItemMgr::Query(RandomItemType type, RandomItemPredicate* predicate)
{
    RandomItemList &list = cache[type];
    if (list.empty())
        list = cache[type] = Query(type);

    RandomItemList result;
    for (RandomItemList::iterator i = list.begin(); i != list.end(); ++i)
    {
        uint32 itemId = *i;
        ItemPrototype const* proto = sObjectMgr.GetItemPrototype(itemId);
        if (!proto)
            continue;

        if (predicate && !predicate->Apply(proto))
            continue;

        result.push_back(itemId);
    }

    return result;
}

RandomItemList RandomItemMgr::Query(RandomItemType type)
{
    RandomItemList items;

	for (uint32 itemId = 0; itemId < sItemStorage.GetMaxEntry(); ++itemId)
	{
		ItemPrototype const* proto = sObjectMgr.GetItemPrototype(itemId);
        if (!proto)
            continue;

        if (proto->Duration & 0x80000000)
            continue;

        if (sAhBotConfig.ignoreItemIds.find(proto->ItemId) != sAhBotConfig.ignoreItemIds.end())
            continue;

        if (strstri(proto->Name1, "qa") || strstri(proto->Name1, "test") || strstri(proto->Name1, "deprecated"))
            continue;

        if ((proto->RequiredLevel && proto->RequiredLevel > sAhBotConfig.maxRequiredLevel) || proto->ItemLevel > sAhBotConfig.maxItemLevel)
            continue;

        if (predicates[type] && !predicates[type]->Apply(proto))
            continue;

        if (!auctionbot.GetSellPrice(proto))
            continue;

        items.push_back(itemId);
    }

    if (items.empty())
        sLog.outError( "no items available for random item query %u", type);

    return items;
}

uint32 RandomItemMgr::GetRandomItem(RandomItemType type, RandomItemPredicate* predicate)
{
    RandomItemList const& list = Query(type, predicate);
    if (list.empty())
        return 0;

    uint32 index = urand(0, list.size() - 1);
    uint32 itemId = list[index];

    return itemId;
}

bool RandomItemMgr::CanEquipItem(BotEquipKey key, ItemPrototype const* proto)
{
    if (proto->Duration & 0x80000000)
        return false;

    if (proto->Quality != key.quality)
        return false;

    if (proto->Bonding == BIND_QUEST_ITEM || proto->Bonding == BIND_WHEN_USE)
        return false;

    if (proto->Class == ITEM_CLASS_CONTAINER)
        return true;

    set<InventoryType> slots = viableSlots[(EquipmentSlots)key.slot];
    if (slots.find((InventoryType)proto->InventoryType) == slots.end())
        return false;

    uint32 requiredLevel = proto->RequiredLevel;
    if (!requiredLevel)
        return false;

    uint32 level = key.level;
    uint32 delta = 2;
    if (level < 15)
        delta = urand(7, 15);
    else if (proto->Class == ITEM_CLASS_WEAPON || proto->SubClass == ITEM_SUBCLASS_ARMOR_SHIELD)
        delta = urand(2, 3);
    else if (!(level % 10) || (level % 10) == 9)
        delta = 2;
    else if (level < 40)
        delta = urand(5, 10);
    else if (level < 60)
        delta = urand(3, 7);
    else if (level < 70)
        delta = urand(2, 5);
    else if (level < 80)
        delta = urand(2, 4);

    if (key.quality > ITEM_QUALITY_NORMAL &&
            (requiredLevel > level || requiredLevel < level - delta))
        return false;

    for (uint32 gap = 60; gap <= 80; gap += 10)
    {
        if (level > gap && requiredLevel <= gap)
            return false;
    }

    return true;
}


void RandomItemMgr::AddItemStats(uint32 mod, uint8 &sp, uint8 &ap, uint8 &tank)
{
    switch (mod)
    {
    case ITEM_MOD_HEALTH:
    case ITEM_MOD_STAMINA:
    case ITEM_MOD_MANA:
    case ITEM_MOD_INTELLECT:
    case ITEM_MOD_SPIRIT:
        sp++;
        break;
    }

    switch (mod)
    {
    case ITEM_MOD_AGILITY:
    case ITEM_MOD_STRENGTH:
    case ITEM_MOD_HEALTH:
    case ITEM_MOD_STAMINA:
        tank++;
        break;
    }

    switch (mod)
    {
    case ITEM_MOD_HEALTH:
    case ITEM_MOD_STAMINA:
    case ITEM_MOD_AGILITY:
    case ITEM_MOD_STRENGTH:
        ap++;
        break;
    }
}

bool RandomItemMgr::CheckItemStats(BotEquipKey key, uint8 sp, uint8 ap, uint8 tank)
{
    switch (key.clazz)
    {
    case CLASS_PRIEST:
    case CLASS_MAGE:
    case CLASS_WARLOCK:
        if (!sp || ap > sp || tank > sp)
            return false;
        break;
    case CLASS_PALADIN:
    case CLASS_WARRIOR:
        if ((!ap && !tank) || sp > ap || sp > tank)
            return false;
        break;
    case CLASS_HUNTER:
    case CLASS_ROGUE:
        if (!ap || sp > ap || sp > tank)
            return false;
        break;
    }

    return sp || ap || tank;
}

bool RandomItemMgr::CanEquipArmor(BotEquipKey key, ItemPrototype const* proto)
{
    if ((key.clazz == CLASS_WARRIOR || key.clazz == CLASS_PALADIN || key.clazz == CLASS_SHAMAN)
            && proto->SubClass == ITEM_SUBCLASS_ARMOR_SHIELD)
        return true;

    if ((key.clazz == CLASS_WARRIOR || key.clazz == CLASS_PALADIN) && key.level >= 40)
    {
        if (proto->SubClass != ITEM_SUBCLASS_ARMOR_PLATE)
            return false;
    }

    if (((key.clazz == CLASS_WARRIOR || key.clazz == CLASS_PALADIN) && key.level < 40) ||
            (key.clazz == CLASS_HUNTER || key.clazz == CLASS_SHAMAN) && key.level >= 40)
    {
        if (proto->SubClass != ITEM_SUBCLASS_ARMOR_MAIL)
            return false;
    }

    if (((key.clazz == CLASS_HUNTER || key.clazz == CLASS_SHAMAN) && key.level < 40) ||
            (key.clazz == CLASS_DRUID || key.clazz == CLASS_ROGUE))
    {
        if (proto->SubClass != ITEM_SUBCLASS_ARMOR_LEATHER)
            return false;
    }

    if (proto->Quality <= ITEM_QUALITY_NORMAL)
        return true;

    uint8 sp = 0, ap = 0, tank = 0;
    for (int j = 0; j < MAX_ITEM_PROTO_STATS; ++j)
    {
        // for ItemStatValue != 0
        if(!proto->ItemStat[j].ItemStatValue)
            continue;

        AddItemStats(proto->ItemStat[j].ItemStatType, sp, ap, tank);
    }

    return CheckItemStats(key, sp, ap, tank);
}

bool RandomItemMgr::CanEquipWeapon(BotEquipKey key, ItemPrototype const* proto)
{
    switch (key.clazz)
    {
    case CLASS_PRIEST:
        if (proto->SubClass != ITEM_SUBCLASS_WEAPON_STAFF &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_WAND &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_MACE)
            return false;
        break;
    case CLASS_MAGE:
    case CLASS_WARLOCK:
        if (proto->SubClass != ITEM_SUBCLASS_WEAPON_STAFF &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_WAND &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_SWORD)
            return false;
        break;
    case CLASS_WARRIOR:
        if (proto->SubClass != ITEM_SUBCLASS_WEAPON_MACE2 &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_SWORD2 &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_MACE &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_SWORD &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_GUN &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_CROSSBOW &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_BOW &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_THROWN)
            return false;
        break;
    case CLASS_PALADIN:
        if (proto->SubClass != ITEM_SUBCLASS_WEAPON_MACE2 &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_SWORD2 &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_MACE &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_SWORD)
            return false;
        break;
    case CLASS_SHAMAN:
        if (proto->SubClass != ITEM_SUBCLASS_WEAPON_MACE &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_MACE2 &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_STAFF)
            return false;
        break;
    case CLASS_DRUID:
        if (proto->SubClass != ITEM_SUBCLASS_WEAPON_MACE &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_MACE2 &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_DAGGER &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_STAFF)
            return false;
        break;
    case CLASS_HUNTER:
        if (proto->SubClass != ITEM_SUBCLASS_WEAPON_AXE2 &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_SWORD2 &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_GUN &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_CROSSBOW &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_BOW)
            return false;
        break;
    case CLASS_ROGUE:
        if (proto->SubClass != ITEM_SUBCLASS_WEAPON_DAGGER &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_SWORD &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_MACE &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_GUN &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_CROSSBOW &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_BOW &&
                proto->SubClass != ITEM_SUBCLASS_WEAPON_THROWN)
            return false;
        break;
    }

    return true;
}


void RandomItemMgr::BuildEquipCache()
{
    uint32 maxLevel = sPlayerbotAIConfig.randomBotMaxLevel;
    if (maxLevel > sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
        maxLevel = sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL);

    QueryResult* results = CharacterDatabase.PQuery("select clazz, lvl, slot, quality, item from ai_playerbot_equip_cache");
    if (results)
    {
        sLog.outString("Loading equipment cache for %d classes, %d levels, %d slots, %d quality from %d items",
                MAX_CLASSES, maxLevel, EQUIPMENT_SLOT_END, ITEM_QUALITY_ARTIFACT, sItemStorage.GetMaxEntry());
        int count = 0;
        do
        {
            Field* fields = results->Fetch();
            uint32 clazz = fields[0].GetUInt32();
            uint32 level = fields[1].GetUInt32();
            uint32 slot = fields[2].GetUInt32();
            uint32 quality = fields[3].GetUInt32();
            uint32 itemId = fields[4].GetUInt32();

            BotEquipKey key(level, clazz, slot, quality);
            equipCache[key].push_back(itemId);
            count++;

        } while (results->NextRow());
        delete results;
        sLog.outString("Equipment cache loaded from %d records", count);
    }
    else
    {
        uint64 total = MAX_CLASSES * maxLevel * EQUIPMENT_SLOT_END * ITEM_QUALITY_ARTIFACT;
        sLog.outString("Building equipment cache for %d classes, %d levels, %d slots, %d quality from %d items (%d total)",
                MAX_CLASSES, maxLevel, EQUIPMENT_SLOT_END, ITEM_QUALITY_ARTIFACT, sItemStorage.GetMaxEntry(), total);

        BarGoLink bar(total);
        for (uint8 clazz = CLASS_WARRIOR; clazz < MAX_CLASSES; ++clazz)
        {
            // skip nonexistent classes
            if (!((1 << (clazz - 1)) & CLASSMASK_ALL_PLAYABLE) || !sChrClassesStore.LookupEntry(clazz))
                continue;

            for (uint32 level = 1; level <= maxLevel; ++level)
            {
                for (uint8 slot = 0; slot < EQUIPMENT_SLOT_END; ++slot)
                {
                    for (uint32 quality = ITEM_QUALITY_POOR; quality <= ITEM_QUALITY_ARTIFACT; ++quality)
                    {
                        BotEquipKey key(level, clazz, slot, quality);

                        RandomItemList items;
                        for (uint32 itemId = 0; itemId < sItemStorage.GetMaxEntry(); ++itemId)
                        {
                            ItemPrototype const* proto = sObjectMgr.GetItemPrototype(itemId);
                            if (!proto)
                                continue;

                            if (proto->Class != ITEM_CLASS_WEAPON &&
                                proto->Class != ITEM_CLASS_ARMOR &&
                                proto->Class != ITEM_CLASS_CONTAINER &&
                                proto->Class != ITEM_CLASS_PROJECTILE)
                                continue;

                            if (!CanEquipItem(key, proto))
                                continue;

                            if (proto->Class == ITEM_CLASS_ARMOR && (
                                slot == EQUIPMENT_SLOT_HEAD ||
                                slot == EQUIPMENT_SLOT_SHOULDERS ||
                                slot == EQUIPMENT_SLOT_CHEST ||
                                slot == EQUIPMENT_SLOT_WAIST ||
                                slot == EQUIPMENT_SLOT_LEGS ||
                                slot == EQUIPMENT_SLOT_FEET ||
                                slot == EQUIPMENT_SLOT_WRISTS ||
                                slot == EQUIPMENT_SLOT_HANDS) && !CanEquipArmor(key, proto))
                                    continue;

                            if (proto->Class == ITEM_CLASS_WEAPON && !CanEquipWeapon(key, proto))
                                continue;

                            if (slot == EQUIPMENT_SLOT_OFFHAND && key.clazz == CLASS_ROGUE && proto->Class != ITEM_CLASS_WEAPON)
                                continue;

                            items.push_back(itemId);

                            CharacterDatabase.PExecute("insert into ai_playerbot_equip_cache (clazz, lvl, slot, quality, item) values (%u, %u, %u, %u, %u)",
                                    clazz, level, slot, quality, itemId);
                        }

                        equipCache[key] = items;
                        bar.step();
                        sLog.outDetail("Equipment cache for class: %d, level %d, slot %d, quality %d: %d items",
                                clazz, level, slot, quality, items.size());
                    }
                }
            }
        }
        sLog.outString("Equipment cache saved to DB");
    }
}

RandomItemList RandomItemMgr::Query(uint32 level, uint8 clazz, uint8 slot, uint32 quality)
{
    BotEquipKey key(level, clazz, slot, quality);
    return equipCache[key];
}
