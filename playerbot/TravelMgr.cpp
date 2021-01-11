#include "TravelMgr.h"
#include "ObjectMgr.h"
#include <numeric>


using namespace ai;
using namespace MaNGOS;


vector<WorldPosition*> TravelDestination::getPoints(int max) {
    if (max == -1)
        max = maxVisitorsPerPoint;

    if (max == 0)
        return points;

    vector<WorldPosition*> retVec;  
    std::copy_if(points.begin(), points.end(), std::back_inserter(retVec), [max](WorldPosition* p) { return p->getVistitor() < max; }); 
    return retVec;
}

WorldPosition* TravelDestination::nearestPoint(WorldPosition* pos) { 
    //struct compare { bool operator() (WorldPosition* i, WorldPosition* j, WorldPosition* p) { return i->distance(p) < j->distance(p); } } compare_obj;
    //return *std::min_element(getPoints().begin(), getPoints().end(), std::bind(compare_obj, _1, _2, pos)); 

    return *std::min_element(points.begin(), points.end(), [pos](WorldPosition* i, WorldPosition* j) {return i->distance(pos) < j->distance(pos); });
}

vector<WorldPosition *> TravelDestination::touchingPoints(WorldPosition * pos) {
    vector<WorldPosition *> ret_points;
    for (auto& point : points)
    {
        float dist = pos->distance(point);
        if (dist == 0)
            continue;

        if (dist > radiusMax * 2)
            continue;

        ret_points.push_back(point);
    }

    return ret_points;
};

vector<WorldPosition*> TravelDestination::sortedPoints(WorldPosition *pos) {
    vector<WorldPosition*> ret_points = points;

    std::sort(ret_points.begin(), ret_points.end(), [pos](WorldPosition* i, WorldPosition* j) {return i->distance(pos) < j->distance(pos); });

    return ret_points;
};

vector <WorldPosition*> TravelDestination::nextPoint(WorldPosition * pos, bool ignoreFull) {
    return sTravelMgr.getNextPoint(pos, ignoreFull ? points : getPoints());
}

bool TravelDestination::isFull(bool ignoreFull) {
    if (!ignoreFull && maxVisitors > 0 && visitors >= maxVisitors)
        return true;

    if (maxVisitorsPerPoint > 0)
        if (getPoints().empty())
                return true;

    return false;
}

bool QuestRelationTravelDestination::isActive(Player* bot) {
    if (relation == 0)
    {
        if (questTemplate->GetQuestLevel() >= bot->getLevel() + 5)
            return false;
        if (questTemplate->XPValue(bot) == 0)
            return false;
        if (!bot->CanTakeQuest(questTemplate, false))
            return false;
        if (sTravelMgr.getDialogStatus(bot, entry, questTemplate) != DIALOG_STATUS_AVAILABLE)
            return false;
    }
    else
    {
        if (!bot->HasQuest(questId))
            return false;
        if (!bot->CanCompleteQuest(questId))
            return false;

        if (sTravelMgr.getDialogStatus(bot, entry, questTemplate) != DIALOG_STATUS_REWARD2)
            return false;
    }

    return true;
}

bool QuestObjectiveTravelDestination::isActive(Player* bot) {
    if (questTemplate->GetQuestLevel() > bot->getLevel())
        return false;

    return sTravelMgr.getObjectiveStatus(bot, questTemplate, objective);
}


void TravelTarget::setTarget(TravelDestination* tDestination1, WorldPosition* wPosition1) {

    wPosition = wPosition1;
    tDestination = tDestination1;

    setStatus(TRAVEL_STATUS_TRAVEL);
}

void TravelTarget::copyTarget(TravelTarget* target) { 
    if (tDestination)
        tDestination->remVisitor();
    if (wPosition)
        wPosition->remVisitor();

    setTarget(target->tDestination, target->wPosition);
    groupCopy = target->isGroupCopy();

    wPosition->addVisitor();
    tDestination->addVisitor();
}

