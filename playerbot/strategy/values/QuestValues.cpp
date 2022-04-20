#include "botpch.h"
#include "../../playerbot.h"
#include "QuestValues.h"
#include "SharedValueContext.h"

using namespace ai;


//What kind of a relation does this entry have with this quest.
entryQuestRelationMap EntryQuestRelationMapValue::Calculate()
{
	entryQuestRelationMap rMap;

	//Quest givers takers
	QuestObjectMgr* questObjectMgr = (QuestObjectMgr*)&sObjectMgr;

	for (auto relation : questObjectMgr->GetCreatureQuestRelationsMap())
		rMap[relation.first][relation.second] |= (int)QuestRelationFlag::questGiver;

	for (auto relation : questObjectMgr->GetCreatureQuestInvolvedRelationsMap())
		rMap[relation.first][relation.second] |= (int)QuestRelationFlag::questTaker;

	for (auto relation : questObjectMgr->GetGOQuestRelationsMap())
		rMap[-(int32)relation.first][relation.second] |= (int)QuestRelationFlag::questGiver;

	for (auto relation : questObjectMgr->GetGOQuestInvolvedRelationsMap())
		rMap[-(int32)relation.first][relation.second] |= (int)QuestRelationFlag::questGiver;

	//Quest objectives
	ObjectMgr::QuestMap const& questMap = sObjectMgr.GetQuestTemplates();

	for (auto& questItr : questMap)
	{
		uint32 questId = questItr.first;
		Quest* quest = questItr.second;

		for (uint32 objective = 0; objective < QUEST_OBJECTIVES_COUNT; objective++)
		{
			uint32 relationFlag = 1 << objective;

			//Kill objective
			if (quest->ReqCreatureOrGOId[objective])
				rMap[quest->ReqCreatureOrGOId[objective]][questId] |= relationFlag;

			//Loot objective
			if (quest->ReqItemId[objective])
			{
				for (auto& entry : GAI_VALUE2(list<int32>, "item drop list", quest->ReqItemId[objective]))
					rMap[entry][questId] |= relationFlag;
			}
		}

		//Target entry of source item of quest. 
		if (quest->GetSrcItemId())
		{
			ItemRequiredTargetMapBounds bounds = sObjectMgr.GetItemRequiredTargetMapBounds(quest->GetSrcItemId());

			if (bounds.first != bounds.second) //Add target of source item to second quest objective.
				for (ItemRequiredTargetMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
					rMap[itr->second.m_uiTargetEntry][questId] |= (int)QuestRelationFlag::objective2;
		}
	}

	return rMap;
}


//Get all the objective entries for a specific quest.
void FindQuestObjectData::GetObjectiveEntries()
{
	relationMap = GAI_VALUE(entryQuestRelationMap, "entry quest relation");
}

//Data worker. Checks for a specific creature what quest they are needed for and puts them in the proper place in the quest map.
bool FindQuestObjectData::operator()(CreatureDataPair const& dataPair)
{
	uint32 entry = dataPair.second.id;

	for (auto& relation : relationMap[entry])
	{
		uint32 questId = relation.first;
		uint32 flag = relation.second;

		data[questId][flag][entry].push_back(GuidPosition(&dataPair));
	}

	return false;
}


//GameObject data worker. Checks for a specific gameObject what quest they are needed for and puts them in the proper place in the quest map.
bool FindQuestObjectData::operator()(GameObjectDataPair const& dataPair)
{
	int32 entry = dataPair.second.id * -1;

	for (auto& relation : relationMap[entry])
	{
		uint32 questId = relation.first;
		uint32 flag = relation.second;

		data[questId][flag][entry].push_back(GuidPosition(&dataPair));
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
questGiverMap QuestGiversValue::Calculate()
{
	uint32 level = 0;
	string q = getQualifier();
	bool hasQualifier = !q.empty();

	if (hasQualifier)
		level = stoi(q);

	questGuidpMap questMap = GAI_VALUE(questGuidpMap, "quest guidp map");

	questGiverMap guidps;

	for (auto& qPair : questMap)
	{
		for (auto& entry : qPair.second[(int)QuestRelationFlag::questGiver])
		{
			for (auto& guidp : entry.second)
			{
				uint32 questId = qPair.first;

				if (hasQualifier)
				{
					Quest const* quest = sObjectMgr.GetQuestTemplate(questId);

					if (quest && (level < quest->GetMinLevel() || (int)level > quest->GetQuestLevel() + 10))
						continue;
				}

				guidps[questId].push_back(guidp);
			}
		}
	}

	return guidps;
}

list<GuidPosition> ActiveQuestGiversValue::Calculate()
{
	questGiverMap qGivers = GAI_VALUE2(questGiverMap, "quest givers", bot->GetLevel());

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

		auto qt = q->second.find((int)QuestRelationFlag::questTaker);

		if (qt == q->second.end())
			continue;		

		for (auto& entry : qt->second)
		{
			if (entry.first > 0)
			{
				CreatureInfo const* info = sObjectMgr.GetCreatureTemplate(entry.first);

				if (info)
				{
					if (!ai->IsFriendlyTo(info->Faction))
						continue;
				}
			}

			for (auto& guidp : entry.second)
			{
				if (guidp.isDead())
					continue;

				retQuestTakers.push_back(guidp);
			}
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

			if (quest->ReqCreatureOrGOCount[objective])
			{
				uint32 reqCount = quest->ReqCreatureOrGOCount[objective];
				uint32 hasCount = statusData.m_creatureOrGOcount[objective];

				if (!reqCount || hasCount >= reqCount)
					continue;
			}

			auto q = questMap.find(questId);

			if (q == questMap.end())
				continue;

			auto qt = q->second.find((int)QuestRelationFlag(1<<objective));

			if (qt == q->second.end())
				continue;

			for (auto& entry : qt->second)
			{
				for (auto& guidp : entry.second)
				{
					if (guidp.isDead())
						continue;

					retQuestObjectives.push_back(guidp);
				}
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

uint32 DialogStatusValue::getDialogStatus(Player* bot, int32 questgiver, uint32 questId)
{
	uint32 dialogStatus = DIALOG_STATUS_NONE;

	QuestRelationsMapBounds rbounds;                        // QuestRelations (quest-giver)
	QuestRelationsMapBounds irbounds;                       // InvolvedRelations (quest-finisher)

	if (questgiver > 0)
	{
		rbounds = sObjectMgr.GetCreatureQuestRelationsMapBounds(questgiver);
		irbounds = sObjectMgr.GetCreatureQuestInvolvedRelationsMapBounds(questgiver);
	}
	else
	{
		rbounds = sObjectMgr.GetGOQuestRelationsMapBounds(questgiver * -1);
		irbounds = sObjectMgr.GetGOQuestInvolvedRelationsMapBounds(questgiver * -1);
	}

	// Check markings for quest-finisher
	for (QuestRelationsMap::const_iterator itr = irbounds.first; itr != irbounds.second; ++itr)
	{
		if (questId && itr->second != questId)
			continue;

		Quest const* pQuest = sObjectMgr.GetQuestTemplate(itr->second);

		uint32 dialogStatusNew = DIALOG_STATUS_NONE;

		if (!pQuest || !pQuest->IsActive())
		{
			continue;
		}

		QuestStatus status = bot->GetQuestStatus(itr->second);

		if ((status == QUEST_STATUS_COMPLETE && !bot->GetQuestRewardStatus(itr->second)) ||
			(pQuest->IsAutoComplete() && bot->CanTakeQuest(pQuest, false)))
		{
			if (pQuest->IsAutoComplete() && pQuest->IsRepeatable())
			{
				dialogStatusNew = DIALOG_STATUS_REWARD_REP;
			}
			else
			{
				dialogStatusNew = DIALOG_STATUS_REWARD2;
			}
		}
		else if (status == QUEST_STATUS_INCOMPLETE)
		{
			dialogStatusNew = DIALOG_STATUS_INCOMPLETE;
		}

		if (dialogStatusNew > dialogStatus)
		{
			dialogStatus = dialogStatusNew;
		}
	}

	// check markings for quest-giver
	for (QuestRelationsMap::const_iterator itr = rbounds.first; itr != rbounds.second; ++itr)
	{
		if (questId && itr->second != questId)
			continue;

		Quest const* pQuest = sObjectMgr.GetQuestTemplate(itr->second);

		uint32 dialogStatusNew = DIALOG_STATUS_NONE;

		if (!pQuest || !pQuest->IsActive())
		{
			continue;
		}

		QuestStatus status = bot->GetQuestStatus(itr->second);

		if (status == QUEST_STATUS_NONE)                    // For all other cases the mark is handled either at some place else, or with involved-relations already
		{
			if (bot->CanSeeStartQuest(pQuest))
			{
				if (bot->SatisfyQuestLevel(pQuest, false))
				{
					int32 lowLevelDiff = sWorld.getConfig(CONFIG_INT32_QUEST_LOW_LEVEL_HIDE_DIFF);
					if (pQuest->IsAutoComplete() || (pQuest->IsRepeatable() && bot->getQuestStatusMap()[itr->second].m_rewarded))
					{
						dialogStatusNew = DIALOG_STATUS_REWARD_REP;
					}
					else if (lowLevelDiff < 0 || bot->GetLevel() <= bot->GetQuestLevelForPlayer(pQuest) + uint32(lowLevelDiff))
					{
						dialogStatusNew = DIALOG_STATUS_AVAILABLE;
					}
					else
					{
#ifndef MANGOSBOT_TWO
						dialogStatusNew = DIALOG_STATUS_CHAT;
#else
						dialogStatusNew = DIALOG_STATUS_LOW_LEVEL_AVAILABLE;
#endif
					}
				}
				else
				{
					dialogStatusNew = DIALOG_STATUS_UNAVAILABLE;
				}
			}
		}

		if (dialogStatusNew > dialogStatus)
		{
			dialogStatus = dialogStatusNew;
		}
	}

	return dialogStatus;
}