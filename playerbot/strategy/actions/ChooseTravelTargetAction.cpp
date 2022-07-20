#include "botpch.h"
#include "../../playerbot.h"
#include "../../LootObjectStack.h"
#include "ChooseTravelTargetAction.h"
#include "../../PlayerbotAIConfig.h"

using namespace ai;

bool ChooseTravelTargetAction::Execute(Event event)
{
    //Get the current travel target. This target is no longer active.
    TravelTarget * oldTarget = context->GetValue<TravelTarget *>("travel target")->Get();

    //Select a new target to travel to. 
    TravelTarget newTarget = TravelTarget(ai);   
    getNewTarget(&newTarget, oldTarget);

    //If the new target is not active we failed.
    if (!newTarget.isActive())
       return false;    

    setNewTarget(&newTarget, oldTarget);

    return true;
}

//Select a new travel target.
//Currently this selectes mostly based on priority (current quest > new quest).
//This works fine because destinations can be full (max 15 bots per quest giver, max 1 bot per quest mob).
//
//Eventually we want to rewrite this to be more intelligent.
void ChooseTravelTargetAction::getNewTarget(TravelTarget* newTarget, TravelTarget* oldTarget)
{
    bool foundTarget = false;

    foundTarget = SetGroupTarget(newTarget);                                 //Join groups members

    //Enpty bags/repair
    if (!foundTarget && urand(1, 100) > 10)                                  //90% chance
        if (AI_VALUE2(bool, "group or", "should sell,can sell,following party,near leader") || AI_VALUE2(bool, "group or", "should repair,can repair,following party,near leader"))
            foundTarget = SetRpgTarget(newTarget);                           //Go to town to sell items or repair

    //Rpg in city
    if (!foundTarget && urand(1, 100) > 90 && bot->GetLevel() > 5)           //10% chance
        foundTarget = SetNpcFlagTarget(newTarget, { UNIT_NPC_FLAG_BANKER,UNIT_NPC_FLAG_BATTLEMASTER,UNIT_NPC_FLAG_AUCTIONEER });

    // PvP activities
    bool pvpActivate = false;
    if (pvpActivate && !foundTarget && urand(0, 4) && bot->GetLevel() > 50)
    {
        WorldPosition* botPos = &WorldPosition(bot);
        TravelTarget* target = context->GetValue<TravelTarget*>("travel target")->Get();

        TravelDestination* dest = ChooseTravelTargetAction::FindDestination(bot, "Tarren Mill");
        if (dest)
        {
            vector <WorldPosition*> points = dest->nextPoint(botPos, true);

            if (!points.empty())
            {
                target->setTarget(dest, points.front());
                target->setForced(true);

                ostringstream out; out << "Traveling to " << dest->getTitle();
                ai->TellMasterNoFacing(out.str());
                foundTarget = true;
            }
        }
    }

    //Grind for money
    if (!foundTarget && AI_VALUE(bool, "should get money"))
    {
        if (urand(1, 100) > 66) //Focus on active quests for money.
        {
            foundTarget = SetQuestTarget(newTarget, true);                    //Turn in quests for money.

            if (!foundTarget)
                foundTarget = SetQuestTarget(newTarget);                     //Do low level quests
        }
        else //Focus on mobs and new quests for money.
        {
            if (urand(1, 100) > 50)
                foundTarget = SetGrindTarget(newTarget);                         //Go grind mobs for money
            else
                foundTarget = SetNewQuestTarget(newTarget);                      //Find a low level quest to do

            if (!foundTarget)
                foundTarget = SetQuestTarget(newTarget, true);                   //Turn in quests for money.

            if (!foundTarget)
                foundTarget = SetQuestTarget(newTarget);                         //Do low level quests
        }
    }


    //Continue current target.
    if (!foundTarget && urand(1, 100) > 10)                               //90% chance 
        foundTarget = SetCurrentTarget(newTarget, oldTarget);             //Extend current target.

    //Dungeon in group.
    if (!foundTarget && urand(1, 100) > 50)                                 //50% chance
        if (AI_VALUE(bool, "can fight boss"))
            foundTarget = SetBossTarget(newTarget);                         //Go fight a (dungeon boss)

    //Continue current quests
    if (!foundTarget && urand(1, 100) > 5)                                 //95% chance
        foundTarget = SetQuestTarget(newTarget);                           //Do a target of an active quest.

    //Find a new quest to do.
    if (!foundTarget && urand(1, 100) > 5)                                 //95% chance
        foundTarget = SetNewQuestTarget(newTarget);                        //Find a new quest to do.

    //Explore a nearby unexplored area.
    if (!foundTarget && ai->HasStrategy("explore", BOT_STATE_NON_COMBAT))  //Explore a unexplored sub-zone.
        foundTarget = SetExploreTarget(newTarget);

    //Just hang with an npc
    if (!foundTarget && urand(1, 100) > 50)                                 //50% chance
    {
        foundTarget = SetRpgTarget(newTarget);
        if(foundTarget)
            newTarget->setForced(true);
    }

    if (!foundTarget)
        SetNullTarget(newTarget);                                           //Idle a bit.
}

