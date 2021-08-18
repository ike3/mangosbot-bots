#pragma once
#include <boost/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>

namespace ai
{
    //
    enum class LootType : uint8
    {
        normal = 0, 
        disenchant = 1, 
        fishing = 2, 
        mail = 3,
        milling = 4,
        pickpocket = 5,
        prospecting = 6,
        skinning = 7
    };    

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
    typedef boost::bimap<boost::bimaps::multiset_of<uint32>, boost::bimaps::multiset_of<int32>> LootMap;

    //Returns the loot map of all entries
    class LootMapValue : public SingleCalculatedValue<LootMap*>
    {
    public:
        LootMapValue(PlayerbotAI* ai) : SingleCalculatedValue(ai, "loot map") {}

        static LootTemplateAccess const* GetLootTemplate(ObjectGuid guid, LootType type = LootType::normal);

        virtual LootMap* Calculate();
    };

    //Returns the items a specific entry can drop
    class EntryLootListValue : public SingleCalculatedValue<list<uint32>>, public Qualified
    {
    public:
        EntryLootListValue(PlayerbotAI* ai) : SingleCalculatedValue(ai, "entry loot list") {}

        virtual list<uint32> Calculate();
    };

    //Returns the entries that drop a specific item
    class ItemLootListValue : public SingleCalculatedValue<list<int32>>, public Qualified
    {
    public:
        ItemLootListValue(PlayerbotAI* ai) : SingleCalculatedValue(ai, "item loot list") {}

        virtual list<int32> Calculate();
    };

}

