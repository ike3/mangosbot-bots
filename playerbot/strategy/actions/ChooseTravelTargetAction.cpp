#include "botpch.h"
#include "../../playerbot.h"
#include "../../LootObjectStack.h"
#include "ChooseTravelTargetAction.h"
#include "../../PlayerbotAIConfig.h"
#include <iomanip>

using namespace ai;

bool ChooseTravelTargetAction::Execute(Event& event)
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
        {
            PerformanceMonitorOperation* pmo = sPerformanceMonitor.start(PERF_MON_VALUE, "SetRpgTarget1", &context->performanceStack);
            foundTarget = SetRpgTarget(newTarget);                           //Go to town to sell items or repair
            if(pmo) pmo->finish();
        }
        else if (AI_VALUE2(bool, "group or", "should sell,can ah sell,following party,near leader") && bot->GetLevel() > 5)
        {
            PerformanceMonitorOperation* pmo = sPerformanceMonitor.start(PERF_MON_VALUE, "SetNpcFlagTarget1", &context->performanceStack);
            foundTarget = SetNpcFlagTarget(newTarget, { UNIT_NPC_FLAG_AUCTIONEER });
            if(pmo) pmo->finish();
        }

    //Rpg in city
    if (!foundTarget && urand(1, 100) > 90 && bot->GetLevel() > 5)           //10% chance
    {
        PerformanceMonitorOperation* pmo = sPerformanceMonitor.start(PERF_MON_VALUE, "SetNpcFlagTarget2", &context->performanceStack);
        foundTarget = SetNpcFlagTarget(newTarget, { UNIT_NPC_FLAG_BANKER,UNIT_NPC_FLAG_BATTLEMASTER,UNIT_NPC_FLAG_AUCTIONEER });
        if(pmo) pmo->finish();
    }

    // PvP activities
    bool pvpActivate = false;
    if (pvpActivate && !foundTarget && urand(0, 4) && bot->GetLevel() > 50)
    {
        WorldPosition pos = WorldPosition(bot);
        WorldPosition* botPos = &pos;
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
                ai->TellMasterNoFacing(out.str(), PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
                foundTarget = true;
            }
        }
    }

    //Grind for money
    if (!foundTarget && AI_VALUE(bool, "should get money"))
    {
        if (urand(1, 100) > 50) //50% Focus on active quests for money.
        {
            {
                PerformanceMonitorOperation* pmo = sPerformanceMonitor.start(PERF_MON_VALUE, "SetQuestTarget1", &context->performanceStack);
                foundTarget = SetQuestTarget(newTarget, false, true, true);           //Turn in quests for money.
                if(pmo) pmo->finish();
            }

            if (!foundTarget)
            {
                PerformanceMonitorOperation* pmo = sPerformanceMonitor.start(PERF_MON_VALUE, "SetQuestTarget2", &context->performanceStack);
                foundTarget = SetQuestTarget(newTarget, true, false, false);      //Find new (low) level quests
                if(pmo) pmo->finish();
            }
        }
        else
        {
            PerformanceMonitorOperation* pmo = sPerformanceMonitor.start(PERF_MON_VALUE, "SetGrindTarget1", &context->performanceStack);
            foundTarget = SetGrindTarget(newTarget);                               //Go grind mobs for money    
            if(pmo) pmo->finish();
        }
    }


    //Continue current target.
    if (!foundTarget && urand(1, 100) > 10)                               //90% chance 
    {
        PerformanceMonitorOperation* pmo = sPerformanceMonitor.start(PERF_MON_VALUE, "SetCurrentTarget", &context->performanceStack);
        foundTarget = SetCurrentTarget(newTarget, oldTarget);             //Extend current target.
        if(pmo) pmo->finish();
    }

    //Dungeon in group.
    if (!foundTarget && urand(1, 100) > 50)                                 //50% chance
        if (AI_VALUE(bool, "can fight boss"))
        {
            PerformanceMonitorOperation* pmo = sPerformanceMonitor.start(PERF_MON_VALUE, "SetBossTarget", &context->performanceStack);
            foundTarget = SetBossTarget(newTarget);                         //Go fight a (dungeon boss)
            if(pmo) pmo->finish();
        }

    //Do quests (start, do, end)
    if (!foundTarget && urand(1, 100) > 5)                                 //95% chance
    {
        PerformanceMonitorOperation* pmo = sPerformanceMonitor.start(PERF_MON_VALUE, "SetQuestTarget", &context->performanceStack);
        foundTarget = SetQuestTarget(newTarget, true, true, true);    //Do any nearby           
        if(pmo) pmo->finish();
    }

    //Explore a nearby unexplored area.
    if (!foundTarget && ai->HasStrategy("explore", BotState::BOT_STATE_NON_COMBAT) && urand(1, 100) > 90)  //10% chance Explore a unexplored sub-zone.
    {
        PerformanceMonitorOperation* pmo = sPerformanceMonitor.start(PERF_MON_VALUE, "SetExploreTarget", &context->performanceStack);
        foundTarget = SetExploreTarget(newTarget);
        if(pmo) pmo->finish();
    }

    //Just hang with an npc
    if (!foundTarget && urand(1, 100) > 50)                                 //50% chance
    {
        {
            PerformanceMonitorOperation* pmo = sPerformanceMonitor.start(PERF_MON_VALUE, "SetRpgTarget2", &context->performanceStack);
            foundTarget = SetRpgTarget(newTarget);
            if (foundTarget)
                newTarget->setForced(true);
            if(pmo) pmo->finish();
        }
    }

    if (!foundTarget)
    {
        PerformanceMonitorOperation* pmo = sPerformanceMonitor.start(PERF_MON_VALUE, "SetGrindTarget2", &context->performanceStack);
        foundTarget = SetGrindTarget(newTarget);
        if(pmo) pmo->finish();
    }

    if (!foundTarget)
        SetNullTarget(newTarget);                                           //Idle a bit.
}