void ChooseTravelTargetAction::setNewTarget(TravelTarget* newTarget, TravelTarget* oldTarget)
{
    //Tell the master where we are going.
    if (!bot->GetGroup() || (ai->GetGroupMaster() == bot))
        ReportTravelTarget(newTarget, oldTarget);

    //If we are heading to a creature/npc clear it from the ignore list. 
    if (oldTarget && oldTarget == newTarget && newTarget->getEntry())
    {
        set<ObjectGuid>& ignoreList = context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Get();

        for (auto& i : ignoreList)
        {
            if (i.GetEntry() == newTarget->getEntry())
            {
                ignoreList.erase(i);
            }
        }

        context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Set(ignoreList);
    }

    //Actually apply the new target to the travel target used by the bot.
    oldTarget->copyTarget(newTarget);

    //If we are idling but have a master. Idle only 10 seconds.
    if (ai->GetMaster() && oldTarget->isActive() && oldTarget->getDestination()->getName() == "NullTravelDestination")
        oldTarget->setExpireIn(10 * IN_MILLISECONDS);
    else if (oldTarget->isForced()) //Make sure travel goes into cooldown after getting to the destination.
        oldTarget->setExpireIn(HOUR * IN_MILLISECONDS);

    //Clear rpg and pull/grind target. We want to travel, not hang around some more.
    RESET_AI_VALUE(GuidPosition,"rpg target");
    RESET_AI_VALUE(ObjectGuid,"pull target");
};

