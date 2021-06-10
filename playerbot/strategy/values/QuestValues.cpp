#include "botpch.h"
#include "../../playerbot.h"
#include "QuestValues.h"
#include "SharedValueContext.h"

using namespace ai;

//Get all the objective entries for a specific quest.
void FindQuestObjectData::GetObjectiveEntries()
{
	ObjectMgr::QuestMap const& questMap = sObjectMgr.GetQuestTemplates();

	for (auto& quest : questMap)
	{
		for (uint32 objective = 0; objective < QUEST_OBJECTIVES_COUNT; objective++)
		{
			vector<uint32> entries;

			if (quest.second->ReqCreatureOrGOId[objective] && quest.second->ReqCreatureOrGOCount[objective] > 0)
				entryMap[quest.second->ReqCreatureOrGOId[objective]].push_back(make_pair(quest.first, QuestRelationType(objective)));
		
			if (quest.second->ReqItemId[objective] && quest.second->ReqItemCount[objective] > 0)
				itemMap[quest.second->ReqItemId[objective]].push_back(make_pair(quest.first, QuestRelationType(objective)));
		}
	}
}

//Data worker. Checks for a specific creature what quest they are needed for and puts them in the proper place in the quest map.
bool FindQuestObjectData::operator()(CreatureDataPair const& dataPair)
{
	uint32 entry = dataPair.second.id;

	QuestRelationsMapBounds	rbounds = sObjectMgr.GetCreatureQuestRelationsMapBounds(entry);

	for (QuestRelationsMap::const_iterator itr = rbounds.first; itr != rbounds.second; ++itr)
	{
		data[itr->second][QuestRelationType::questGiver].push_back(GuidPosition(&dataPair));
	}

	QuestRelationsMapBounds	ibounds = sObjectMgr.GetCreatureQuestInvolvedRelationsMapBounds(entry);

	for (QuestRelationsMap::const_iterator itr = ibounds.first; itr != ibounds.second; ++itr)
	{
		data[itr->second][QuestRelationType::questTaker].push_back(GuidPosition(&dataPair));
	}

	for (auto entryPair : entryMap[entry])
	{
		data[entryPair.first][entryPair.second].push_back(GuidPosition(&dataPair));
	}

	CreatureInfo const* info = sObjectMgr.GetCreatureTemplate(entry);
	
	if (!info || info->LootId == 0)
		return false;

	LootTemplate const* lTemplate = LootTemplates_Creature.GetLootFor(info->LootId);

	if (!lTemplate)
		return false;

	LootTemplateAccess const* lTemplateA = reinterpret_cast<LootTemplateAccess const*>(lTemplate);

	for (auto& item : lTemplateA->Entries)
	{
		for (auto& itemPair : itemMap[item.itemid])
		{
			data[itemPair.first][itemPair.second].push_back(GuidPosition(&dataPair));
		}
	}

	return false;
}


//GameObject data worker. Checks for a specific gameObject what quest they are needed for and puts them in the proper place in the quest map.
bool FindQuestObjectData::operator()(GameObjectDataPair const& dataPair)
{
	int32 entry = dataPair.second.id;

	QuestRelationsMapBounds	rbounds = sObjectMgr.GetGOQuestRelationsMapBounds(entry);

	for (QuestRelationsMap::const_iterator itr = rbounds.first; itr != rbounds.second; ++itr)
	{
		data[itr->second][QuestRelationType::questGiver].push_back(GuidPosition(&dataPair));
	}

	QuestRelationsMapBounds	ibounds = sObjectMgr.GetGOQuestInvolvedRelationsMapBounds(entry);

	for (QuestRelationsMap::const_iterator itr = ibounds.first; itr != ibounds.second; ++itr)
	{
		data[itr->second][QuestRelationType::questTaker].push_back(GuidPosition(&dataPair));
	}

	for (auto entryPair : entryMap[entry * -1]) //GameObjectData entry is negative in quest objectives.
	{
		data[entryPair.first][entryPair.second].push_back(GuidPosition(&dataPair));
	}

	GameObjectInfo const* info = sObjectMgr.GetGameObjectInfo(entry);

	if (!info || info->GetLootId() == 0)
		return false;

	LootTemplate const* lTemplate = LootTemplates_Gameobject.GetLootFor(info->GetLootId());

	if (!lTemplate)
		return false;

	LootTemplateAccess const* lTemplateA = reinterpret_cast<LootTemplateAccess const*>(lTemplate);

	for (auto& item : lTemplateA->Entries)
	{
		for (auto& itemPair : itemMap[item.itemid])
		{
			data[itemPair.first][itemPair.second].push_back(GuidPosition(&dataPair));
		}
	}

	return false;
}

//Goes past all creatures and gameobjects and creatures the full quest guid map.
questGuidpMap QuestGuidpMapValue::Calculate()
{
	FindQuestObjectData worker;
	sObjectMgr.DoCreatureData(worker);
	sObjectMgr.DoGOData(worker);
	return worker.GetResult();
}