void TravelTarget::setStatus(TravelStatus status) {
    m_status = status;
    startTime = WorldTimer::getMSTime();

    switch (m_status) {
    case TRAVEL_STATUS_NONE:
    case TRAVEL_STATUS_PREPARE:
    case TRAVEL_STATUS_EXPIRED:
        statusTime = -1;
        break;
    case TRAVEL_STATUS_TRAVEL:
        statusTime = getMaxTravelTime() * 2 + sPlayerbotAIConfig.maxWaitForMove;
        break;
    case TRAVEL_STATUS_WORK:
        statusTime = tDestination->getExpireDelay();
        break;
    case TRAVEL_STATUS_COOLDOWN:
        statusTime = tDestination->getCooldownDelay();
    }
}

bool TravelTarget::isActive() { 
    if (m_status == TRAVEL_STATUS_NONE || m_status == TRAVEL_STATUS_EXPIRED || m_status == TRAVEL_STATUS_PREPARE)
        return false;

    if (statusTime > 0 && startTime + statusTime < WorldTimer::getMSTime())
    {
        setStatus(TRAVEL_STATUS_EXPIRED);
        return false;
    }

    if (isTraveling())
        return true;

    if (isWorking())
        return true;

    if (m_status == TRAVEL_STATUS_COOLDOWN)
        return true;

    if (!tDestination->isActive(bot)) //Target has become invalid. Stop.
    {
        setStatus(TRAVEL_STATUS_COOLDOWN);
        return true;
    }   

    return true;
};

bool TravelTarget::isTraveling() { 
    if (m_status != TRAVEL_STATUS_TRAVEL)
        return false;

    if (!tDestination->isActive(bot)) //Target has become invalid. Stop.
    {
        setStatus(TRAVEL_STATUS_COOLDOWN);
        return false;
    }
    
    WorldPosition pos(bot);

    bool HasArrived = tDestination->isIn(&pos);

    if (HasArrived)
    {
        setStatus(TRAVEL_STATUS_WORK);
        return false;
    }        
    
    return true;
}

bool TravelTarget::isWorking() {
    if (m_status != TRAVEL_STATUS_WORK)
        return false;

    if (!tDestination->isActive(bot)) //Target has become invalid. Stop.
    {
        setStatus(TRAVEL_STATUS_COOLDOWN);
        return false;
    }

    WorldPosition pos(bot);

    /*
    bool HasLeft = tDestination->isOut(&pos);

    if (HasLeft)
    {
        setStatus(TRAVEL_STATUS_TRAVEL);
        return false;
    }
    */

    return true;
}

bool TravelTarget::isPreparing() {
    if (m_status != TRAVEL_STATUS_PREPARE)
        return false;

    return true;
}

void TravelMgr::Clear()
{
    for (auto& quest : questTravelDestinations)
    {
        delete quest.second;
    }

    questTravelDestinations.clear();
    pointsMap.clear();
}