//Tell the master what travel target we are moving towards.
//This should at some point be rewritten to be denser or perhaps logic moved to ->getTitle()
void ChooseTravelTargetAction::ReportTravelTarget(TravelTarget* newTarget, TravelTarget* oldTarget)
{
    TravelDestination* destination = newTarget->getDestination();

    TravelDestination* oldDestination = oldTarget->getDestination();

    ostringstream out;

    if (newTarget->isForced())
        out << "(Forced) ";

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

        if (newTarget->isGroupCopy())
            out << "Following group ";
        else if(oldDestination && oldDestination == destination)
            out << "Continuing ";
        else
            out << "Traveling ";

        out << round(newTarget->getDestination()->distanceTo(&botLocation)) << "y";

        out << " for " << chat->formatQuest(quest);

        out << " to " << QuestDestination->getTitle();

        ai->TellMaster(out);
    }
    else if (destination->getName() == "RpgTravelDestination")
    {
        RpgTravelDestination* RpgDestination = (RpgTravelDestination*)destination;

        WorldPosition botLocation(bot);

        if (newTarget->isGroupCopy())
            out << "Following group ";
        else if (oldDestination && oldDestination == destination)
            out << "Continuing ";
        else
            out << "Traveling ";

        out << round(newTarget->getDestination()->distanceTo(&botLocation)) << "y";

        out << " for ";

        if (AI_VALUE2(bool, "group or", "should sell,can sell"))
            out << "selling items";
        else if (AI_VALUE2(bool, "group or", "should repair,can repair"))
            out << "repairing";
        else
            out << "rpg";

        out << " to " << RpgDestination->getTitle();

        ai->TellMaster(out);
    }
    else if (destination->getName() == "ExploreTravelDestination")
    {
        ExploreTravelDestination* ExploreDestination = (ExploreTravelDestination*)destination;

        WorldPosition botLocation(bot);

        if (newTarget->isGroupCopy())
            out << "Following group ";
        else if (oldDestination && oldDestination == destination)
            out << "Continuing ";
        else
            out << "Traveling ";

        out << round(newTarget->getDestination()->distanceTo(&botLocation)) << "y";

        out << " for exploration";

        out << " to " << ExploreDestination->getTitle();

        ai->TellMaster(out);
    }
    else if (destination->getName() == "GrindTravelDestination")
    {
        GrindTravelDestination* GrindDestination = (GrindTravelDestination*)destination;

        WorldPosition botLocation(bot);

        if (newTarget->isGroupCopy())
            out << "Following group ";
        else if (oldDestination && oldDestination == destination)
            out << "Continuing ";
        else
            out << "Traveling ";

        out << round(newTarget->getDestination()->distanceTo(&botLocation)) << "y";

        out << " for grinding money";

        out << " to " << GrindDestination->getTitle();

        ai->TellMaster(out);
    }
    else if (destination->getName() == "BossTravelDestination")
    {
        BossTravelDestination* BossDestination = (BossTravelDestination*)destination;

        WorldPosition botLocation(bot);

        if (newTarget->isGroupCopy())
            out << "Following group ";
        else if (oldDestination && oldDestination == destination)
            out << "Continuing ";
        else
            out << "Traveling ";

        out << round(newTarget->getDestination()->distanceTo(&botLocation)) << "y";

        out << " for good loot";

        out << " to " << BossDestination->getTitle();

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

//Select only those destinations that are in sight distance or failing that a multiplication of the sight distance.
void ChooseTravelTargetAction::getLogicalDestinations(vector<TravelDestination*>& activeDestinations)
{   
    WorldPosition botLocation(bot);

    unordered_map <TravelDestination*, float> distances;

    for (auto dest : activeDestinations) //Calculate the distances to all the destinations.
        distances[dest] = dest->distanceTo(&botLocation);

    //Find the lowest distance.
    float minDistance = std::min_element(distances.begin(), distances.end(), [](std::pair< TravelDestination*, float> i, std::pair< TravelDestination*, float> j) { return i.second < j.second; })->second;

    vector<float> distancesToLookAt = { 1,4,10,100 }; //Multiplications of sightdistance.

    //Increase the minimal distance to a multiplication of sightdistance.
    for (float distanceToLookAt : distancesToLookAt)
        if (minDistance < sPlayerbotAIConfig.sightDistance * distanceToLookAt)
        {
            minDistance = sPlayerbotAIConfig.sightDistance * distanceToLookAt;
            break;
        }


    //Remove all destinations that fall outside this increased distance.
    auto it = activeDestinations.begin();

    while (it != activeDestinations.end())
        if (distances[*it] > minDistance)
            it = activeDestinations.erase(it);
        else
            ++it;
}

//Narrows down the destinations and points to the closest (random shuffle) ones.
bool ChooseTravelTargetAction::getBestDestination(vector<TravelDestination*>& activeDestinations, vector<WorldPosition*>& activePoints)
{
    if (activeDestinations.empty() || activePoints.empty()) //No targets or no points.
        return false;

    if (activeDestinations.size() == 1 && activePoints.size() == 1)
        return true;

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

//Overload to only narrow down destinations.
bool ChooseTravelTargetAction::getBestDestination(vector<TravelDestination*>& activeDestinations)
{
    WorldPosition botLocation(bot);

    vector<WorldPosition*> activePoints;

    for (auto& activeTarget : activeDestinations)
    {
        vector<WorldPosition*> points = activeTarget->nextPoint(&botLocation, true);
        if (!points.empty())
            activePoints.push_back(points.front());
    }

    return getBestDestination(activeDestinations, activePoints);
}

//Sets the target to the best destination.
bool ChooseTravelTargetAction::SetBestTarget(TravelTarget* target, vector<TravelDestination*>& TravelDestinations)
{
    if (TravelDestinations.empty())
        return false;

    //Narrow down the destinations on sight distance or a multiplier.
    if (TravelDestinations.size() > 1)
        getLogicalDestinations(TravelDestinations);

    WorldPosition botLocation(bot);
    vector<WorldPosition*> activePoints;

    //Pick one good point per destination.
    for (auto& activeTarget : TravelDestinations)
    {
        vector<WorldPosition*> points = activeTarget->nextPoint(&botLocation, true);
        if (!points.empty())
            activePoints.push_back(points.front());
    }

    //Pick the best destination and point (random shuffle).
    if (!getBestDestination(TravelDestinations, activePoints))
        return false;

    target->setTarget(TravelDestinations.front(), activePoints.front());

    return target->isActive();
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

        if (!groupTarget->getDestination()->isActive(bot) || groupTarget->getDestination()->getName() == "RpgTravelDestination")
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

    if (oldTarget->isMaxRetry(false))
        return false;

    if (!oldDestination) //Does this target have a destination?
        return false;

    if (!oldDestination->isActive(bot)) //Is the destination still valid?
        return false;

    WorldPosition botLocation(bot);
    vector<WorldPosition*> availablePoints = oldDestination->nextPoint(&botLocation, true);

    if (availablePoints.empty())
        return false;

    target->setTarget(oldTarget->getDestination(), availablePoints.front());
    target->setStatus(TRAVEL_STATUS_TRAVEL);
    target->setRetry(false, oldTarget->getRetryCount(false) + 1);

    return target->isActive();
}

bool ChooseTravelTargetAction::SetQuestTarget(TravelTarget* target, bool onlyCompleted)
{
    vector<TravelDestination*> TravelDestinations;

    QuestStatusMap& questMap = bot->getQuestStatusMap();

    //Find destinations related to the active quests.
    for (auto& quest : questMap)
    {
        if (quest.second.m_rewarded)
            continue;

        uint32 questId = quest.first;
        QuestStatusData* questStatus = &quest.second;

        if (onlyCompleted && sObjectMgr.GetQuestTemplate(questId) && !bot->CanRewardQuest(sObjectMgr.GetQuestTemplate(questId), false))
            continue;

        //Find quest takers or objectives
        vector<TravelDestination*> questDestinations = sTravelMgr.getQuestTravelDestinations(bot, questId, true, false);

        if (questDestinations.size() == 1)
            TravelDestinations.push_back(questDestinations.front());
        else if (getBestDestination(questDestinations))
        {
            TravelDestinations.push_back(questDestinations.front());
        }               
    }

    return SetBestTarget(target, TravelDestinations);
}

bool ChooseTravelTargetAction::SetNewQuestTarget(TravelTarget* target)
{
    //Find quest givers
    vector<TravelDestination*> TravelDestinations = sTravelMgr.getQuestTravelDestinations(bot, -1, true, false);
   
    return SetBestTarget(target, TravelDestinations);
}

bool ChooseTravelTargetAction::SetRpgTarget(TravelTarget* target)
{
    //Find rpg npcs
    vector<TravelDestination*> TravelDestinations = sTravelMgr.getRpgTravelDestinations(bot, true, false);

    return SetBestTarget(target, TravelDestinations);
}

bool ChooseTravelTargetAction::SetGrindTarget(TravelTarget* target)
{
    //Find grind mobs.
    vector<TravelDestination*> TravelDestinations = sTravelMgr.getGrindTravelDestinations(bot, true, false, sPlayerbotAIConfig.sightDistance);

    return SetBestTarget(target, TravelDestinations);
}

bool ChooseTravelTargetAction::SetBossTarget(TravelTarget* target)
{
    //Find boss mobs.
    vector<TravelDestination*> TravelDestinations = sTravelMgr.getBossTravelDestinations(bot, false);

    return SetBestTarget(target, TravelDestinations);
}

bool ChooseTravelTargetAction::SetExploreTarget(TravelTarget* target)
{
    //Find exploration loctions (middle of a sub-zone).
    vector<TravelDestination*> TravelDestinations = sTravelMgr.getExploreTravelDestinations(bot, true, true);

    return SetBestTarget(target, TravelDestinations);
}

char* strstri(const char* haystack, const char* needle);

bool ChooseTravelTargetAction::SetNpcFlagTarget(TravelTarget* target, vector<NPCFlags> flags, string name, vector<uint32> items)
{
    WorldPosition* botPos = &WorldPosition(bot);

    vector<TravelDestination*> TravelDestinations;

    //Loop over all npcs.
    for (auto& d : sTravelMgr.getRpgTravelDestinations(bot, true, true))
    {
        if (!d->getEntry())
            continue;

        CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(d->getEntry());

        if (!cInfo)
            continue;

        //Check if the npc has any of the required flags.
        bool foundFlag = false;
        for(auto flag : flags)
            if (cInfo->NpcFlags & flag)
            {
                foundFlag = true;
                break;
            }

        if (!foundFlag)
            continue;

        //Check if the npc has (part of) the required name.
        if (!name.empty() && !strstri(cInfo->Name, name.c_str()) && !strstri(cInfo->SubName, name.c_str()))
            continue;

        //Check if the npc sells any of the wanted items.
        if (!items.empty())
        {
            bool foundItem = false;
            VendorItemData const* vItems = nullptr;
            VendorItemData const* tItems = nullptr;

            vItems = sObjectMgr.GetNpcVendorItemList(d->getEntry());

//#ifndef MANGOSBOT_ZERO    
            uint32 vendorId = cInfo->VendorTemplateId;
            if (vendorId)
                tItems = sObjectMgr.GetNpcVendorTemplateItemList(vendorId);
//#endif

            for (auto item : items)
            {
                if (vItems && !vItems->Empty())
                for(auto vitem : vItems->m_items) 
                   if (vitem->item == item)
                    {
                        foundItem = true;
                        break;
                    }
                if(tItems && !tItems->Empty())
                for (auto titem : tItems->m_items)
                    if (titem->item == item)
                    {
                        foundItem = true;
                        break;
                    }
            }

            if (!foundItem)
                continue;
        }

        //Check if the npc is friendly.
        FactionTemplateEntry const* factionEntry = sFactionTemplateStore.LookupEntry(cInfo->Faction);
        ReputationRank reaction = ai->getReaction(factionEntry);

        if (reaction  < REP_NEUTRAL)
            continue;

        TravelDestinations.push_back(d);
    }

    if (SetBestTarget(target, TravelDestinations))
    {
        target->setForced(true);
    }

    return false;
}

bool ChooseTravelTargetAction::SetNullTarget(TravelTarget* target)
{
    target->setTarget(sTravelMgr.nullTravelDestination, sTravelMgr.nullWorldPosition, true);
    
    return true;
}

vector<string> split(const string& s, char delim);
char* strstri(const char* haystack, const char* needle);

//Find a destination based on (part of) it's name. Includes zones, ncps and mobs. Picks the closest one that matches.
TravelDestination* ChooseTravelTargetAction::FindDestination(Player* bot, string name)
{
    PlayerbotAI* ai = bot->GetPlayerbotAI();

    AiObjectContext* context = ai->GetAiObjectContext();

    vector<TravelDestination*> dests;

    //Zones
    for (auto& d : sTravelMgr.getExploreTravelDestinations(bot, true, true))
    {
        if (strstri(d->getTitle().c_str(), name.c_str()))
            dests.push_back(d);
    }

    //Npcs
    for (auto& d : sTravelMgr.getRpgTravelDestinations(bot, true, true))
    {
        if (strstri(d->getTitle().c_str(), name.c_str()))
            dests.push_back(d);
    }

    //Mobs
    for (auto& d : sTravelMgr.getGrindTravelDestinations(bot, true, true))
    {
        if (strstri(d->getTitle().c_str(), name.c_str()))
            dests.push_back(d);
    }

    //Bosses
    for (auto& d : sTravelMgr.getBossTravelDestinations(bot, true, true))
    {
        if (strstri(d->getTitle().c_str(), name.c_str()))
            dests.push_back(d);
    }

    WorldPosition* botPos = &WorldPosition(bot);

    if (dests.empty())
        return nullptr;

    TravelDestination* dest = *std::min_element(dests.begin(), dests.end(), [botPos](TravelDestination* i, TravelDestination* j) {return i->distanceTo(botPos) < j->distanceTo(botPos); });

    return dest;
};

bool ChooseTravelTargetAction::isUseful()
{
    if (!ai->AllowActivity(TRAVEL_ACTIVITY))
        return false;

    return !context->GetValue<TravelTarget *>("travel target")->Get()->isActive() 
        && !context->GetValue<LootObject>("loot target")->Get().IsLootPossible(bot)
        && !bot->IsInCombat();
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

            if (questTemplate->GetQuestLevel() > (int)bot->GetLevel())
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
