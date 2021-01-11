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
        if (!bot->IsActiveQuest(questId))
            return false;

        if (!bot->CanRewardQuest(questTemplate, false))
            return false;

        if (sTravelMgr.getDialogStatus(bot, entry, questTemplate) != DIALOG_STATUS_REWARD2)
            return false;
    }

    return true;
}

bool QuestObjectiveTravelDestination::isActive(Player* bot) {
    if (questTemplate->GetQuestLevel() > bot->getLevel())
        return false;

    //Check mob level
    if (getEntry() > 0)
    {
        CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(getEntry());

        if (cInfo && (int)cInfo->MaxLevel - (int)bot->getLevel() > 4)
            return false;
    }

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
    for (auto& quest : quests)
    {
        for (auto& dest : quest.second->questGivers)
        {
            delete dest;
        }

        for (auto& dest : quest.second->questTakers)
        {
            delete dest;
        }

        for (auto& dest : quest.second->questObjectives)
        {
            delete dest;
        }       
    }

    quests.clear();
    pointsMap.clear();
}

void TravelMgr::logQuestError(uint32 errorNr, Quest* quest, uint32 objective, uint32 unitId, uint32 itemId)
{
    bool logQuestErrors = false; //For debugging.

    if (!logQuestErrors)
        return;

    if (errorNr == 1)
    {
        string unitName = "<unknown>";
        CreatureInfo const* cInfo = NULL;
        GameObjectInfo const* gInfo = NULL;

        if (unitId > 0)
            cInfo = ObjectMgr::GetCreatureTemplate(unitId);
        else
            gInfo = ObjectMgr::GetGameObjectInfo(unitId * -1);

        if (cInfo)
            unitName = cInfo->Name;
        else if (gInfo)
            unitName = gInfo->name;

        sLog.outString("Quest %s [%d] has %s %s [%d] but none is found in the world.", quest->GetTitle().c_str(), quest->GetQuestId(), objective == 0 ? "quest giver" : "quest taker", unitName.c_str(), unitId);
    }
    else if (errorNr == 2)
    {
        string unitName = "<unknown>";
        CreatureInfo const* cInfo = NULL;
        GameObjectInfo const* gInfo = NULL;

        if (unitId > 0)
            cInfo = ObjectMgr::GetCreatureTemplate(unitId);
        else
            gInfo = ObjectMgr::GetGameObjectInfo(unitId * -1);

        if (cInfo)
            unitName = cInfo->Name;
        else if (gInfo)
            unitName = gInfo->name;

        sLog.outErrorDb("Quest %s [%d] needs %s [%d] for objective %d but none is found in the world.", quest->GetTitle().c_str(), quest->GetQuestId(), unitName.c_str(), unitId, objective);
    }
    else if (errorNr == 3)
    {
        sLog.outErrorDb("Quest %s [%d] needs itemId %d but no such item exists.", quest->GetTitle().c_str(), quest->GetQuestId(), itemId);
    }
    else if (errorNr == 4)
    {
        ItemPrototype const* proto = sObjectMgr.GetItemPrototype(itemId);

        string unitName = "<unknown>";
        CreatureInfo const* cInfo = NULL;
        GameObjectInfo const* gInfo = NULL;

        if (unitId > 0)
            cInfo = ObjectMgr::GetCreatureTemplate(unitId);
        else
            gInfo = ObjectMgr::GetGameObjectInfo(unitId * -1);

        if (cInfo)
            unitName = cInfo->Name;
        else if (gInfo)
            unitName = gInfo->name;

        sLog.outString("Quest %s [%d] needs %s [%d] for loot of item %s [%d] for objective %d but none is found in the world.", quest->GetTitle().c_str(), quest->GetQuestId(), unitName.c_str(), unitId, proto->Name1, itemId, objective);
    } 
    else if (errorNr == 5)
    {
        ItemPrototype const* proto = sObjectMgr.GetItemPrototype(itemId);

        sLog.outString("Quest %s [%d] needs item %s [%d] for objective %d but none is found in the world.", quest->GetTitle().c_str(), quest->GetQuestId(), proto->Name1, itemId, objective);
    }
    else if (errorNr == 6)
    {
        sLog.outErrorDb("Quest %s [%d] has no quest giver.", quest->GetTitle().c_str(), quest->GetQuestId());
    }
    else if (errorNr == 7)
    {
        sLog.outErrorDb("Quest %s [%d] has no quest taker.", quest->GetTitle().c_str(), quest->GetQuestId());
    }
    else if (errorNr == 8)
    {
        sLog.outErrorDb("Quest %s [%d] has no quest viable quest objective.", quest->GetTitle().c_str(), quest->GetQuestId());
    }
}

