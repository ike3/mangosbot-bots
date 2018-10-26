#ifndef _RandomItemMgr_H
#define _RandomItemMgr_H

#include "Common.h"
#include "PlayerbotAIBase.h"

using namespace std;

enum RandomItemType
{
    RANDOM_ITEM_GUILD_TASK,
    RANDOM_ITEM_GUILD_TASK_REWARD_EQUIP_BLUE,
    RANDOM_ITEM_GUILD_TASK_REWARD_EQUIP_GREEN,
    RANDOM_ITEM_GUILD_TASK_REWARD_TRADE
};

class RandomItemPredicate
{
public:
    virtual bool Apply(ItemPrototype const* proto) = 0;
};

typedef vector<uint32> RandomItemList;
typedef map<RandomItemType, RandomItemList> RandomItemCache;

class BotEquipKey
{
public:
    uint32 level;
    uint8 clazz;
    uint8 slot;
    uint32 quality;
    uint64 key;

public:
    BotEquipKey() : level(0), clazz(0), slot(0), quality(0), key(GetKey()) {}
    BotEquipKey(uint32 level, uint8 clazz, uint8 slot, uint32 quality) : level(level), clazz(clazz), slot(slot), quality(quality), key(GetKey()) {}
    BotEquipKey(BotEquipKey const& other)  : level(other.level), clazz(other.clazz), slot(other.slot), quality(other.quality), key(GetKey()) {}

private:
    uint64 GetKey();

public:
    bool operator< (const BotEquipKey& other) const
    {
        return other.key < this->key;
    }
};

typedef map<BotEquipKey, RandomItemList> BotEquipCache;

class RandomItemMgr
{
    public:
        RandomItemMgr();
        virtual ~RandomItemMgr();
        static RandomItemMgr& instance()
        {
            static RandomItemMgr instance;
            return instance;
        }

	public:
        void Init();
        static bool HandleConsoleCommand(ChatHandler* handler, char const* args);
        RandomItemList Query(uint32 level, RandomItemType type, RandomItemPredicate* predicate);
        RandomItemList Query(uint32 level, uint8 clazz, uint8 slot, uint32 quality);
        uint32 GetRandomItem(uint32 level, RandomItemType type, RandomItemPredicate* predicate = NULL);
        uint32 GetAmmo(uint32 level, uint32 subClass);

    private:
        void BuildRandomItemCache();
        void BuildEquipCache();
        void BuildAmmoCache();
        bool CanEquipItem(BotEquipKey key, ItemPrototype const* proto);
        void AddItemStats(uint32 mod, uint8 &sp, uint8 &ap, uint8 &tank);
        bool CheckItemStats(BotEquipKey key, uint8 sp, uint8 ap, uint8 tank);
        bool CanEquipArmor(BotEquipKey key, ItemPrototype const* proto);
        bool CanEquipWeapon(BotEquipKey key, ItemPrototype const* proto);

    private:
        map<uint32, RandomItemCache> randomItemCache;
        map<RandomItemType, RandomItemPredicate*> predicates;
        BotEquipCache equipCache;
        map<EquipmentSlots, set<InventoryType> > viableSlots;
        map<uint32, map<uint32, uint32> > ammoCache;
};

#define sRandomItemMgr RandomItemMgr::instance()

#endif
