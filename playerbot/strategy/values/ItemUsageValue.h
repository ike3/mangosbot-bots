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
        ITEM_USAGE_SKILL = 4,
        ITEM_USAGE_USE = 5,
        ITEM_USAGE_GUILD_TASK = 6,
        ITEM_USAGE_DISENCHANT = 7
    };

    class ItemUsageValue : public CalculatedValue<ItemUsage>, public Qualified
	{
	public:
        ItemUsageValue(PlayerbotAI* ai) : CalculatedValue<ItemUsage>(ai) {}

    public:
        virtual ItemUsage Calculate();

    private:
        ItemUsage QueryItemUsageForEquip(ItemPrototype const * proto);
        bool IsItemUsefulForSkill(ItemPrototype const * proto);
	};
}