void TravelMgr::LoadQuestTravelTable()
{
    // Clearing store (for reloading case)
    Clear();

    struct unit { uint32 guid; uint32 type; uint32 entry; uint32 map; float  x; float  y; float  z;  float  o; } t_unit;
    vector<unit> units;

    struct relation { uint32 type; uint32 role;  uint32 entry; uint32 questId; } t_rel;
    vector<relation> relations;

    struct loot { uint32 type; uint32 entry;  uint32 item; } t_loot;
    vector<loot> loots;

    ObjectMgr::QuestMap const& questMap = sObjectMgr.GetQuestTemplates();
    vector<uint32> questIds;

    for (auto& quest : questMap)
        questIds.push_back(quest.first);

    sort(questIds.begin(), questIds.end());

    string query = "SELECT 0,guid,id,map,position_x,position_y,position_z,orientation FROM creature c UNION ALL SELECT 1,guid,id,map,position_x,position_y,position_z,orientation FROM gameobject g";

    QueryResult* result = WorldDatabase.PQuery(query.c_str());

    if (result)
    {
        BarGoLink bar(result->GetRowCount());

        do
        {
            Field* fields = result->Fetch();
            bar.step();

            t_unit.type = fields[0].GetUInt32();
            t_unit.guid = fields[1].GetUInt32();
            t_unit.entry = fields[2].GetUInt32();
            t_unit.map = fields[3].GetUInt32();
            t_unit.x = fields[4].GetFloat();
            t_unit.y = fields[5].GetFloat();
            t_unit.z = fields[6].GetFloat();
            t_unit.o = fields[7].GetFloat();

            units.push_back(t_unit);

        } while (result->NextRow());

        delete result;

        sLog.outString(">> Loaded " SIZEFMTD " units locations.", units.size());
        sLog.outString();
    }
    else
    {
        sLog.outString();
        sLog.outErrorDb(">> Error loading units locations.");
    }

    query = "SELECT actor, role, entry, quest FROM quest_relations qr";

    result = WorldDatabase.PQuery(query.c_str());

    if (result)
    {
        BarGoLink bar(result->GetRowCount());

        do
        {
            Field* fields = result->Fetch();
            bar.step();

            t_rel.type = fields[0].GetUInt32();
            t_rel.role = fields[1].GetUInt32();
            t_rel.entry = fields[2].GetUInt32();
            t_rel.questId = fields[3].GetUInt32();

            relations.push_back(t_rel);

        } while (result->NextRow());

        delete result;

        sLog.outString(">> Loaded " SIZEFMTD " relations.", units.size());
        sLog.outString();
    }
    else
    {
        sLog.outString();
        sLog.outErrorDb(">> Error loading relations.");
    }

    query = "SELECT 0, ct.entry, item FROM creature_template ct JOIN creature_loot_template clt ON (ct.lootid = clt.entry) UNION ALL SELECT 0, entry, item FROM npc_vendor UNION ALL SELECT 1, gt.entry, item FROM gameobject_template gt JOIN gameobject_loot_template glt ON (gt.TYPE = 3 AND gt.DATA1 = glt.entry)";

    result = WorldDatabase.PQuery(query.c_str());

    if (result)
    {
        BarGoLink bar(result->GetRowCount());

        do
        {
            Field* fields = result->Fetch();
            bar.step();

            t_loot.type = fields[0].GetUInt32();
            t_loot.entry = fields[1].GetUInt32();
            t_loot.item = fields[2].GetUInt32();

            loots.push_back(t_loot);

        } while (result->NextRow());

        delete result;

        sLog.outString(">> Loaded " SIZEFMTD " loot lists.", units.size());
        sLog.outString();
    }
    else
    {
        sLog.outString();
        sLog.outErrorDb(">> Error loading loot lists.");
    }

    BarGoLink bar(questIds.size());

    for (auto& questId : questIds)
    {

        bar.step();

        Quest* quest = questMap.find(questId)->second;

        QuestContainer* container = new QuestContainer;
        QuestTravelDestination* loc;
        WorldPosition point;

        bool hasError = false;

        //Relations
        for (auto& r : relations)
        {
            if (questId != r.questId)
                continue;

            int32 entry = r.type == 0 ? r.entry : r.entry * -1;

            loc = new QuestRelationTravelDestination(r.questId, entry, r.role, sPlayerbotAIConfig.tooCloseDistance, sPlayerbotAIConfig.sightDistance);
            loc->setExpireDelay(5 * 60 * 1000);
            loc->setMaxVisitors(15, 0);

            for (auto& u : units)
            {
                if (r.type != u.type || r.entry != u.entry)
                    continue;

                int32 guid = u.type == 0 ? u.guid : u.guid * -1;

                point = WorldPosition(u.map, u.x, u.y, u.z, u.o);
                pointsMap.insert(make_pair(guid, point));

                loc->addPoint(&pointsMap.find(guid)->second);
            }

            if (loc->getPoints(0).empty())
            {
                logQuestError(1, quest, r.role, entry);
                delete loc;
                continue;
            }


            if (r.role == 0)
            {
                container->questGivers.push_back(loc);
            }
            else
                container->questTakers.push_back(loc);

        }

        //Mobs
        for (int i = 0; i < 4; i++)
        {
            if (quest->ReqCreatureOrGOCount[i] == 0)
                continue;

            uint32 reqEntry = quest->ReqCreatureOrGOId[i];

            loc = new QuestObjectiveTravelDestination(questId, reqEntry, i, sPlayerbotAIConfig.tooCloseDistance, sPlayerbotAIConfig.sightDistance);
            loc->setExpireDelay(1 * 60 * 1000);
            loc->setMaxVisitors(100, 1);

            for (auto& u : units)
            {
                int32 entry = u.type > 0 ? u.entry : u.entry * 1;

                if (entry != reqEntry)
                    continue;

                int32 guid = u.type == 0 ? u.guid : u.guid * -1;

                point = WorldPosition(u.map, u.x, u.y, u.z, u.o);
                pointsMap.insert(make_pair(guid, point));

                loc->addPoint(&pointsMap.find(guid)->second);
            }

            if (loc->getPoints(0).empty())
            {
                logQuestError(2, quest, i, reqEntry);

                delete loc;
                hasError = true;
                continue;
            }
            
            container->questObjectives.push_back(loc);
        }

        //Loot
        for (int i = 0; i < 4; i++)
        {
            if (quest->ReqItemCount[i] == 0)
                continue;

            ItemPrototype const* proto = sObjectMgr.GetItemPrototype(quest->ReqItemId[i]);

            if (!proto)
            {
                logQuestError(3, quest, i, 0, quest->ReqItemId[i]);
                hasError = true;
                continue;
            }

            uint32 foundLoot = 0;

            for (auto& l : loots)
            {
                if (l.item != quest->ReqItemId[i])
                    continue;

                int32 entry = l.type == 0 ? l.entry : l.entry * -1;

                loc = new QuestObjectiveTravelDestination(questId, entry, i, sPlayerbotAIConfig.tooCloseDistance, sPlayerbotAIConfig.sightDistance);
                loc->setExpireDelay(1 * 60 * 1000);
                loc->setMaxVisitors(100, 1);

                for (auto& u : units)
                {
                    if (l.type != u.type || l.entry != u.entry)
                        continue;

                    int32 guid = u.type == 0 ? u.guid : u.guid * -1;

                    point = WorldPosition(u.map, u.x, u.y, u.z, u.o);
                    pointsMap.insert(make_pair(guid, point));

                    loc->addPoint(&pointsMap.find(guid)->second);
                }

                if (loc->getPoints(0).empty())
                {
                    logQuestError(4, quest, i, entry, quest->ReqItemId[i]);
                    delete loc;
                    continue;
                }

                container->questObjectives.push_back(loc);

                foundLoot++;
            }

            if (foundLoot == 0)
            {
                hasError = true;
                logQuestError(5, quest, i, 0, quest->ReqItemId[i]);
            }
        }

        if (container->questTakers.empty())
            logQuestError(7, quest);

        if(!container->questGivers.empty() || !container->questTakers.empty() || hasError)
        {
            quests.insert_or_assign(questId, container);

            for (auto loc : container->questGivers)
                questGivers.push_back(loc);
        }
    }

    sLog.outString(">> Loaded " SIZEFMTD " quest details.", questIds.size());
}

