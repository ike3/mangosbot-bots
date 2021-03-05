#include "botpch.h"
#include "../../playerbot.h"
#include "../../LootObjectStack.h"
#include "ChooseTravelTargetAction.h"
#include "../../PlayerbotAIConfig.h"

using namespace ai;

bool ChooseTravelTargetAction::Execute(Event event)
{
    TravelTarget * target = context->GetValue<TravelTarget *>("travel target")->Get();
    bool addLoot = ai->DoSpecificAction("add all loot", Event(), true);

    if (context->GetValue<LootObject>("loot target")->Get().IsLootPossible(bot))
    {
        target->setStatus(TRAVEL_STATUS_PREPARE);
        return false;
    }

    TravelTarget newTarget = TravelTarget(ai);
    
    SetTarget(&newTarget, target);

    if (!newTarget.isActive())
       return false;

    ReportTravelTarget(&newTarget, target);

    if (target && target == &newTarget && newTarget.getEntry())
    {
        set<ObjectGuid>& ignoreList = context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Get();
        
        for (auto& i : ignoreList)
        {
            if (i.GetEntry() == newTarget.getEntry())
            {
                ignoreList.erase(i);
            }
        }

        context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Set(ignoreList);
    }

    target->copyTarget(&newTarget);

    if (ai->GetMaster() && target->isActive() && target->getDestination()->getName() == "NullTravelDestination")
        target->setExpireIn(10 * 1000);

    context->GetValue<ObjectGuid>("rpg target")->Set(ObjectGuid());
    context->GetValue<ObjectGuid>("pull target")->Set(ObjectGuid());

    return true;
}

void ChooseTravelTargetAction::ReportTravelTarget(TravelTarget* newTarget, TravelTarget* oldTarget)
{
    TravelDestination* destination = newTarget->getDestination();

    TravelDestination* oldDestination = oldTarget->getDestination();

    if (destination->getName() == "QuestRelationTravelDestination" || destination->getName() == "QuestObjectiveTravelDestination")
    {
        QuestTravelDestination* QuestDestination = (QuestTravelDestination*)destination;
        Quest const* quest = QuestDestination->GetQuestTemplate();
        WorldPosition botLocation(bot);
        CreatureInfo const* cInfo = NULL;
        GameObjectInfo const* gInfo = NULL;

        if (destination->getEntry() > 0)
            cInfo = ObjectMgr::GetCreatureTemplate(destination->getEntry());
        else
            gInfo = ObjectMgr::GetGameObjectInfo(destination->getEntry() * -1);

        string Sub;

        ostringstream out;

        if (newTarget->isGroupCopy())
            out << "Following group ";
        else if(oldDestination && oldDestination == destination)
            out << "Continuing ";
        else
            out << "Traveling ";

        out << round(newTarget->getDestination()->distanceTo(&botLocation));

        out << " for " << chat->formatQuest(quest);

        out << " to " << QuestDestination->getTitle();

        ai->TellMaster(out);
    }
    else if (destination->getName() == "NullTravelDestination")
    {
        if (!oldTarget->getDestination() || oldTarget->getDestination()->getName() != "NullTravelDestination")
        {
            ai->TellMaster("No where to travel. Idling a bit.");
        }
    }
}

bool ChooseTravelTargetAction::SetTarget(TravelTarget* target, TravelTarget* oldTarget)
{
    bool foundTarget = false;

    foundTarget = SetGroupTarget(target);                  //Join groups members

    if (!foundTarget && urand(1, 100) > 10)                //90% chance 
        foundTarget = SetCurrentTarget(target, oldTarget); //Extend current target.

    if (!foundTarget && urand(1, 100) > 5)                 //95% chance
        foundTarget = SetQuestTarget(target);              //Do a target of an active quest.

    if (!foundTarget)
        foundTarget = SetNewQuestTarget(target);           //Find a new quest to do.

    if (!foundTarget)
        SetNullTarget(target);                             //Idle a bit.

    return target;
}

