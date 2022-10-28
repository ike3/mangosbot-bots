#include "botpch.h"
#include "../../playerbot.h"
#include "UseTrinketAction.h"

using namespace ai;

bool UseTrinketAction::Execute(Event& event)
{
	auto trinkets = AI_VALUE(list<Item*>, "trinkets on use");

	if (trinkets.empty())
		return false;

	for each (Item * item in trinkets)
	{
		ItemPrototype const* proto = item->GetProto();

		if (proto->InventoryType == INVTYPE_TRINKET && item->IsEquipped())
		{
			if (bot->CanUseItem(item) == EQUIP_ERR_OK && !item->IsInTrade())
			{
				return UseItemAuto(item);
			}
		}
	}

    return false;
}

bool UseTrinketAction::isPossible()
{
	return AI_VALUE(list<Item*>, "trinkets on use").size() > 0;
}
