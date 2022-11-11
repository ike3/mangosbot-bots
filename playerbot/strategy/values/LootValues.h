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

    class LootAccess
    {
    public:
        friend struct LootItem;
        friend class GroupLootRoll;
        friend class LootMgr;

        vector<LootItem> GetLootContentFor(Player* player) const;

        // What is looted
        WorldObject* m_lootTarget;
        Item* m_itemTarget;
        ObjectGuid       m_guidTarget;

        LootItemList     m_lootItems;                     // store of the items contained in loot
        uint32           m_gold;                          // amount of money contained in loot
        uint32           m_maxSlot;                       // used to increment slot index and get total items count
        LootType         m_lootType;                      // internal loot type
        ClientLootType   m_clientLootType;                // client loot type
        LootMethod       m_lootMethod;                    // used to know what kind of check must be done at loot time
        ItemQualities    m_threshold;                     // group threshold for items
        ObjectGuid       m_masterOwnerGuid;               // master loot player or round robin owner
        ObjectGuid       m_currentLooterGuid;             // current player for under threshold items (Round Robin)
        GuidSet          m_ownerSet;                      // set of all player who have right to the loot
        uint32           m_maxEnchantSkill;               // used to know group right to use disenchant option
        bool             m_haveItemOverThreshold;         // if at least one item in the loot is over threshold
        bool             m_isChecked;                     // true if at least one player received the loot content
        bool             m_isChest;                       // chest type object have special loot right
        bool             m_isChanged;                     // true if at least one item is looted
        bool             m_isFakeLoot;                    // nothing to loot but will sparkle for empty windows
        GroupLootRollMap m_roll;                          // used if an item is under rolling
        GuidSet          m_playersLooting;                // player who opened loot windows
        GuidSet          m_playersOpened;                 // players that have released the corpse
        TimePoint        m_createTime;                    // create time (used to refill loot if need)
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


    //Move to inventoryValues.h at some point
    class StackSpaceForItem : public Uint32CalculatedValue, public Qualified
    {
    public:
        StackSpaceForItem(PlayerbotAI* ai) : Uint32CalculatedValue(ai, "stack space for item", 2) {}

        virtual uint32 Calculate();
    };

    class ShouldLootObject : public BoolCalculatedValue, public Qualified
    {
    public:
        ShouldLootObject(PlayerbotAI* ai) : BoolCalculatedValue(ai, "should loot object") {}

        virtual bool Calculate();
    };
}

