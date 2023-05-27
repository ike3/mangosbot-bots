#pragma once
#include "../Value.h"
#include "../NamedObjectContext.h"

namespace ai
{
    class ItemQualifier
    {
    public:
        ItemQualifier() { itemId = 0; enchantId = 0; randomPropertyId = 0; gem1 = 0; gem2 = 0; gem3 = 0; gem4 = 0; proto = nullptr; };
        ItemQualifier(uint32 itemId, int32 randomPropertyId = 0, uint32 enchantId = 0, uint32 gem1 = 0, uint32 gem2 = 0, uint32 gem3 = 0, uint32 gem4 = 0) : itemId(itemId), randomPropertyId(randomPropertyId), enchantId(enchantId), gem1(gem1), gem2(gem2), gem3(gem3), gem4(gem4) { proto = nullptr; };
        ItemQualifier(Item* item) { itemId = item->GetProto()->ItemId; enchantId = item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT); randomPropertyId = item->GetItemRandomPropertyId(); gem1 = GemId(item, 0); gem2 = GemId(item, 1); gem3 = GemId(item, 2); gem4 = GemId(item, 3); proto = item->GetProto();};
        ItemQualifier(LootItem* item) { itemId = item->itemId; enchantId = 0; randomPropertyId = item->randomPropertyId; gem1 = 0; gem2 = 0; gem3 = 0; gem4 = 0; proto = nullptr; };
        ItemQualifier(AuctionEntry* auction) { itemId = auction->itemTemplate; enchantId = 0; randomPropertyId = auction->itemRandomPropertyId; gem1 = 0; gem2 = 0; gem3 = 0; gem4 = 0; proto = nullptr;};
        ItemQualifier(string qualifier, bool linkQualifier = true);

        uint32 GetId() { return itemId; }
        uint32 GetEnchantId() { return enchantId; }
        int32 GetRandomPropertyId() { return randomPropertyId; }
        uint32 GetGem1() { return gem1; }
        uint32 GetGem2() { return gem2; }
        uint32 GetGem3() { return gem3; }
        uint32 GetGem4() { return gem4; }       

#ifdef MANGOSBOT_ZERO
        string GetLinkQualifier() { return to_string(itemId) + ":" + to_string(enchantId) + ":" + to_string(randomPropertyId) + ":0"; }
#else
        string GetLinkQualifier() { return to_string(itemId) + ":" + to_string(enchantId) + ":" + to_string(gem1) + ":" + to_string(gem2) + ":" + to_string(gem3) + ":" + to_string(gem4) + ":" + to_string(randomPropertyId) + ":0"; }
#endif
        string GetQualifier() { return to_string(itemId) + ((enchantId || gem1 || gem2 || gem3 || gem4 || randomPropertyId) ? ":" + to_string(enchantId) + ":" + to_string(gem1) + ":" + to_string(gem2) + ":" + to_string(gem3) + ":" + to_string(gem4) + ":" + to_string(randomPropertyId) : ""); }

        ItemPrototype const* GetProto() { if (!proto) proto = sItemStorage.LookupEntry<ItemPrototype>(itemId); return proto; };
        static uint32 GemId(Item* item, uint8 gemSlot = 0);
    private:
        uint32 itemId;
        uint32 enchantId;
        int32 randomPropertyId;
        uint32 gem1;
        uint32 gem2;
        uint32 gem3;
        uint32 gem4;
        ItemPrototype const* proto;
    };

    enum class ItemUsage : uint8
    {
        ITEM_USAGE_NONE = 0,
        ITEM_USAGE_EQUIP = 1,
        ITEM_USAGE_BAD_EQUIP = 2,
        ITEM_USAGE_BROKEN_EQUIP = 3,
        ITEM_USAGE_QUEST = 4,
        ITEM_USAGE_SKILL = 5,
        ITEM_USAGE_USE = 6,
        ITEM_USAGE_GUILD_TASK = 7,
        ITEM_USAGE_DISENCHANT = 8,
        ITEM_USAGE_AH = 9,
        ITEM_USAGE_KEEP = 10,
        ITEM_USAGE_VENDOR = 11,
        ITEM_USAGE_AMMO = 12,
        ITEM_USAGE_FORCE_NEED = 13,
        ITEM_USAGE_FORCE_GREED = 14
    };

    enum class ForceItemUsage : uint8
    {
        FORCE_USAGE_NONE = 0,  //Normal usage.
        FORCE_USAGE_KEEP = 1,  //Do not sell item.
        FORCE_USAGE_EQUIP = 2, //Equip item if no other forced equipped.
        FORCE_USAGE_GREED = 3,  //Get more and greed for rolls.
        FORCE_USAGE_NEED = 4    //Get more and need for rolls.
    };

    class ItemUsageValue : public CalculatedValue<ItemUsage>, public Qualified
	{
	public:
        ItemUsageValue(PlayerbotAI* ai, string name = "item usage") : CalculatedValue<ItemUsage>(ai, name), Qualified() {}
        virtual ItemUsage Calculate();

    private:
        ItemUsage QueryItemUsageForEquip(ItemQualifier& itemQualifier);
        uint32 GetSmallestBagSize();
        bool IsItemUsefulForQuest(Player* player, ItemPrototype const* proto, bool ignoreInventory = false);
        bool IsItemNeededForSkill(ItemPrototype const* proto);
        bool IsItemUsefulForSkill(ItemPrototype const * proto);
        bool IsItemNeededForUsefullCraft(ItemPrototype const* proto, bool checkAllReagents);
        Item* CurrentItem(ItemPrototype const* proto);
        float BetterStacks(ItemPrototype const* proto, string usageType = "");

#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "item usage"; } //Must equal iternal name
        virtual string GetHelpTypeName() { return "item"; }
        virtual string GetHelpDescription() 
        { 
            return "This value gives the reason why a bot finds an item useful.\n"
                   "Based on this value bots will equip/unequip/need/greed/loot/destroy/sell/ah/craft items."; 
        }
        virtual vector<string> GetUsedValues() { return {"bag space", "force item usage", "inventory items", "item count" }; }
#endif

    public:
        static float CurrentStacks(PlayerbotAI* ai, ItemPrototype const* proto);
        static vector<uint32> SpellsUsingItem(uint32 itemId, Player* bot);

        static string GetConsumableType(ItemPrototype const* proto, bool hasMana);
	};

    class ForceItemUsageValue : public ManualSetValue<ForceItemUsage>, public Qualified
    {
    public:
        ForceItemUsageValue(PlayerbotAI* ai, string name = "force item usage") : ManualSetValue<ForceItemUsage>(ai, ForceItemUsage::FORCE_USAGE_NONE, name), Qualified() {}

#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "force item usage"; } //Must equal iternal name
        virtual string GetHelpTypeName() { return "item"; }
        virtual string GetHelpDescription()
        {
            return "This value overrides some reasons why a bot finds an item useful\n"
                    "Based on this value bots will no longer sell/ah/destroy/unequip items.";
        }
        virtual vector<string> GetUsedValues() { return {}; }
#endif

        virtual string Save() override { return (uint8)value ? to_string((uint8)value) : "?"; }
        virtual bool Load(string force) override { if (!force.empty()) value = ForceItemUsage(stoi(force)); return !force.empty(); }        
    };
}