void TravelMgr::LoadQuestTravelTable()
{
    uint32 count = 0;

    // Clearing store (for reloading case)
    Clear();

    string query = "SELECT 0, qr.quest, 0, c.id, role objective, c.guid, c.map, c.position_x, c.position_y, c.position_z, c.orientation FROM quest_relations qr JOIN creature c  ON(c.id = qr.entry) WHERE actor = 0";
    query = query + " UNION ALL ";
    query = query + "SELECT 1, qr.quest, 0, c.id, role objective, c.guid,c.map, c.position_x, c.position_y, c.position_z, c.orientation FROM quest_relations qr JOIN gameobject c  ON(c.id = qr.entry) WHERE actor = 1";
    query = query + " UNION ALL ";

    for (int i = 1; i <= 4; i++)
    {
        query = query + "SELECT 2, qt.entry, 0, c.id, " + to_string(i) + " objective, c.guid, c.map, c.position_x,c.position_y, c.position_z, c.orientation FROM quest_template qt JOIN creature c  ON (c.id = qt.ReqCreatureOrGOId" + to_string(i) + ")";
        query = query + " UNION ALL ";
        query = query + "SELECT 2,qt.entry, clt.item, clt.entry, " + to_string(i) + " objective, c.guid, c.map, c.position_x,c.position_y, c.position_z, c.orientation FROM quest_template qt JOIN creature_loot_template clt  ON (clt.item = qt.ReqItemId" + to_string(i) + ") JOIN creature c ON (c.id = clt.entry)";
        query = query + " UNION ALL ";
        query = query + "SELECT 3,qt.entry, 0, c.id, " + to_string(i) + " objective,c.guid, c.map, c.position_x,c.position_y, c.position_z, c.orientation FROM quest_template qt JOIN gameobject c  ON (c.id = qt.ReqCreatureOrGOId" + to_string(i) + " * -1)";
        query = query + " UNION ALL ";
        query = query + "SELECT 3,qt.entry, clt.item, clt.entry, " + to_string(i) + " objective, c.guid, c.map, c.position_x,c.position_y, c.position_z, c.orientation FROM quest_template qt JOIN gameobject_loot_template clt  ON (clt.item = qt.ReqItemId" + to_string(i) + ") JOIN gameobject c ON (c.id = clt.entry)";
        query = query + (i<4?" UNION ALL ":" ORDER BY 2,4,1");
    }

    QueryResult* result = WorldDatabase.PQuery(query.c_str());

    if (result)
    {
        BarGoLink bar(result->GetRowCount());

        uint32 pQuestId = 0;
        uint32 pEntry = 0;
        uint32 pType = 0;

        QuestTravelDestination* loc;
        QuestContainer * container;

        do
        {
            Field* fields = result->Fetch();
            bar.step();

            uint32 type = fields[0].GetUInt32();
            uint32 questId = fields[1].GetUInt32();
            uint32 item = fields[2].GetUInt32();
            uint32 entry = fields[3].GetUInt32();
            uint32 objective = fields[4].GetUInt32();
            uint32 guid = fields[5].GetUInt32();
            uint32 map = fields[6].GetUInt32();
            float  position_x = fields[7].GetFloat();
            float  position_y = fields[8].GetFloat();
            float  position_z = fields[9].GetFloat();
            float  orientation = fields[10].GetFloat();

            if (pQuestId != questId || pEntry != entry || pType != type)
            {
                if (type == 0 || type == 1) //Quest giver/taker
                {
                    loc = new QuestRelationTravelDestination(questId, entry, objective, sPlayerbotAIConfig.tooCloseDistance, sPlayerbotAIConfig.sightDistance);
                    loc->setExpireDelay(5 * 60 * 1000);
                    loc->setMaxVisitors(15, 0);
                }
                else if (type == 2 || type == 3) //Quest mobs
                {
                    loc = new QuestObjectiveTravelDestination(questId, entry, objective - 1, sPlayerbotAIConfig.tooCloseDistance, sPlayerbotAIConfig.sightDistance);
                    loc->setExpireDelay(1 * 60 * 1000);
                    loc->setMaxVisitors(100, 1);
                }

                questTravelDestinations.push_back(make_pair(questId, loc));

                pQuestId = questId;
                pEntry = entry;
                pType = type;

                auto i = quests.find(questId);
                if (i != quests.end())
                    container = i->second;
                else
                    container = new QuestContainer;

                if (type < 2)
                {
                    if (objective == 0)
                    {
                        questGivers.push_back(loc);
                        container->questGivers.push_back(loc);
                    }
                    else if (objective == 1)
                        container->questTakers.push_back(loc);
                }
                else
                    container->questObjectives.push_back(loc);

                quests.insert_or_assign(questId, container);            
            }

            WorldPosition point = WorldPosition(map, position_x, position_y, position_z, orientation);

            pointsMap.insert(make_pair(guid, point));

            questTravelDestinations.back().second->addPoint(&pointsMap.find(guid)->second);               
                        
            ++count;
        } while (result->NextRow());

        delete result;

        sLog.outString(">> Loaded " SIZEFMTD " quest details definitions.", questTravelDestinations.size());
        sLog.outString();
    }
    else
    {
        sLog.outString();
        sLog.outErrorDb(">> Error loading quest details.");
    }
}