uint32 TravelMgr::getDialogStatus(Player* pPlayer, uint32 questgiver, Quest const* pQuest)
{
    uint32 dialogStatus = DIALOG_STATUS_NONE;

    QuestRelationsMapBounds rbounds;                        // QuestRelations (quest-giver)
    QuestRelationsMapBounds irbounds;                       // InvolvedRelations (quest-finisher)

    uint32 questId = pQuest->GetQuestId();

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
        if (itr->second != questId)
            continue;

        uint32 dialogStatusNew = DIALOG_STATUS_NONE;

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
        if (itr->second != questId)
            continue;

        uint32 dialogStatusNew = DIALOG_STATUS_NONE;

        if (!pQuest || !pQuest->IsActive())
        {
            continue;
        }

        QuestStatus status = pPlayer->GetQuestStatus(questId);

        if (status == QUEST_STATUS_NONE)                    // For all other cases the mark is handled either at some place else, or with involved-relations already
        {
            if (pPlayer->CanSeeStartQuest(pQuest))
            {
                if (pPlayer->SatisfyQuestLevel(pQuest, false))
                {
                    int32 lowLevelDiff = sWorld.getConfig(CONFIG_INT32_QUEST_LOW_LEVEL_HIDE_DIFF);
                    if (pQuest->IsAutoComplete() || (pQuest->IsRepeatable() && pPlayer->getQuestStatusMap()[questId].m_rewarded))
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
            {
                //bool act = dest->isActive(bot);
               // bool full = dest->isFull(ignoreFull);
                //float dist = dest->distanceTo(&botLocation);

               // bot->Say(dest->GetQuestTemplate()->GetTitle() + " is " + (act ? "active" : "inactive") + " and " + (act ? "active" : "inactive") + " at " + to_string(dist) , 0);

               // QuestTravelDestination* des = dest;

                if (dest->isActive(bot) && !dest->isFull(ignoreFull) && dest->distanceTo(&botLocation) < 2000)
                    retTravelLocations.push_back(dest);
            }
        }
    }

    return retTravelLocations;
}