bool ChooseTravelTargetAction::getBestDestination(vector<TravelDestination*> activeDestinations, vector<WorldPosition*> activePoints)
{
    if (activeDestinations.empty() || activePoints.empty()) //No targets or no points.
        return false;

    WorldPosition botLocation(bot);
    vector<WorldPosition*> availablePoints = sTravelMgr.getNextPoint(&botLocation, activePoints); //Pick a good point.

    if (availablePoints.empty()) //No points available.
        return false;

    TravelDestination* targetDestination;

    for (auto activeTarget : activeDestinations) //Pick the destination that has this point.
        if (activeTarget->distanceTo(availablePoints.front()) == 0)
            targetDestination = activeTarget;

    if (!targetDestination)
        return false;

    activeDestinations.clear();
    activePoints.clear();

    activeDestinations.push_back(targetDestination);
    activePoints.push_back(availablePoints.front());

    return true;
}

bool ChooseTravelTargetAction::SetGroupTarget(TravelTarget* target)
{
    vector<TravelDestination*> activeDestinations;
    vector<WorldPosition*> activePoints;

    list<ObjectGuid> groupPlayers;

    Group* group = bot->GetGroup();
    if (group)
    {
        for (GroupReference* ref = group->GetFirstMember(); ref; ref = ref->next())
        {
            if (ref->getSource() != bot)
            {
                if (ref->getSubGroup() != bot->GetSubGroup())
                {
                    groupPlayers.push_back(ref->getSource()->GetObjectGuid());
                }
                else
                {
                    groupPlayers.push_front(ref->getSource()->GetObjectGuid());
                }
            }
        }
    }

    //Find targets of the group.
    for (auto& member : groupPlayers)
    {
        Player* player = sObjectMgr.GetPlayer(member);

        if (!player)
            continue;

        if (!player->GetPlayerbotAI())
            continue;

        if (!player->GetPlayerbotAI()->GetAiObjectContext())
            continue;

        TravelTarget* groupTarget = player->GetPlayerbotAI()->GetAiObjectContext()->GetValue<TravelTarget*>("travel target")->Get();

        if (groupTarget->isGroupCopy())
            continue;

        if (!groupTarget->isActive())
            continue;

        if (!groupTarget->getDestination()->isActive(bot))
            continue;

        activeDestinations.push_back(groupTarget->getDestination());
        activePoints.push_back(groupTarget->getPosition());
    }

    if (!getBestDestination(activeDestinations, activePoints))
        return false;

    target->setTarget(activeDestinations.front(), activePoints.front(), true);

    return target->isActive();
}

bool ChooseTravelTargetAction::SetCurrentTarget(TravelTarget* target, TravelTarget* oldTarget)
{
    TravelDestination* oldDestination = oldTarget->getDestination();

    if (!oldDestination) //Does this target have a destination?
        return false;

    if (!oldDestination->isActive(bot)) //Is the destination still valid?
        return false;

    WorldPosition botLocation(bot);
    vector<WorldPosition*> availablePoints = oldDestination->nextPoint(&botLocation);

    if (availablePoints.empty())
        return false;

    target->setTarget(oldTarget->getDestination(), availablePoints.front());
    target->setStatus(TRAVEL_STATUS_TRAVEL);

    return target->isActive();
}

bool ChooseTravelTargetAction::SetQuestTarget(TravelTarget* target)
{
    vector<TravelDestination*> activeDestinations;
    vector<WorldPosition*> activePoints;

    QuestStatusMap& questMap = bot->getQuestStatusMap();

    WorldPosition botLocation(bot);

    //Find destinations related to the active quests.
    for (auto& quest : questMap)
    {
        if (quest.second.m_rewarded)
            continue;

        uint32 questId = quest.first;
        QuestStatusData* questStatus = &quest.second;

        vector<TravelDestination*> questDestinations = sTravelMgr.getQuestTravelDestinations(bot, questId, ai->GetMaster(), false, 2000,  AI_VALUE(uint8, "bag space") > 80);
        vector< WorldPosition*> questPoints;
        
        for (auto& questDestination : questDestinations)
        {
            vector< WorldPosition*> destinationPoints = questDestination->getPoints();
            if (!destinationPoints.empty())
                questPoints.insert(questPoints.end(), destinationPoints.begin(), destinationPoints.end());
        }

        if (getBestDestination(questDestinations, questPoints))
        {
            activeDestinations.push_back(questDestinations.front());
            activePoints.push_back(questPoints.front());
        }       
        
    }

    if (!getBestDestination(activeDestinations, activePoints))
        return false;

    target->setTarget(activeDestinations.front(), activePoints.front());

    return target->isActive();
}