void ChooseTravelTargetAction::setNewTarget(TravelTarget* newTarget, TravelTarget* oldTarget)
{
    //Tell the master where we are going.
    //if (!bot->GetGroup() || (ai->GetGroupMaster() == bot))
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

        out << round(newTarget->getDestination()->distanceTo(botLocation)) << "y";

        out << " for " << chat->formatQuest(quest);

        out << " to " << QuestDestination->getTitle();
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

        out << round(newTarget->getDestination()->distanceTo(botLocation)) << "y";

        out << " for ";

        if (AI_VALUE2(bool, "group or", "should sell,can sell"))
            out << "selling items";
        else if (AI_VALUE2(bool, "group or", "should repair,can repair"))
            out << "repairing";
        else
            out << "rpg";

        out << " to " << RpgDestination->getTitle();        
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

        out << round(newTarget->getDestination()->distanceTo(botLocation)) << "y";

        out << " for exploration";

        out << " to " << ExploreDestination->getTitle();
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

        out << round(newTarget->getDestination()->distanceTo(botLocation)) << "y";

        out << " for grinding money";

        out << " to " << GrindDestination->getTitle();
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

        out << round(newTarget->getDestination()->distanceTo(botLocation)) << "y";

        out << " for good loot";

        out << " to " << BossDestination->getTitle();
    }
    else if (destination->getName() == "NullTravelDestination")
    {
        if (!oldTarget->getDestination() || oldTarget->getDestination()->getName() != "NullTravelDestination")
        {
            out.clear();
            out << "No where to travel. Idling a bit.";
        }
    }

    if (out.str().empty())
        return;

    ai->TellMasterNoFacing(out,PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);

    string message = out.str().c_str();

    if (sPlayerbotAIConfig.hasLog("travel_map.csv"))
    {
        WorldPosition botPos(bot);
        WorldPosition destPos = *newTarget->getPosition();

        ostringstream out;
        out << sPlayerbotAIConfig.GetTimestampStr() << "+00,";
        out << bot->GetName() << ",";
        out << std::fixed << std::setprecision(2);

        out << to_string(bot->getRace()) << ",";
        out << to_string(bot->getClass()) << ",";
        float subLevel = ((float)bot->GetLevel() + ((float)bot->GetUInt32Value(PLAYER_XP) / (float)bot->GetUInt32Value(PLAYER_NEXT_LEVEL_XP)));

        out << subLevel << ",";

        if (!destPos)
            destPos = botPos;

        botPos.printWKT({ botPos,destPos }, out, 1);

        if (destination->getName() == "NullTravelDestination")
            out << "0,";
        else
            out << round(newTarget->getDestination()->distanceTo(botPos)) << ",";

        out << "1," << "\"" << destination->getTitle() << "\",\"" << message << "\"";

        sPlayerbotAIConfig.log("travel_map.csv", out.str().c_str());

        WorldPosition lastPos = AI_VALUE2(WorldPosition, "custom position", "last choose travel");

        if (lastPos)
        {
            ostringstream out;
            out << sPlayerbotAIConfig.GetTimestampStr() << "+00,";
            out << bot->GetName() << ",";
            out << std::fixed << std::setprecision(2);

            out << to_string(bot->getRace()) << ",";
            out << to_string(bot->getClass()) << ",";
            float subLevel = ((float)bot->GetLevel() + ((float)bot->GetUInt32Value(PLAYER_XP) / (float)bot->GetUInt32Value(PLAYER_NEXT_LEVEL_XP)));

            out << subLevel << ",";

            WorldPosition lastPos = AI_VALUE2(WorldPosition, "custom position", "last choose travel");

            botPos.printWKT({ lastPos, botPos }, out, 1);

            if (destination->getName() == "NullTravelDestination")
                out << "0,";
            else
                out << round(newTarget->getDestination()->distanceTo(botPos)) << ",";

            out << "0," << "\"" << destination->getTitle() << "\",\""<< message << "\"";

            sPlayerbotAIConfig.log("travel_map.csv", out.str().c_str());
        }

        SET_AI_VALUE2(WorldPosition, "custom position", "last choose travel", botPos);
    }
}

