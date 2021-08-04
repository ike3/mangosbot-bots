#pragma once
#include "../Value.h"

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
        ITEM_USAGE_AMMO = 13
    };

    class ItemUsageValue : public CalculatedValue<ItemUsage>, public Qualified
	{
	public:
        ItemUsageValue(PlayerbotAI* ai) : CalculatedValue<ItemUsage>(ai) {}

    public:
        virtual ItemUsage Calculate();

    private:
        ItemUsage QueryItemUsageForEquip(ItemPrototype const * proto);

        uint32 GetSmallestBagSize();
        bool IsItemUsefulForQuest(Player* player, ItemPrototype const* proto);
        bool IsItemNeededForSkill(ItemPrototype const* proto);
        bool IsItemUsefulForSkill(ItemPrototype const * proto);
        bool IsItemNeededForUsefullSpell(ItemPrototype const* proto, bool checkAllReagents = false);
        bool HasItemsNeededForSpell(uint32 spellId, ItemPrototype const* proto);
        Item* CurrentItem(ItemPrototype const* proto);
        float CurrentStacks(ItemPrototype const* proto);
        float BetterStacks(ItemPrototype const* proto, string usageType = "");

    public:
        static vector<uint32> SpellsUsingItem(uint32 itemId, Player* bot);
        static bool SpellGivesSkillUp(uint32 spellId, Player* bot);

        static string GetConsumableType(ItemPrototype const* proto, bool hasMana);
	};
}