uint32 TravelMgr::getDialogStatus(Player* pPlayer, uint32 questgiver, Quest const* pQuest)
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
        if (itr->second != pQuest->GetQuestId())
            continue;

        uint32 dialogStatusNew = DIALOG_STATUS_NONE;
        uint32 questId = itr->second;

        if (!pQuest || !pQuest->IsActive())
        {
            continue;
        }

        QuestStatus status = pPlayer->GetQuestStatus(questId);

        if ((status == QUEST_STATUS_COMPLETE && !pPlayer->GetQuestRewardStatus(questId)) ||
            (pQuest->IsAutoComplete() && pPlayer->CanTakeQuest(pQuest, false)))
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
        if (itr->second != pQuest->GetQuestId())
            continue;

        uint32 dialogStatusNew = DIALOG_STATUS_NONE;
        uint32 quest_id = itr->second;

        if (!pQuest || !pQuest->IsActive())
        {
            continue;
        }

        QuestStatus status = pPlayer->GetQuestStatus(quest_id);

        if (status == QUEST_STATUS_NONE)                    // For all other cases the mark is handled either at some place else, or with involved-relations already
        {
            if (pPlayer->CanSeeStartQuest(pQuest))
            {
                if (pPlayer->SatisfyQuestLevel(pQuest, false))
                {
                    int32 lowLevelDiff = sWorld.getConfig(CONFIG_INT32_QUEST_LOW_LEVEL_HIDE_DIFF);
                    if (pQuest->IsAutoComplete() || (pQuest->IsRepeatable() && pPlayer->getQuestStatusMap()[quest_id].m_rewarded))
                    {
                        dialogStatusNew = DIALOG_STATUS_REWARD_REP;
                    }
                    else if (lowLevelDiff < 0 || pPlayer->getLevel() <= pPlayer->GetQuestLevelForPlayer(pQuest) + uint32(lowLevelDiff))
                    {
                        dialogStatusNew = DIALOG_STATUS_AVAILABLE;
                    }
                    else
                    {
                        dialogStatusNew = DIALOG_STATUS_CHAT;
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


vector<WorldPosition*> TravelMgr::getNextPoint(WorldPosition* center, vector<WorldPosition*> points) {
    vector<WorldPosition*> retVec;
    //List of weights based on distance (Gausian curve that starts at 100 and lower to 0 at 800)
    vector<int> weights;

    std::transform(points.begin(), points.end(), std::back_inserter(weights), [center](WorldPosition* point) { return 100 * exp(-1 * pow(point->distance(center) / 400.0, 2)); });

    //Total sum of all those weights.
    int sum = std::accumulate(weights.begin(), weights.end(), 0);

    //Pick a random number in that range.
    int rnd = urand(0, sum);

    //Pick a random point based on weights.
    for (unsigned i = 0; i < points.size(); ++i)
        if (rnd < weights[i])
        {
            retVec.push_back(points[i]);
            return retVec;
        }
        else
            rnd -= weights[i];

    assert(!"No valid point found.");

    return retVec;
}

QuestStatusData* TravelMgr::getQuestStatus(Player* bot, uint32 questId)
{
    return &bot->getQuestStatusMap()[questId];
}

bool TravelMgr::getObjectiveStatus(Player* bot,  Quest const* pQuest, int objective)
{
    uint32 questId = pQuest->GetQuestId();
    if (!bot->IsActiveQuest(questId))
        return false;

    if (bot->GetQuestStatus(questId) != QUEST_STATUS_INCOMPLETE)
        return false;

    QuestStatusData* questStatus = sTravelMgr.getQuestStatus(bot, questId);

    uint32  reqCount = pQuest->ReqItemCount[objective];
    uint32  hasCount = questStatus->m_itemcount[objective];

    if (reqCount && hasCount < reqCount)
        return true;

    reqCount = pQuest->ReqCreatureOrGOCount[objective];
    hasCount = questStatus->m_creatureOrGOcount[objective];

    if (reqCount && hasCount < reqCount)
        return true;

    return false;
}

vector<QuestTravelDestination*> TravelMgr::getQuestTravelDestinations(Player* bot, uint32 questId, bool ignoreFull)
{
    WorldPosition botLocation(bot);

    vector<QuestTravelDestination*> retTravelLocations;

    if (questId == -1)
    {
        for (auto& dest : questGivers)
            if (dest->isActive(bot) && !dest->isFull(ignoreFull) && dest->distanceTo(&botLocation) < 2000)
                retTravelLocations.push_back(dest);
    }
    else
    {
        auto i = quests.find(questId);

        if (i != quests.end())
        {
            for (auto& dest : i->second->questTakers)
                if (dest->isActive(bot) && !dest->isFull(ignoreFull) && dest->distanceTo(&botLocation) < 2000)
                    retTravelLocations.push_back(dest);

            for (auto& dest : i->second->questObjectives)
                if (dest->isActive(bot) && !dest->isFull(ignoreFull) && dest->distanceTo(&botLocation) < 2000)
                    retTravelLocations.push_back(dest);
        }
    }

    return retTravelLocations;
}