//Selects all questgivers for a specific level (range).
questGivers QuestGiversValue::Calculate()
{
	int level = 0;
	string q = getQualifier();
	bool hasQualifier = !q.empty();

	if (hasQualifier)
		level = stoi(q);

	questGuidpMap questMap = GAI_VALUE(questGuidpMap, "quest guidp map");

	questGivers guidps;

	for (auto& qPair : questMap)
	{
		for (auto& guidp : qPair.second[QuestRelationType::questGiver])
		{
			uint32 questId = qPair.first;

			if (hasQualifier)
			{
				Quest const* quest = sObjectMgr.GetQuestTemplate(questId);

				if (quest && (level < quest->GetMinLevel() || level > quest->GetQuestLevel() + 10))
					continue;
			}

			guidps[questId].push_back(guidp);
		}
	}

	return guidps;
}

list<GuidPosition> ActiveQuestGiversValue::Calculate()
{
	questGivers qGivers = GAI_VALUE2(questGivers, "quest givers", bot->getLevel());

	list<GuidPosition> retQuestGivers;

	for (auto& qGiver : qGivers)
	{
		uint32 questId = qGiver.first;
		Quest const* quest = sObjectMgr.GetQuestTemplate(questId);

		if (!quest || !quest->IsActive())
		{
			continue;
		}

		if (!bot->CanTakeQuest(quest, false))
			continue;

		QuestStatus status = bot->GetQuestStatus(questId);

		if (status != QUEST_STATUS_NONE)
			continue;

		for (auto& guidp : qGiver.second)
		{
			CreatureInfo const* creatureInfo = guidp.GetCreatureTemplate();

			if (creatureInfo)
			{
				if (!ai->IsFriendlyTo(creatureInfo->Faction))
					continue;
			}

			if (guidp.isDead())
				continue;

			retQuestGivers.push_back(guidp);
		}
	}

	return retQuestGivers;
}

list<GuidPosition> ActiveQuestTakersValue::Calculate()
{
	questGuidpMap questMap = GAI_VALUE(questGuidpMap, "quest guidp map");

	list<GuidPosition> retQuestTakers;

	QuestStatusMap& questStatusMap = bot->getQuestStatusMap();

	for (auto& questStatus : questStatusMap)
	{
		uint32 questId = questStatus.first;

		Quest const* quest = sObjectMgr.GetQuestTemplate(questId);

		if (!quest || !quest->IsActive())
		{
			continue;
		}

		QuestStatus status = questStatus.second.m_status;

		if ((status != QUEST_STATUS_COMPLETE || bot->GetQuestRewardStatus(questId)) && (!quest->IsAutoComplete() || !bot->CanTakeQuest(quest, false)))
			continue;

		auto q = questMap.find(questId);

		if (q == questMap.end())
			continue;

		auto qt = q->second.find(QuestRelationType::questTaker);

		if (qt == q->second.end())
			continue;		

		for (auto& guidp : qt->second)
		{
			CreatureInfo const* creatureInfo = guidp.GetCreatureTemplate();

			if (creatureInfo)
			{
				if (!ai->IsFriendlyTo(creatureInfo->Faction))
					continue;
			}

			if (guidp.isDead())
				continue;

			retQuestTakers.push_back(guidp);
		}
	}

	return retQuestTakers;
}

list<GuidPosition> ActiveQuestObjectivesValue::Calculate()
{
	questGuidpMap questMap = GAI_VALUE(questGuidpMap, "quest guidp map");

	list<GuidPosition> retQuestObjectives;

	QuestStatusMap& questStatusMap = bot->getQuestStatusMap();

	for (auto& questStatus : questStatusMap)
	{
		uint32 questId = questStatus.first;

		Quest const* quest = sObjectMgr.GetQuestTemplate(questId);

		if (!quest || !quest->IsActive())
		{
			continue;
		}

		QuestStatusData statusData = questStatus.second;

		if (statusData.m_status != QUEST_STATUS_INCOMPLETE)
			continue;

		for (uint32 objective = 0; objective < QUEST_OBJECTIVES_COUNT; objective++)
		{
			if (quest->ReqItemCount[objective])
			{
				uint32  reqCount = quest->ReqItemCount[objective];
				uint32  hasCount = statusData.m_itemcount[objective];

				if (!reqCount || hasCount >= reqCount)
					continue;
			}
			else
			{
				uint32 reqCount = quest->ReqCreatureOrGOCount[objective];
				uint32 hasCount = statusData.m_creatureOrGOcount[objective];

				if (!reqCount || hasCount >= reqCount)
					continue;
			}

			auto q = questMap.find(questId);

			if (q == questMap.end())
				continue;

			auto qt = q->second.find((QuestRelationType)objective);

			if (qt == q->second.end())
				continue;

			for (auto& guidp : qt->second)
			{
				if (guidp.isDead())
					continue;

				retQuestObjectives.push_back(guidp);
			}
		}
	}

	return retQuestObjectives;
}

uint8 FreeQuestLogSlotValue::Calculate()
{
	uint8 numQuest = 0;
	for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
	{
		uint32 questId = bot->GetQuestSlotQuestId(slot);

		if (!questId)
			continue;

		Quest const* quest = sObjectMgr.GetQuestTemplate(questId);
		if (!quest)
			continue;

		numQuest++;
	}

	return MAX_QUEST_LOG_SIZE - numQuest;
}