bool ChooseTravelTargetAction::SetNewQuestTarget(TravelTarget* target)
{
    vector<TravelDestination*> activeDestinations;
    vector<WorldPosition*> activePoints;

    WorldPosition botLocation(bot);

    //Find quest givers.
    vector<TravelDestination*> TravelDestinations = sTravelMgr.getQuestTravelDestinations(bot, -1, ai->GetMaster());

    activeDestinations.insert(activeDestinations.end(), TravelDestinations.begin(), TravelDestinations.end());

    //Pick one good point per destination.
    for (auto& activeTarget : activeDestinations)
    {
        vector<WorldPosition*> points = activeTarget->nextPoint(&botLocation);
        if (!points.empty())
            activePoints.push_back(points.front());
    }

    if (!getBestDestination(activeDestinations, activePoints))
        return false;

    target->setTarget(activeDestinations.front(), activePoints.front());

    return target->isActive();
}

bool ChooseTravelTargetAction::SetNullTarget(TravelTarget* target)
{
    target->setTarget(sTravelMgr.nullTravelDestination, sTravelMgr.nullWorldPosition, true);
    
    return true;
}

bool ChooseTravelTargetAction::isUseful()
{
    return !context->GetValue<TravelTarget *>("travel target")->Get()->isActive() 
        && !context->GetValue<LootObject>("loot target")->Get().IsLootPossible(bot)
        && ai->AllowActive(TRAVEL_ACTIVITY);
}


bool ChooseTravelTargetAction::needForQuest(Unit* target)
{
    bool justCheck = (bot->GetObjectGuid() == target->GetObjectGuid());

    QuestStatusMap& questMap = bot->getQuestStatusMap();
    for (auto& quest : questMap)
    {
        const Quest* questTemplate = sObjectMgr.GetQuestTemplate(quest.first);
        if (!questTemplate)
            continue;

        uint32 questId = questTemplate->GetQuestId();

        if (!questId)
            continue;

        QuestStatus status = bot->GetQuestStatus(questId);

        if ((status == QUEST_STATUS_COMPLETE && !bot->GetQuestRewardStatus(questId)))
        {
            if (!justCheck && !target->HasInvolvedQuest(questId))
                continue;

            return true;
        }
        else if (status == QUEST_STATUS_INCOMPLETE)
        {
            QuestStatusData questStatus = quest.second;

            if (questTemplate->GetQuestLevel() > bot->getLevel())
                continue;

            for (int j = 0; j < QUEST_OBJECTIVES_COUNT; j++)
            {
                int32 entry = questTemplate->ReqCreatureOrGOId[j];

                if (entry && entry > 0)
                {
                    int required = questTemplate->ReqCreatureOrGOCount[j];
                    int available = questStatus.m_creatureOrGOcount[j];

                    if(required && available < required && (target->GetEntry() == entry || justCheck))
                        return true;
                }         

                if (justCheck)
                {
                    int32 itemId = questTemplate->ReqItemId[j];

                    if (itemId && itemId > 0)
                    {
                        int required = questTemplate->ReqItemCount[j];
                        int available = questStatus.m_itemcount[j];

                        if (required && available < required)
                            return true;
                    }
                }
            }

            if (!justCheck)
            {
                CreatureInfo const* data = sObjectMgr.GetCreatureTemplate(target->GetEntry());

                if (data)
                {
                    uint32 lootId = data->LootId;

                    if (lootId)
                    {
                        if (LootTemplates_Creature.HaveQuestLootForPlayer(lootId, bot))
                            return true;
                    }
                }
            }
        }

    }
    return false;
}

bool ChooseTravelTargetAction::needItemForQuest(uint32 itemId, const Quest* questTemplate, const QuestStatusData* questStatus)
{
    for (int i = 0; i < QUEST_OBJECTIVES_COUNT; i++)
    {
        if (questTemplate->ReqItemId[i] != itemId)
            continue;

        int required = questTemplate->ReqItemCount[i];
        int available = questStatus->m_itemcount[i];

        if (!required)
            continue;

        return available < required;
    }

    return false;
}
