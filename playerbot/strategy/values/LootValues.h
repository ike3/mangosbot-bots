#pragma once
#include <boost/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>
#include "ItemUsageValue.h"

namespace ai
{ 
    //Cheat class copy to hack into the loot system
    class LootTemplateAccess
    {
    public:
        class  LootGroup;                                   // A set of loot definitions for items (refs are not allowed inside)
        typedef std::vector<LootGroup> LootGroups;
        LootStoreItemList Entries;                          // not grouped only
        LootGroups        Groups;                           // groups have own (optimized) processing, grouped entries go there
    };

    //                   itemId, entry
    typedef unordered_multimap<uint32, int32> DropMap;
    typedef unordered_map<pair<uint32, int32> , float > ChanceMap;

    //Returns the loot map of all entries
    class DropMapValue : public SingleCalculatedValue<DropMap*>
    {
    public:
        DropMapValue(PlayerbotAI* ai) : SingleCalculatedValue(ai, "drop map") {}

        static LootTemplateAccess const* GetLootTemplate(ObjectGuid guid, LootType type = LOOT_CORPSE);

        virtual DropMap* Calculate();
    };

    //Returns the entries that drop a specific item
    class ItemDropListValue : public SingleCalculatedValue<list<int32>>, public Qualified
    {
    public:
        ItemDropListValue(PlayerbotAI* ai) : SingleCalculatedValue(ai, "item drop list") {}

        virtual list<int32> Calculate();
    };

    //Returns the items a specific entry can drop
    class EntryLootListValue : public SingleCalculatedValue<list<uint32>>, public Qualified
    {
    public:
        EntryLootListValue(PlayerbotAI* ai) : SingleCalculatedValue(ai, "entry loot list") {}

        virtual list<uint32> Calculate();
    };

    class LootChanceValue : public SingleCalculatedValue<float>, public Qualified
    {
    public:
        LootChanceValue(PlayerbotAI* ai) : SingleCalculatedValue(ai, "loot chance") {}

        virtual float Calculate();
    };

    typedef unordered_map<ItemUsage, vector<uint32>> itemUsageMap;

    class EntryLootUsageValue : public CalculatedValue<itemUsageMap>, public Qualified
    {
    public:
        EntryLootUsageValue(PlayerbotAI* ai) : CalculatedValue(ai, "entry loot usage",2) {}

        virtual itemUsageMap Calculate();
    };

    class HasUpgradeValue : public BoolCalculatedValue, public Qualified
    {
    public:
        HasUpgradeValue(PlayerbotAI* ai) : BoolCalculatedValue(ai, "has upgrade", 2) {}

        virtual bool Calculate() { itemUsageMap uMap = AI_VALUE2(itemUsageMap, "entry loot usage", getQualifier()); return uMap.find(ITEM_USAGE_EQUIP) != uMap.end() || uMap.find(ITEM_USAGE_REPLACE) != uMap.end(); };
    };
}