//Select only those points that are in sight distance or failing that a multiplication of the sight distance.
vector<WorldPosition*> ChooseTravelTargetAction::getLogicalPoints(vector<WorldPosition*>& travelPoints)
{
    PerformanceMonitorOperation* pmo = sPerformanceMonitor.start(PERF_MON_VALUE, "getLogicalPoints", &context->performanceStack);
    vector<WorldPosition*> retvec;

    static vector<float> distanceLimits = { sPlayerbotAIConfig.sightDistance, 4 * sPlayerbotAIConfig.sightDistance, 10 * sPlayerbotAIConfig.sightDistance, 20 * sPlayerbotAIConfig.sightDistance, 50 * sPlayerbotAIConfig.sightDistance, 100 * sPlayerbotAIConfig.sightDistance, 10000 * sPlayerbotAIConfig.sightDistance };

    vector<vector<WorldPosition*>> partitions;

    for (uint8 l = 0; l < distanceLimits.size(); l++)
        partitions.push_back({});

    WorldPosition centerLocation;

    int32 botLevel = (int)bot->GetLevel();

    bool canFightElite = AI_VALUE(bool, "can fight elite");

    if (AI_VALUE(bool, "can fight boss"))
        botLevel += 5;
    else if (canFightElite)
        botLevel += 2;
    else if (!AI_VALUE(bool, "can fight equal"))
        botLevel -= 2;

    if (botLevel < 6)
        botLevel = 6;

    if (ai->GetMaster())
        centerLocation = WorldPosition(ai->GetMaster());
    else
        centerLocation = WorldPosition(bot);

    PerformanceMonitorOperation* pmo1 = sPerformanceMonitor.start(PERF_MON_VALUE, "Shuffle", &context->performanceStack);
    if(travelPoints.size() > 50)
        std::shuffle(travelPoints.begin(), travelPoints.end(), *GetRandomGenerator());
    if(pmo1) pmo1->finish();

    uint8 checked = 0;

    //Loop over all points
    for (auto pos : travelPoints)
    {
        PerformanceMonitorOperation* pmo1 = sPerformanceMonitor.start(PERF_MON_VALUE, "AreaLevel", &context->performanceStack);
        int32 areaLevel = pos->getAreaLevel();
        if (pmo1) pmo1->finish();

        if (!pos->isOverworld() && !canFightElite)
            areaLevel += 10;

        if (!areaLevel || botLevel < areaLevel) //Skip points that are in a area that is too high level.
            continue;

        GuidPosition* guidP = dynamic_cast<GuidPosition*>(pos);

        PerformanceMonitorOperation* pmo2 = sPerformanceMonitor.start(PERF_MON_VALUE, "IsEventUnspawned", &context->performanceStack);
        if (guidP && guidP->IsEventUnspawned()) //Skip points that are not spawned due to events.
        {
            if(pmo2) pmo2->finish();
            continue;
        }
        if (pmo2) pmo2->finish();

        PerformanceMonitorOperation* pmo3 = sPerformanceMonitor.start(PERF_MON_VALUE, "distancePartition", &context->performanceStack);
        centerLocation.distancePartition(distanceLimits, pos, partitions); //Partition point in correct distance bracket.
        if (pmo3) pmo3->finish();

        if (checked++ > 50)
            break;
    }

    if(pmo) pmo->finish();

    for (uint8 l = 0; l < distanceLimits.size(); l++)
    {
        if (partitions[l].empty() || !urand(0, 10)) //Return the first non-empty bracket with 10% chance to skip a higher bracket.
            continue;

        return partitions[l];
    }

    return partitions.back();
}

