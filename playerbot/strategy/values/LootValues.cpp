#include "../../../botpch.h"
#include "../../playerbot.h"
#include "SharedValueContext.h"
#include "LootValues.h"

using namespace ai;


LootTemplateAccess const* LootMapValue::GetLootTemplate(ObjectGuid guid, LootType type)
{
	LootTemplate const* lTemplate = nullptr;

	if (guid.IsCreature())
	{
		CreatureInfo const* info = sObjectMgr.GetCreatureTemplate(guid.GetEntry());

		if (info)
		{
			if (type == LOOT_CORPSE)
				lTemplate = LootTemplates_Creature.GetLootFor(info->LootId);
			else if (type == LOOT_PICKPOCKETING && info->PickpocketLootId)
				lTemplate = LootTemplates_Pickpocketing.GetLootFor(info->PickpocketLootId);
			else if (type == LOOT_SKINNING && info->SkinningLootId)
				lTemplate = LootTemplates_Skinning.GetLootFor(info->SkinningLootId);
		}
	}
	else if (guid.IsGameObject())
	{
		GameObjectInfo const* info = sObjectMgr.GetGameObjectInfo(guid.GetEntry());

		if (info && info->GetLootId() != 0)
		{
			if (type == LOOT_CORPSE)
				lTemplate = LootTemplates_Gameobject.GetLootFor(info->GetLootId());
			else if (type == LOOT_FISHINGHOLE)
				lTemplate = LootTemplates_Fishing.GetLootFor(info->GetLootId());
		}
	}
	else if (guid.IsItem())
	{
		ItemPrototype const* proto = sObjectMgr.GetItemPrototype(guid.GetEntry());
		
		if (proto)
		{
			if (type == LOOT_CORPSE)
				lTemplate = LootTemplates_Item.GetLootFor(proto->ItemId);
			else if (type == LOOT_DISENCHANTING && proto->DisenchantID)
				lTemplate = LootTemplates_Disenchant.GetLootFor(proto->DisenchantID);
#ifdef MANGOSBOT_TWO
			if (type == LOOT_MILLING)
				lTemplate = LootTemplates_Milling.GetLootFor(proto->ItemId);
			if (type == LOOT_PROSPECTING)
				lTemplate = LootTemplates_Prospecting.GetLootFor(proto->ItemId);
#endif
		}
	}

	LootTemplateAccess const* lTemplateA = reinterpret_cast<LootTemplateAccess const*>(lTemplate);

	return lTemplateA;
}

LootMap* LootMapValue::Calculate()
{
	LootMap* lootMap = new LootMap;

	int32 sEntry;

	for (uint32 entry = 0; entry < sCreatureStorage.GetMaxEntry(); entry++)
	{
		sEntry = entry;

		LootTemplateAccess const* lTemplateA = GetLootTemplate(ObjectGuid(HIGHGUID_UNIT, entry, uint32(1)), LOOT_CORPSE);

		if(lTemplateA)
			for (LootStoreItem const& lItem : lTemplateA->Entries)
				lootMap->insert({ lItem.itemid,sEntry });
	}

	for (uint32 entry = 0; entry < sGOStorage.GetMaxEntry(); entry++)
	{
		sEntry = entry;

		LootTemplateAccess const* lTemplateA = GetLootTemplate(ObjectGuid(HIGHGUID_GAMEOBJECT, entry, uint32(1)), LOOT_CORPSE);

		if(lTemplateA)
			for (LootStoreItem const& lItem : lTemplateA->Entries)
				lootMap->insert({ lItem.itemid,-sEntry });
	}

	return lootMap;
}

//What items does this entry have in its loot list?
list<uint32> EntryLootListValue::Calculate()
{
	int32 entry = stoi(getQualifier());

	list<uint32> items;

	LootMap* lootMap = GAI_VALUE(LootMap*, "loot map");

	auto range = lootMap->right.equal_range(entry);

	for (auto itr = range.first; itr != range.second; ++itr)
		items.push_back(itr->second);

	return items;
}

//What items does this entry have in its loot list?
list<int32> ItemLootListValue::Calculate()
{
	uint32 itemId = stoi(getQualifier());

	LootMap* lootMap = GAI_VALUE(LootMap*, "loot map");

	list<int32> entries;

	auto range = lootMap->left.equal_range(itemId);

	for (auto itr = range.first; itr != range.second; ++itr)
		entries.push_back(itr->second);

	return entries;
}
