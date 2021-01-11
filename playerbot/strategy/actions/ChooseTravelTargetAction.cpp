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

    TravelTarget newTarget = ChooseTarget(target);

    if (!newTarget.isActive())
       return false;

    ReportTravelTarget(&newTarget, target);

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

        if (destination->getEntry() < 0 && gInfo)
            out << " to " << gInfo->name;
        if (destination->getEntry() > 0 && cInfo)
            out << " to " << cInfo->Name;

        out << " for";

        if (destination->getName() == "QuestObjectiveTravelDestination")
            out << " doing ";
        else {
            QuestRelationTravelDestination* rel = (QuestRelationTravelDestination*)newTarget->getDestination();

            if (rel->getRelation() == 0)
                out << " picking up ";
            else
                out << " handing in ";
        }

        out << "[" << quest->GetTitle() << "]";

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

TravelTarget ChooseTravelTargetAction::ChooseTarget(TravelTarget* oldTarget)
{
    TravelTarget target = GetGroupTarget();       //Join groups members

    if (!target.isActive() && urand(1,100) > 10)  //90% chance 
        target = GetCurrentTarget(oldTarget);     //Extend current target.

    if (!target.isActive() && urand(1, 100) > 5)  //95% chance
        target = GetQuestTarget();                //Do a target of an active quest.

    if (!target.isActive())
        target = GetNewQuestTarget();             //Find a new quest to do.

    if (!target.isActive())
        target = GetNullTarget();                 //Idle a bit.

    return target;
}

TravelTarget ChooseTravelTargetAction::getBestTarget(vector<TravelDestination*> activeDestinations, vector<WorldPosition*> activePoints, bool groupCopy)
{
    if (activeDestinations.empty() || activePoints.empty()) //No targets or no points.
        return TravelTarget(ai);    

    WorldPosition botLocation(bot);
    vector<WorldPosition*> availablePoints = sTravelMgr.getNextPoint(&botLocation, activePoints); //Pick a good point.

    if (availablePoints.empty()) //No points available.
        return TravelTarget(ai);

    TravelDestination* targetDestination;

    for (auto activeTarget : activeDestinations) //Pick the destination that has this point.
        if (activeTarget->distanceTo(availablePoints.front()) == 0)
            targetDestination = activeTarget;

    TravelTarget travelTarget(ai, targetDestination, availablePoints.front(), groupCopy);

    return travelTarget;

}

TravelTarget ChooseTravelTargetAction::GetGroupTarget()
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
    for (auto member : groupPlayers)
    {
        Player* player = sObjectMgr.GetPlayer(member);

        if (!player)
            continue;

        if (!player->GetPlayerbotAI())
            continue;

        if (!player->GetPlayerbotAI()->GetAiObjectContext())
            continue;

        TravelTarget* target = player->GetPlayerbotAI()->GetAiObjectContext()->GetValue<TravelTarget*>("travel target")->Get();

        if (target->isGroupCopy())
            continue;

        if (!target->isActive())
            continue;

        if (!target->getDestination()->isActive(bot))
            continue;

        activeDestinations.push_back(target->getDestination());
        activePoints.push_back(target->getPosition());
    }

    return getBestTarget(activeDestinations, activePoints, true);
}

TravelTarget ChooseTravelTargetAction::GetCurrentTarget(TravelTarget* oldTarget)
{
    TravelTarget travelTarget(ai);

    TravelDestination* oldDestination = oldTarget->getDestination();

    if (!oldDestination) //Does this target have a destination?
        return travelTarget;

    if (!oldDestination->isActive(bot)) //Is the destination still valid?
        return travelTarget;

    WorldPosition botLocation(bot);
    vector<WorldPosition*> availablePoints = oldDestination->nextPoint(&botLocation);

    if (availablePoints.empty())
        return travelTarget;

    travelTarget = TravelTarget(ai, oldTarget->getDestination(), oldTarget->getPosition(), oldTarget->isGroupCopy());
    travelTarget.setStatus(TRAVEL_STATUS_TRAVEL);

    return travelTarget;
}

TravelTarget ChooseTravelTargetAction::GetQuestTarget()
{
    vector<TravelDestination*> activeDestinations;
    vector<WorldPosition*> activePoints;

    QuestStatusMap& questMap = bot->getQuestStatusMap();

    WorldPosition botLocation(bot);

    //Find destinations related to the active quests.
    for (auto quest : questMap)
    {
        uint32 questId = quest.first;
        QuestStatusData* questStatus = &quest.second;

        vector<QuestTravelDestination*> TravelDestinations = sTravelMgr.getQuestTravelDestinations(bot, questId, ai->GetMaster());

        activeDestinations.insert(activeDestinations.end(), TravelDestinations.begin(), TravelDestinations.end());
    }

    //Pick one good point per destination.
    for (auto activeTarget : activeDestinations)
    {
        vector<WorldPosition*> points = activeTarget->nextPoint(&botLocation);
        if (!points.empty())
            activePoints.push_back(points.front());
    }

    return getBestTarget(activeDestinations, activePoints);
}

TravelTarget ChooseTravelTargetAction::GetNewQuestTarget()
{
    vector<TravelDestination*> activeDestinations;
    vector<WorldPosition*> activePoints;

    WorldPosition botLocation(bot);

    //Find quest givers.
    vector<QuestTravelDestination*> TravelDestinations = sTravelMgr.getQuestTravelDestinations(bot, -1, ai->GetMaster());

    activeDestinations.insert(activeDestinations.end(), TravelDestinations.begin(), TravelDestinations.end());

    //Pick one good point per destination.
    for (auto activeTarget : activeDestinations)
    {
        vector<WorldPosition*> points = activeTarget->nextPoint(&botLocation);
        if (!points.empty())
            activePoints.push_back(points.front());
    }

    return getBestTarget(activeDestinations, activePoints);
}

TravelTarget ChooseTravelTargetAction::GetNullTarget()
{
    return TravelTarget(ai, sTravelMgr.nullTravelDestination, sTravelMgr.nullWorldPosition, true);
}

bool ChooseTravelTargetAction::isUseful()
{
    return !context->GetValue<TravelTarget *>("travel target")->Get()->isActive() && !context->GetValue<LootObject>("loot target")->Get().IsLootPossible(bot);
}


bool ChooseTravelTargetAction::needForQuest(Unit* target)
{
    bool justCheck = (bot->GetObjectGuid() == target->GetObjectGuid());

    QuestStatusMap& questMap = bot->getQuestStatusMap();
    for (auto & quest : questMap)
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