//Sets the target to the best destination.
bool ChooseTravelTargetAction::SetBestTarget(TravelTarget* target, vector<TravelDestination*>& TravelDestinations)
{
    if (TravelDestinations.empty())
        return false;

    WorldPosition botLocation(bot);

    vector<WorldPosition*> travelPoints;

    //Select all points from the selected destinations
    for (auto& activeTarget : TravelDestinations)
    {
        vector<WorldPosition*> points = activeTarget->getPoints(true);
        for (WorldPosition* point : points)
        {
            if (point && point->isValid())
            {
                travelPoints.push_back(point);
            }
        }
    }

    if (ai->HasStrategy("debug travel", BotState::BOT_STATE_NON_COMBAT))
        ai->TellMasterNoFacing(to_string(travelPoints.size()) + " points total.");

    if (travelPoints.empty()) //No targets or no points.
        return false;

    if (TravelDestinations.size() == 1 && travelPoints.size() == 1)
    {
        target->setTarget(TravelDestinations.front(), travelPoints.front());
        return target->isActive();
    }

    travelPoints = getLogicalPoints(travelPoints);

    if (travelPoints.empty())
        return false;

    if (ai->HasStrategy("debug travel", BotState::BOT_STATE_NON_COMBAT))
        ai->TellMasterNoFacing(to_string(travelPoints.size()) + " points in reasonable range.");

    travelPoints = sTravelMgr.getNextPoint(&botLocation, travelPoints); //Pick a good point.

    //Pick the best destination and point (random shuffle).

    for (auto destination : TravelDestinations) //Pick the destination that has this point.
        if (destination->hasPoint(travelPoints.front()))
        {
            TravelDestinations.front() = destination;
            break;
        }

    target->setTarget(TravelDestinations.front(), travelPoints.front());

    if (ai->HasStrategy("debug travel", BotState::BOT_STATE_NON_COMBAT))
        ai->TellMasterNoFacing("Point at " + to_string(floor(target->distance(bot))) + "y selected.");

    return target->isActive();
}

bool ChooseTravelTargetAction::SetGroupTarget(TravelTarget* target)
{
    vector<TravelDestination*> activeDestinations;
    vector<WorldPosition*> activePoints;

    list<ObjectGuid> groupPlayers;

    Group* group = bot->GetGroup();
    if (!group)
        return false;

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

    if (ai->HasStrategy("debug travel", BotState::BOT_STATE_NON_COMBAT))
        ai->TellMasterNoFacing(to_string(activeDestinations.size()) + " group targets found.");

    bool hasTarget = SetBestTarget(target, activeDestinations);

    if (hasTarget)
        target->setGroupCopy();

    return hasTarget;
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

    vector<TravelDestination*> TravelDestinations = { oldDestination };

    if (!SetBestTarget(target, TravelDestinations))
        return false;
   
    target->setStatus(TravelStatus::TRAVEL_STATUS_TRAVEL);
    target->setRetry(false, oldTarget->getRetryCount(false) + 1);

    return target->isActive();
}

bool ChooseTravelTargetAction::SetQuestTarget(TravelTarget* target, bool newQuests, bool activeQuests, bool completedQuests)
{
    vector<TravelDestination*> TravelDestinations;

    if (newQuests)
    {
        PerformanceMonitorOperation* pmo = sPerformanceMonitor.start(PERF_MON_VALUE, "getQuestTravelDestinations1", &context->performanceStack);
        TravelDestinations = sTravelMgr.getQuestTravelDestinations(bot, -1, true, false, 400 + bot->GetLevel() * 10); //Prefer new quests near the player at lower levels.
        if(pmo) pmo->finish();
    }

    if (ai->HasStrategy("debug travel", BotState::BOT_STATE_NON_COMBAT))
        ai->TellMasterNoFacing(to_string(TravelDestinations.size()) + " new quest destinations found.");

    if (activeQuests || completedQuests)
    {
        QuestStatusMap& questMap = bot->getQuestStatusMap();

        //Find destinations related to the active quests.
        for (auto& quest : questMap)
        {
            if (quest.second.m_rewarded)
                continue;

            uint32 questId = quest.first;
            QuestStatusData* questStatus = &quest.second;

            if (!activeQuests && !bot->CanRewardQuest(sObjectMgr.GetQuestTemplate(questId), false))
                continue;

            if (!completedQuests && bot->CanRewardQuest(sObjectMgr.GetQuestTemplate(questId), false))
                continue;

            //Find quest takers or objectives
            PerformanceMonitorOperation* pmo = sPerformanceMonitor.start(PERF_MON_VALUE, "getQuestTravelDestinations2", &context->performanceStack);
            vector<TravelDestination*> questDestinations = sTravelMgr.getQuestTravelDestinations(bot, questId, true, false,0);
            if(pmo) pmo->finish();

            TravelDestinations.insert(TravelDestinations.end(), questDestinations.begin(), questDestinations.end());
        }
    }

    if (ai->HasStrategy("debug travel", BotState::BOT_STATE_NON_COMBAT))
        ai->TellMasterNoFacing(to_string(TravelDestinations.size()) + " quest destinations found.");

    return SetBestTarget(target, TravelDestinations);
}

