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
        ITEM_USAGE_QUEST = 4,
        ITEM_USAGE_SKILL = 5,
        ITEM_USAGE_USE = 6,
        ITEM_USAGE_GUILD_TASK = 7,
        ITEM_USAGE_DISENCHANT = 8,
        ITEM_USAGE_AH = 9,
        ITEM_USAGE_VENDOR = 10,
        ITEM_USAGE_AMMO = 11
    };

    class ItemUsageValue : public CalculatedValue<ItemUsage>, public Qualified
	{
	public:
        ItemUsageValue(PlayerbotAI* ai) : CalculatedValue<ItemUsage>(ai) {}

    public:
        virtual ItemUsage Calculate();

    private:
        ItemUsage QueryItemUsageForEquip(ItemPrototype const * proto);
        bool IsItemUsefulForQuest(Player const* player, uint32 itemId);
        bool IsItemUsefulForSkill(ItemPrototype const * proto);
	};
}
