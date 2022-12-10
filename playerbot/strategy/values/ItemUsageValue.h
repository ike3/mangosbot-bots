#pragma once
#include "../Value.h"
#include "../NamedObjectContext.h"

namespace ai
{
    enum ItemUsage
    {
        ITEM_USAGE_NONE = 0,
        ITEM_USAGE_EQUIP = 1,
        ITEM_USAGE_REPLACE = 2,
        ITEM_USAGE_BAD_EQUIP = 3,
        ITEM_USAGE_BROKEN_EQUIP = 4,
        ITEM_USAGE_QUEST = 5,
        ITEM_USAGE_SKILL = 6,
        ITEM_USAGE_USE = 7,
        ITEM_USAGE_GUILD_TASK = 8,
        ITEM_USAGE_DISENCHANT = 9,
        ITEM_USAGE_AH = 10,
        ITEM_USAGE_KEEP = 11,
        ITEM_USAGE_VENDOR = 12,
        ITEM_USAGE_AMMO = 13,
        ITEM_USAGE_FORCE = 14
    };

    enum class ForceItemUsage : uint8
    {
        FORCE_USAGE_NONE = 0,  //Normal usage.
        FORCE_USAGE_KEEP = 1,  //Do not sell item.
        FORCE_USAGE_EQUIP = 2, //Equip item. Need if no other forced equiped.
        FORCE_USAGE_GREED = 3,  //Get more and greed for rolls.
        FORCE_USAGE_NEED = 4    //Get more and need for rolls.
    };

    class ItemUsageValue : public CalculatedValue<ItemUsage>, public Qualified
	{
	public:
        ItemUsageValue(PlayerbotAI* ai, string name = "item usage") : CalculatedValue<ItemUsage>(ai, name) {}

    public:
        virtual ItemUsage Calculate();

    private:
        ItemUsage QueryItemUsageForEquip(ItemPrototype const * proto);

        uint32 GetSmallestBagSize();
        bool IsItemUsefulForQuest(Player* player, ItemPrototype const* proto, bool ignoreInventory = false);
        bool IsItemNeededForSkill(ItemPrototype const* proto);
        bool IsItemUsefulForSkill(ItemPrototype const * proto);
        bool IsItemNeededForUsefullSpell(ItemPrototype const* proto, bool checkAllReagents = false);
        Item* CurrentItem(ItemPrototype const* proto);
        float CurrentStacks(ItemPrototype const* proto);
        float BetterStacks(ItemPrototype const* proto, string usageType = "");
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "item usage"; } //Must equal iternal name
        virtual string GetHelpTypeName() { return "item"; }
        virtual string GetHelpDescription() 
        { return "This value gives the reason why a bot finds an item usefull.\n"
            "Based on this value bots will equip/unequip/need/greed/loot/destroy/sell/ah/craft items."; 
        }
        virtual vector<string> GetUsedValues() { return {"bag space", "force item usage", "inventory items", "item count" }; }
#endif 
    public:
        static bool HasItemsNeededForSpell(uint32 spellId, ItemPrototype const* proto, Player* bot);
        static vector<uint32> SpellsUsingItem(uint32 itemId, Player* bot);
        static bool SpellGivesSkillUp(uint32 spellId, Player* bot);

        static string GetConsumableType(ItemPrototype const* proto, bool hasMana);
	};

    class ForceItemUsageValue : public ManualSetValue<ForceItemUsage>, public Qualified
    {
    public:
        ForceItemUsageValue(PlayerbotAI* ai, string name = "force item usage") : ManualSetValue<ForceItemUsage>(ai, ForceItemUsage::FORCE_USAGE_NONE, name) {}
#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "force item usage"; } //Must equal iternal name
        virtual string GetHelpTypeName() { return "item"; }
        virtual string GetHelpDescription()
        {
            return "This value overrides some reasons why a bot finds an item usefull\n"
                "Based on this value bots will no longer sell/ah/destroy/unequip items.";
        }
        virtual vector<string> GetUsedValues() { return {}; }
#endif 
        virtual string Save() { return (uint8)value ? to_string((uint8)value) : "?"; }
        virtual bool Load(string force) { if (!force.empty()) value = ForceItemUsage(stoi(force)); return !force.empty(); }
    };
}