bool ChooseTravelTargetAction::SetRpgTarget(TravelTarget* target)
{
    //Find rpg npcs
    vector<TravelDestination*> TravelDestinations = sTravelMgr.getRpgTravelDestinations(bot, true, false);

    if (ai->HasStrategy("debug travel", BotState::BOT_STATE_NON_COMBAT))
        ai->TellMasterNoFacing(to_string(TravelDestinations.size()) + " rpg destinations found.");

    return SetBestTarget(target, TravelDestinations);
}

bool ChooseTravelTargetAction::SetGrindTarget(TravelTarget* target)
{
    //Find grind mobs.
    vector<TravelDestination*> TravelDestinations = sTravelMgr.getGrindTravelDestinations(bot, true, false, 600+bot->GetLevel()*400);

    if (ai->HasStrategy("debug travel", BotState::BOT_STATE_NON_COMBAT))
        ai->TellMasterNoFacing(to_string(TravelDestinations.size()) + " grind destinations found.");

    return SetBestTarget(target, TravelDestinations);
}

bool ChooseTravelTargetAction::SetBossTarget(TravelTarget* target)
{
    //Find boss mobs.
    vector<TravelDestination*> TravelDestinations = sTravelMgr.getBossTravelDestinations(bot, true);

    if (ai->HasStrategy("debug travel", BotState::BOT_STATE_NON_COMBAT))
        ai->TellMasterNoFacing(to_string(TravelDestinations.size()) + " boss destinations found.");

    return SetBestTarget(target, TravelDestinations);
}

bool ChooseTravelTargetAction::SetExploreTarget(TravelTarget* target)
{
    //Find exploration loctions (middle of a sub-zone).
    vector<TravelDestination*> TravelDestinations = sTravelMgr.getExploreTravelDestinations(bot, true, false);

    if (ai->HasStrategy("debug travel", BotState::BOT_STATE_NON_COMBAT))
        ai->TellMasterNoFacing(to_string(TravelDestinations.size()) + " explore destinations found.");

    return SetBestTarget(target, TravelDestinations);
}

char* strstri(const char* haystack, const char* needle);

bool ChooseTravelTargetAction::SetNpcFlagTarget(TravelTarget* target, vector<NPCFlags> flags, string name, vector<uint32> items)
{
    WorldPosition pos = WorldPosition(bot);
    WorldPosition* botPos = &pos;

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

    if (ai->HasStrategy("debug travel", BotState::BOT_STATE_NON_COMBAT))
        ai->TellMasterNoFacing(to_string(TravelDestinations.size()) + " npc flag targets found.");

    SetBestTarget(target, TravelDestinations);

    target->setForced(true);

    return true; //Flag targets are always inactive for now.
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

    WorldPosition botPos(bot);

    if (dests.empty())
        return nullptr;

    TravelDestination* dest = *std::min_element(dests.begin(), dests.end(), [botPos](TravelDestination* i, TravelDestination* j) {return i->distanceTo(botPos) < j->distanceTo(botPos); });

    return dest;
};

bool ChooseTravelTargetAction::isUseful()
{
    if (!ai->AllowActivity(TRAVEL_ACTIVITY))
        return false;

    if (bot->GetGroup() && !bot->GetGroup()->IsLeader(bot->GetObjectGuid()))
        if (ai->HasStrategy("follow", BotState::BOT_STATE_NON_COMBAT) || ai->HasStrategy("stay", BotState::BOT_STATE_NON_COMBAT))
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
