#pragma once

#include <Maps/TransportMgr.h>

#include "TravelMgr.h"
#include "TravelNode.h"
#include "PlayerbotAI.h"
#include "TravelNodeStore.h"

#include "ObjectMgr.h"
#include <numeric>
#include <iomanip>

#include "MotionGenerators/PathFinder.h"
#include "PlayerbotAI.h"
#include "vmap/VMapFactory.h"
#include "MotionGenerators/MoveMap.h"
#include "Entities/Transports.h"

using namespace ai;
using namespace MaNGOS;

WorldPosition::WorldPosition(vector<WorldPosition*> list, WorldPositionConst conType)
{
    uint32 size = list.size();
    if (size == 0)
        return;
    else if (size == 1)
        wLoc = list.front()->wLoc;
    else if (conType == WP_RANDOM)
        wLoc = list[urand(0, size - 1)]->wLoc;
    else if (conType == WP_CENTROID)
        wLoc = std::accumulate(list.begin(), list.end(), WorldLocation(list[0]->getMapId(), 0, 0, 0, 0), [size](WorldLocation i, WorldPosition* j) {i.coord_x += j->getX() / size; i.coord_y += j->getY() / size; i.coord_z += j->getZ() / size; i.orientation += j->getO() / size; return i; });
    else if (conType == WP_MEAN_CENTROID)
    {
        WorldPosition pos = WorldPosition(list, WP_CENTROID);
        wLoc = pos.closestSq(list)->wLoc;
    }
}

WorldPosition::WorldPosition(vector<WorldPosition> list, WorldPositionConst conType)
{    
    uint32 size = list.size();
    if (size == 0)
        return;
    else if (size == 1)
        wLoc = list.front().wLoc;
    else if (conType == WP_RANDOM)
        wLoc = list[urand(0, size - 1)].wLoc;
    else if (conType == WP_CENTROID)
        wLoc = std::accumulate(list.begin(), list.end(), WorldLocation(list[0].getMapId(), 0, 0, 0, 0), [size](WorldLocation i, WorldPosition j) {i.coord_x += j.getX() / size; i.coord_y += j.getY() / size; i.coord_z += j.getZ() / size; i.orientation += j.getO() / size; return i; });
    else if (conType == WP_MEAN_CENTROID)
    {
        WorldPosition pos = WorldPosition(list, WP_CENTROID);
        wLoc = pos.closestSq(list).wLoc;
    }    
}


float WorldPosition::distance(WorldPosition* center)
{
    if(wLoc.mapid == center->getMapId())
        return relPoint(center).size(); 

    //this -> mapTransfer | mapTransfer -> center
    return sTravelMgr.mapTransDistance(*this, *center);
};

//When moving from this along list return last point that falls within range.
//Distance is move distance along path.
WorldPosition WorldPosition::lastInRange(vector<WorldPosition> list, float minDist, float maxDist)
{
    WorldPosition rPoint;

    float startDist = 0.0f;

    //Enter the path at the closest point.
    for (auto& p : list)
    {
        float curDist = distance(p);
        if (startDist < curDist || p == list.front())
            startDist = curDist + 0.1f;
    }

    float totalDist = 0.0f;

    //Follow the path from the last nearest point
    //Return last point in range.
    for (auto& p : list)
    {
        float curDist = distance(p);

        if (totalDist > 0) //We have started the path. Keep counting.
            totalDist += p.distance(std::prev(&p, 1));

        if (curDist == startDist) //Start the path here.
            totalDist = startDist;

        if (minDist > 0 && totalDist < minDist)
            continue;

        if (maxDist > 0 && totalDist > maxDist)
            continue; //We do not break here because the path may loop back and have a second startDist point.

        rPoint = p;
    }

    return rPoint;
};

//Todo: remove or adjust to above standard.
WorldPosition WorldPosition::firstOutRange(vector<WorldPosition> list, float minDist, float maxDist)
{
    WorldPosition rPoint;

    for (auto& p : list)
    {
        if (minDist > 0 && distance(p) < minDist)
            return p;

        if (maxDist > 0 && distance(p) > maxDist)
            return p;

        rPoint = p;
    }

    return rPoint;
}

//Returns true if (on the x-y plane) the position is inside the three points.
bool WorldPosition::isInside(WorldPosition* p1, WorldPosition* p2, WorldPosition* p3)
{
    if (getMapId() != p1->getMapId() != p2->getMapId() != p3->getMapId())
        return false;

    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = mSign(p1, p2);
    d2 = mSign(p2, p3);
    d3 = mSign(p3, p1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

G3D::Vector3 WorldPosition::getVector3() 
{
    return G3D::Vector3(wLoc.coord_x, wLoc.coord_y, wLoc.coord_z); 
}

string WorldPosition::print()
{
    ostringstream out;
    out << wLoc.mapid << std::fixed << std::setprecision(2);
    out << ';'<< wLoc.coord_x;
    out << ';' << wLoc.coord_y;
    out << ';' << wLoc.coord_z;
    out << ';' << wLoc.orientation;

    return out.str();
}

void WorldPosition::printWKT(vector<WorldPosition> points, ostringstream& out, uint32 dim, bool loop)
{
    switch (dim) {
    case 0:
        if(points.size() == 1)
            out << "\"POINT(";
        else
            out << "\"MULTIPOINT(";
        break;
    case 1:
        out << "\"LINESTRING (";
        break;
    case 2:
        out << "\"POLYGON ((";
    }

    for (auto& p : points)
        out << p.getDisplayX() << " " << p.getDisplayY() << (!loop && &p == &points.back() ? "" : ",");

    if (loop)
        out << points.front().getDisplayX() << " " << points.front().getDisplayY();

    out << (dim == 2 ? "))\"," : ")\",");
}

WorldPosition WorldPosition::getDisplayLocation() 
{ 
    return offset(&sTravelNodeMap.getMapOffset(getMapId())); 
};

AreaTableEntry const* WorldPosition::getArea()
{
    uint16 areaFlag = getAreaFlag();

    if(!areaFlag)
        return NULL;

    return GetAreaEntryByAreaFlagAndMap(areaFlag, getMapId());
}

string WorldPosition::getAreaName(bool fullName, bool zoneName)
{    
    if (!isOverworld())
    {
        MapEntry const* map = sMapStore.LookupEntry(getMapId());
        if (map)
            return map->name[0];
    }

    AreaTableEntry const* area = getArea();

    if (!area)
        return "";

    string areaName = area->area_name[0];

    if (fullName)
    {
        uint16 zoneId = area->zone;

        while (zoneId > 0)
        {
            AreaTableEntry const* parentArea = GetAreaEntryByAreaID(zoneId);

            if (!parentArea)
                break;

            string subAreaName = parentArea->area_name[0];

            if (zoneName)
                areaName = subAreaName;
            else
                areaName = subAreaName + " " + areaName;

            zoneId = parentArea->zone;
        }
    }

    return areaName;
}

std::set<Transport*> WorldPosition::getTransports(uint32 entry)
{
    /*
    if(!entry)
        return getMap()->m_transports;
    else
    {
    */
        std::set<Transport*> transports;
        /*
        for (auto transport : getMap()->m_transports)
            if(transport->GetEntry() == entry)
                transports.insert(transport);

        return transports;
    }
    */
    return transports;
}

std::pair<int, int> WorldPosition::getGrid()
{
    std::pair<int, int> xy;
    xy.first = (int)(32 - getX() / SIZE_OF_GRIDS);
    xy.second = (int)(32 - getY() / SIZE_OF_GRIDS);

    return xy;
}

std::vector<pair<int, int>> WorldPosition::getGrids(WorldPosition secondPos)
{
    std::vector<pair<int, int>> retVec;

    int lx = std::min(getGrid().first, secondPos.getGrid().first);
    int ly = std::min(getGrid().second, secondPos.getGrid().second);
    int ux = std::max(getGrid().first, secondPos.getGrid().first);
    int uy = std::max(getGrid().second, secondPos.getGrid().second);
    
    for (int x = lx-1; x <= ux+1; x++)
    {
        for (int y = ly-1; y <= uy+1; y++)
        {
            retVec.push_back(make_pair(x, y));
        }
    }

    return retVec;
}

vector<WorldPosition> WorldPosition::fromGrid(int x, int y)
{
    vector<WorldPosition> retVec;
    for (uint32 d = 0; d < 4; d++)
    {
        float dx = x;
        float dy = y;

        if (d == 1 || d == 2)
            dx += 1;
        if (d == 2 || d == 3)
            dy += 1;
        
        dx = 32 - dx;
        dy = 32 - dy;

        dx *=SIZE_OF_GRIDS;
        dy *= SIZE_OF_GRIDS;

        retVec.push_back(WorldPosition(getMapId(), dx, dy, getZ(), getO()));
    }
   
    return retVec;
}

void WorldPosition::loadMapAndVMap(uint32 mapId, int x, int y)
{
    string fileName = "load_map_grid.csv";

    if ( !VMAP::VMapFactory::createOrGetVMapManager()->IsTileLoaded(mapId, x, y) && !sTravelMgr.isBadVmap(mapId, x, y))
    {
        // load VMAPs for current map/grid...
        const MapEntry* i_mapEntry = sMapStore.LookupEntry(mapId);
        const char* mapName = i_mapEntry ? i_mapEntry->name[sWorld.GetDefaultDbcLocale()] : "UNNAMEDMAP\x0";

        int vmapLoadResult = VMAP::VMapFactory::createOrGetVMapManager()->loadMap((sWorld.GetDataPath() + "vmaps").c_str(), mapId, x, y);
        switch (vmapLoadResult)
        {
        case VMAP::VMAP_LOAD_RESULT_OK:
            //sLog.outError("VMAP loaded name:%s, id:%d, x:%d, y:%d (vmap rep.: x:%d, y:%d)", mapName, mapId, x, y, x, y);
            break;
        case VMAP::VMAP_LOAD_RESULT_ERROR:
            //sLog.outError("Could not load VMAP name:%s, id:%d, x:%d, y:%d (vmap rep.: x:%d, y:%d)", mapName, mapId, x, y, x, y);
            sTravelMgr.addBadVmap(mapId, x, y);
            break;
        case VMAP::VMAP_LOAD_RESULT_IGNORED:
            sTravelMgr.addBadVmap(mapId, x, y);
            //sLog.outError("Ignored VMAP name:%s, id:%d, x:%d, y:%d (vmap rep.: x:%d, y:%d)", mapName, mapId, x, y, x, y);
            break;
        }

        if (sPlayerbotAIConfig.hasLog(fileName))
        {
            ostringstream out;
            out << sPlayerbotAIConfig.GetTimestampStr();
            out << "+00,\"vmap\", " << x << "," << y << ", " << (sTravelMgr.isBadVmap(mapId, x, y) ? "0": "1") << ",";
            printWKT(fromGrid(x, y), out, 1, true);
            sPlayerbotAIConfig.log(fileName, out.str().c_str());
        }
    }

    if (!MMAP::MMapFactory::createOrGetMMapManager()->IsMMapIsLoaded(mapId, x, y) && !sTravelMgr.isBadMmap(mapId, x, y))
    {
        // load navmesh
        if(!MMAP::MMapFactory::createOrGetMMapManager()->loadMap(mapId, x, y))
            sTravelMgr.addBadMmap(mapId, x, y);

        if (sPlayerbotAIConfig.hasLog(fileName))
        {
            ostringstream out;
            out << sPlayerbotAIConfig.GetTimestampStr();
            out << "+00,\"mmap\", " << x << "," << y << "," << (sTravelMgr.isBadMmap(mapId, x, y) ? "0" : "1") << ",";
            printWKT(fromGrid(x, y), out, 1, true);
            sPlayerbotAIConfig.log(fileName, out.str().c_str());
        }
    }
}

void WorldPosition::loadMapAndVMaps(WorldPosition secondPos)
{
    for (auto& grid : getGrids(secondPos))
    {
        loadMapAndVMap(getMapId(), grid.first, grid.second);
    }
}

vector<WorldPosition> WorldPosition::fromPointsArray(std::vector<G3D::Vector3> path)
{
    vector<WorldPosition> retVec;
    for (auto p : path)
        retVec.push_back(WorldPosition(getMapId(), p.x, p.y, p.z, getO()));

    return retVec;
}

//A single pathfinding attempt from one position to another. Returns pathfinding status and path.
vector<WorldPosition> WorldPosition::getPathStepFrom(WorldPosition startPos, Unit* bot)
{
    //Load mmaps and vmaps between the two points.
    loadMapAndVMaps(startPos);

    PointsArray points;
    PathType type;

    if (bot)
    {
        PathFinder path(bot);

#ifdef IKE_PATHFINDER
        path.setAreaCost(8, 10.0f);  //Water
        path.setAreaCost(11, 5.0f);  //Mob proximity
        path.setAreaCost(12, 20.0f); //Mob agro
#endif
        path.calculate(startPos.getVector3(), getVector3(), false);

        points = path.getPath();
        type = path.getPathType();

    }
    else
#ifdef IKE_PATHFINDER
    {
        std::hash<std::thread::id> hasher;
        PathFinder path(getMapId(), hasher(std::this_thread::get_id()));

        path.setAreaCost(8, 10.0f);
        path.setAreaCost(11, 5.0f);
        path.setAreaCost(12, 20.0f);

        path.calculate(startPos.getVector3(), getVector3(), false);

        points = path.getPath();
        type = path.getPathType();
    }
#else
        return PATHFIND_NOPATH;
#endif

    if (sPlayerbotAIConfig.hasLog("pathfind_attempt_point.csv"))
    {
        ostringstream out;
        out << std::fixed << std::setprecision(1);
        printWKT({ startPos, *this }, out);
        sPlayerbotAIConfig.log("pathfind_attempt_point.csv", out.str().c_str());
    }

    if (sPlayerbotAIConfig.hasLog("pathfind_attempt.csv") && (type == PATHFIND_INCOMPLETE || type == PATHFIND_NORMAL))
    {
        ostringstream out;
        out << sPlayerbotAIConfig.GetTimestampStr() << "+00,";
        out << std::fixed << std::setprecision(1) << type << ",";
        printWKT(fromPointsArray(points), out, 1);
        sPlayerbotAIConfig.log("pathfind_attempt.csv", out.str().c_str());
    }    

    if (type == PATHFIND_INCOMPLETE || type == PATHFIND_NORMAL)
        return fromPointsArray(points);
    else
        return {};

}

//A sequential series of pathfinding attempts. Returns the complete path and if the patfinder eventually found a way to the destination.
vector<WorldPosition> WorldPosition::getPathFromPath(vector<WorldPosition> startPath, Unit* bot)
{
    //We start at the end of the last path.
    WorldPosition currentPos = startPath.back();

    //No pathfinding across maps.
    if (getMapId() != currentPos.getMapId())
        return { };

    vector<WorldPosition> subPath, fullPath = startPath;

    //Limit the pathfinding attempts to 40
    for (uint32 i = 0; i < 40; i++)
    {
        //Try to pathfind to this position.
        subPath = getPathStepFrom(currentPos, bot);

        //If we could not find a path return what we have now.
        if (subPath.empty() || currentPos.distance(&subPath.back()) < sPlayerbotAIConfig.targetPosRecalcDistance)
            break;
        
        //Append the path excluding the start (this should be the same as the end of the startPath)
        fullPath.insert(fullPath.end(), std::next(subPath.begin(),1), subPath.end());

        //Are we there yet?
        if (isPathTo(subPath))
            break;

        //Continue pathfinding.
        currentPos = subPath.back();
    }

    return fullPath;
}


bool FindPointCreatureData::operator()(CreatureDataPair const& dataPair)
{
    if (!entry || dataPair.second.id == entry)
        if ((!point || dataPair.second.mapid == point.getMapId()) && (!radius || point.sqDistance(WorldPosition(dataPair.second.mapid, dataPair.second.posX, dataPair.second.posY, dataPair.second.posZ)) < radius * radius))
        {
            data.push_back(&dataPair);
        }

    return false;
}

bool FindPointGameObjectData::operator()(GameObjectDataPair const& dataPair)
{
    if (!entry || dataPair.second.id == entry)
        if ((!point || dataPair.second.mapid == point.getMapId()) && (!radius || point.sqDistance(WorldPosition(dataPair.second.mapid, dataPair.second.posX, dataPair.second.posY, dataPair.second.posZ)) < radius * radius))
        {
            data.push_back(&dataPair);
        }

    return false;
}

vector<CreatureDataPair const*> WorldPosition::getCreaturesNear(float radius, uint32 entry)
{
    FindPointCreatureData worker(*this, radius, entry);
    sObjectMgr.DoCreatureData(worker);
    return worker.GetResult();
}

vector<GameObjectDataPair const*> WorldPosition::getGameObjectsNear(float radius, uint32 entry)
{
    FindPointGameObjectData worker(*this, radius, entry);
    sObjectMgr.DoGOData(worker);
    return worker.GetResult();
}

vector<WorldPosition*> TravelDestination::getPoints(bool ignoreFull) {
    if (ignoreFull)
        return points;

    uint32 max = maxVisitorsPerPoint;

    if (max == 0)
        return points;

    vector<WorldPosition*> retVec;
    std::copy_if(points.begin(), points.end(), std::back_inserter(retVec), [max](WorldPosition* p) { return p->getVisitors() < max; });
    return retVec;
}

WorldPosition* TravelDestination::nearestPoint(WorldPosition* pos) {
    return *std::min_element(points.begin(), points.end(), [pos](WorldPosition* i, WorldPosition* j) {return i->distance(pos) < j->distance(pos); });
}

vector<WorldPosition*> TravelDestination::touchingPoints(WorldPosition* pos) {
    vector<WorldPosition*> ret_points;
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

vector<WorldPosition*> TravelDestination::sortedPoints(WorldPosition* pos) {
    vector<WorldPosition*> ret_points = points;

    std::sort(ret_points.begin(), ret_points.end(), [pos](WorldPosition* i, WorldPosition* j) {return i->distance(pos) < j->distance(pos); });

    return ret_points;
};

vector <WorldPosition*> TravelDestination::nextPoint(WorldPosition* pos, bool ignoreFull) {
    return sTravelMgr.getNextPoint(pos, ignoreFull ? points : getPoints());
}

bool TravelDestination::isFull(bool ignoreFull) {
    if (!ignoreFull && maxVisitors > 0 && visitors >= maxVisitors)
        return true;

    if (maxVisitorsPerPoint > 0)
        if (getPoints(ignoreFull).empty())
            return true;

    return false;
}

string QuestTravelDestination::getTitle() {
    return ChatHelper::formatQuest(questTemplate);
}

bool QuestRelationTravelDestination::isActive(Player* bot) {
    if (relation == 0)
    {
        if (questTemplate->GetQuestLevel() >= bot->getLevel() + 5)
            return false;
        //if (questTemplate->XPValue(bot) == 0)
        //    return false;
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

#ifdef MANGOSBOT_ZERO
        if (sTravelMgr.getDialogStatus(bot, entry, questTemplate) != DIALOG_STATUS_REWARD2 && sTravelMgr.getDialogStatus(bot, entry, questTemplate) != DIALOG_STATUS_REWARD_REP)
#else
        if (sTravelMgr.getDialogStatus(bot, entry, questTemplate) != DIALOG_STATUS_REWARD2 && sTravelMgr.getDialogStatus(bot, entry, questTemplate) != DIALOG_STATUS_REWARD && sTravelMgr.getDialogStatus(bot, entry, questTemplate) != DIALOG_STATUS_REWARD_REP)
#endif
            return false;
    }

    return true;
}

string QuestRelationTravelDestination::getTitle() {
    ostringstream out;

    if (relation == 0)
        out << "questgiver";
    else
        out << "questtaker";

    out << " " << ChatHelper::formatWorldEntry(entry);
    return out.str();
}

bool QuestObjectiveTravelDestination::isActive(Player* bot) {
    if (questTemplate->GetQuestLevel() > bot->getLevel() + 1)
        return false;

    //Check mob level
    if (getEntry() > 0)
    {
        CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(getEntry());

        if (cInfo && (int)cInfo->MaxLevel - (int)bot->getLevel() > 4)
            return false;
    }

    if (questTemplate->GetType() == QUEST_TYPE_ELITE && !bot->GetGroup())
        return false;

    return sTravelMgr.getObjectiveStatus(bot, questTemplate, objective);
}

string QuestObjectiveTravelDestination::getTitle() {
    ostringstream out;

    out << "objective " << objective;

    if (itemId)
        out << " loot " << ChatHelper::formatItem(sObjectMgr.GetItemPrototype(itemId), 0, 0) << " from";
    else
        out << " to kill";

    out << " " << ChatHelper::formatWorldEntry(entry);
    return out.str();
}

bool RpgTravelDestination::isActive(Player* bot)
{
    PlayerbotAI* ai = bot->GetPlayerbotAI();
    AiObjectContext* context = ai->GetAiObjectContext();

    if (AI_VALUE(uint8, "bag space") <= 80 && (AI_VALUE(uint8, "durability") >= 80 || AI_VALUE(uint32, "repair cost") > bot->GetMoney()))
        return false;

    //Once the target rpged with it is added to the ignore list. We can now move on.
    set<ObjectGuid>& ignoreList = bot->GetPlayerbotAI()->GetAiObjectContext()->GetValue<set<ObjectGuid>&>("ignore rpg target")->Get();

    for (auto& i : ignoreList)
    {
        if (i.GetEntry() == getEntry())
        {
            return false;
        }
    }

    CreatureInfo const* cInfo = this->getCreatureInfo();
    FactionTemplateEntry const* factionEntry = sFactionTemplateStore.LookupEntry(cInfo->Faction);
    ReputationRank reaction = ai->getReaction(factionEntry);

    return reaction > REP_NEUTRAL;
}

string RpgTravelDestination::getTitle() {
    ostringstream out;

    out << "rpg npc ";

    out << " " << ChatHelper::formatWorldEntry(entry);

    return out.str();
}

bool ExploreTravelDestination::isActive(Player* bot)
{
    //return true;

    AreaTableEntry const* area = GetAreaEntryByAreaID(areaId);

    if (area->area_level && (uint32)area->area_level > bot->getLevel() && bot->getLevel() < DEFAULT_MAX_LEVEL)
        return false;

    if (area->exploreFlag == 0xffff)
        return false;
    int offset = area->exploreFlag / 32;

    uint32 val = (uint32)(1 << (area->exploreFlag % 32));
    uint32 currFields = bot->GetUInt32Value(PLAYER_EXPLORED_ZONES_1 + offset);

    return !(currFields & val);    
}

string ExploreTravelDestination::getTitle()
{
    return points[0]->getAreaName();
};

TravelTarget::~TravelTarget() {
    if (!tDestination)
        return;

    releaseVisitors();
    //sTravelMgr.botTargets.erase(std::remove(sTravelMgr.botTargets.begin(), sTravelMgr.botTargets.end(), this), sTravelMgr.botTargets.end());
}

void TravelTarget::setTarget(TravelDestination* tDestination1, WorldPosition* wPosition1, bool groupCopy1) {
    releaseVisitors();

    wPosition = wPosition1;
    tDestination = tDestination1;
    groupCopy = groupCopy1;

    addVisitors();

    setStatus(TRAVEL_STATUS_TRAVEL);
}

void TravelTarget::copyTarget(TravelTarget* target) {
    setTarget(target->tDestination, target->wPosition);
    groupCopy = target->isGroupCopy();
    extendRetryCount = target->extendRetryCount;
}

void TravelTarget::addVisitors() {
    if (!visitor)
    {
        wPosition->addVisitor();
        tDestination->addVisitor();
    }

    visitor = true;
}

void TravelTarget::releaseVisitors() {
    if (visitor)
    {
        if (tDestination)
            tDestination->remVisitor();
        if (wPosition)
            wPosition->remVisitor();
    }

    visitor = false;
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

    if (isTraveling() && forced)
        return true;

    if ((statusTime > 0 && startTime + statusTime < WorldTimer::getMSTime()))
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

    if (!tDestination->isActive(bot) && !forced) //Target has become invalid. Stop.
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

TravelState TravelTarget::getTravelState() {
    if (!tDestination || tDestination->getName() == "NullTravelDestination")
        return TRAVEL_STATE_IDLE;

    if (tDestination->getName() == "QuestRelationTravelDestination")
    {
        if (((QuestRelationTravelDestination*)tDestination)->getRelation() == 0)
        {
            if (isTraveling() || isPreparing())
                return TRAVEL_STATE_TRAVEL_PICK_UP_QUEST;
            if (isWorking())
                return TRAVEL_STATE_WORK_PICK_UP_QUEST;
        }
        else
        {
            if (isTraveling() || isPreparing())
                return TRAVEL_STATE_TRAVEL_HAND_IN_QUEST;
            if (isWorking())
                return TRAVEL_STATE_WORK_HAND_IN_QUEST;
        }
    }
    else if (tDestination->getName() == "QuestObjectiveTravelDestination")
    {
        if (isTraveling() || isPreparing())
            return TRAVEL_STATE_TRAVEL_DO_QUEST;
        if (isWorking())
            return TRAVEL_STATE_WORK_DO_QUEST;
    }
    else if (tDestination->getName() == "RpgTravelDestination")
    {
        return TRAVEL_STATE_TRAVEL_RPG;
    }
    else if (tDestination->getName() == "ExploreTravelDestination")
    {
        return TRAVEL_STATE_TRAVEL_EXPLORE;
    }

    return TRAVEL_STATE_IDLE;
}

void TravelMgr::Clear()
{
#ifdef MANGOS
    sObjectAccessor.DoForAllPlayers([this](Player* plr) { TravelMgr::setNullTravelTarget(plr); });
#endif
#ifdef CMANGOS
#ifndef MANGOSBOT_ZERO
    sObjectAccessor.ExecuteOnAllPlayers([this](Player* plr) { TravelMgr::setNullTravelTarget(plr); });
#else
    HashMapHolder<Player>::ReadGuard g(HashMapHolder<Player>::GetLock());
    HashMapHolder<Player>::MapType& m = sObjectAccessor.GetPlayers();
    for (HashMapHolder<Player>::MapType::iterator itr = m.begin(); itr != m.end(); ++itr)
        TravelMgr::setNullTravelTarget(itr->second);
#endif
#endif

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

    questGivers.clear();
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
    if (!sTravelMgr.quests.empty())
        return;
    // Clearing store (for reloading case)
    Clear();

    struct unit { uint32 guid; uint32 type; uint32 entry; uint32 map; float  x; float  y; float  z;  float  o; uint32 c; } t_unit;
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
    //                     0    1  2   3          4          5          6           7     8
    string query = "SELECT 0,guid,id,map,position_x,position_y,position_z,orientation, (select count(*) from creature k where c.id = k.id) FROM creature c UNION ALL SELECT 1,guid,id,map,position_x,position_y,position_z,orientation, (select count(*) from gameobject h where h.id = g.id)  FROM gameobject g";

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
            t_unit.c = fields[8].GetUInt32();

            units.push_back(t_unit);

        } while (result->NextRow());

        delete result;

        sLog.outString(">> Loaded " SIZEFMTD " units locations.", units.size());
    }
    else
    {
        sLog.outString();
        sLog.outErrorDb(">> Error loading units locations.");
    }

#ifdef MANGOS
    query = "SELECT actor, role, entry, quest FROM quest_relations qr";
#endif
#ifdef CMANGOS
    query = "SELECT 0, 0, id, quest FROM creature_questrelation UNION ALL SELECT 0, 1, id, quest FROM creature_involvedrelation UNION ALL SELECT 1, 0, id, quest FROM gameobject_questrelation UNION ALL SELECT 1, 1, id, quest FROM gameobject_involvedrelation";
#endif

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

        sLog.outString(">> Loaded " SIZEFMTD " relations.", relations.size());
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

        sLog.outString(">> Loaded " SIZEFMTD " loot lists.", loots.size());
    }
    else
    {
        sLog.outString();
        sLog.outErrorDb(">> Error loading loot lists.");
    }

    bool loadQuestData = true;


    if (loadQuestData)
    {
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
            for (uint32 i = 0; i < 4; i++)
            {
                if (quest->ReqCreatureOrGOCount[i] == 0)
                    continue;

                uint32 reqEntry = quest->ReqCreatureOrGOId[i];

                loc = new QuestObjectiveTravelDestination(questId, reqEntry, i, sPlayerbotAIConfig.tooCloseDistance, sPlayerbotAIConfig.sightDistance);
                loc->setExpireDelay(1 * 60 * 1000);
                loc->setMaxVisitors(100, 1);

                for (auto& u : units)
                {
                    int32 entry = u.type == 0 ? u.entry : u.entry * -1;

                    if (entry != reqEntry)
                        continue;

                    int32 guid = u.type == 0 ? u.guid : u.guid * -1;

                    point = WorldPosition(u.map, u.x, u.y, u.z, u.o);
                    pointsMap.insert(make_pair(u.guid, point));

                    loc->addPoint(&pointsMap.find(u.guid)->second);
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
            for (uint32 i = 0; i < 4; i++)
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

                    loc = new QuestObjectiveTravelDestination(questId, entry, i, sPlayerbotAIConfig.tooCloseDistance, sPlayerbotAIConfig.sightDistance, l.item);
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

            if (!container->questGivers.empty() || !container->questTakers.empty() || hasError)
            {
                quests.insert(make_pair(questId, container));

                for (auto loc : container->questGivers)
                    questGivers.push_back(loc);
            }
        }

        sLog.outString(">> Loaded " SIZEFMTD " quest details.", questIds.size());
    }

    WorldPosition point;

    //Rpg locations
    for (auto& u : units)
    {
        RpgTravelDestination* loc;

        if (u.type != 0)
            continue;

        CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(u.entry);

        if (!cInfo)
            continue;

        vector<uint32> allowedNpcFlags;

        allowedNpcFlags.push_back(UNIT_NPC_FLAG_INNKEEPER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_GOSSIP);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_QUESTGIVER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_FLIGHTMASTER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_BANKER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_AUCTIONEER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_STABLEMASTER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_PETITIONER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_TABARDDESIGNER);

        allowedNpcFlags.push_back(UNIT_NPC_FLAG_TRAINER);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_VENDOR);
        allowedNpcFlags.push_back(UNIT_NPC_FLAG_REPAIR);

        for (vector<uint32>::iterator i = allowedNpcFlags.begin(); i != allowedNpcFlags.end(); ++i)
        {
            if ((cInfo->NpcFlags & *i) != 0)
            {
                loc = new RpgTravelDestination(u.entry, sPlayerbotAIConfig.tooCloseDistance, sPlayerbotAIConfig.sightDistance);
                loc->setExpireDelay(5 * 60 * 1000);
                loc->setMaxVisitors(15, 0);

                point = WorldPosition(u.map, u.x, u.y, u.z, u.o);
                pointsMap.insert_or_assign(u.guid, point);
                loc->addPoint(&pointsMap.find(u.guid)->second);
                rpgNpcs.push_back(loc);
                break;
            }
        }
    }

    //Explore points
    for (auto& u : units)
    {
        ExploreTravelDestination* loc;

        WorldPosition point = WorldPosition(u.map, u.x, u.y, u.z, u.o);
        AreaTableEntry const* area = point.getArea();

        if (!area)
            continue;

        if (!area->exploreFlag)
            continue;

        if (u.type == 1) 
            continue;

        auto iloc = exploreLocs.find(area->ID);

        int32 guid = u.type == 0 ? u.guid : u.guid * -1;

        pointsMap.insert_or_assign(guid, point);

        if (iloc == exploreLocs.end())
        {
            loc = new ExploreTravelDestination(area->ID, sPlayerbotAIConfig.tooCloseDistance, sPlayerbotAIConfig.sightDistance);
            loc->setMaxVisitors(1000, 0);
            loc->setCooldownDelay(1000);
            loc->setExpireDelay(1000);
            exploreLocs.insert_or_assign(area->ID, loc);
        }
        else
        {
            loc = iloc->second;
        }

        loc->addPoint(&pointsMap.find(guid)->second);
    }
     




#ifdef IKE_PATHFINDER
    bool mmapAvoidMobMod = true;

    if (mmapAvoidMobMod)
    {
        sLog.outString("Loading mob avoidance maps");

        //Mob avoidance
        PathFinder path;
        WorldPosition emptyPoint;
        FactionTemplateEntry const* humanFaction = sFactionTemplateStore.LookupEntry(1);
        FactionTemplateEntry const* orcFaction = sFactionTemplateStore.LookupEntry(2);        

        for (auto& creaturePair : emptyPoint.getCreaturesNear())
        {
            CreatureData const cData = creaturePair->second;
            CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(cData.id);

            if (!cInfo)
                continue;

            WorldPosition point = WorldPosition(cData.mapid, cData.posX, cData.posY, cData.posZ, cData.orientation);

            if (cInfo->NpcFlags > 0)
                continue;

            FactionTemplateEntry const* factionEntry = sFactionTemplateStore.LookupEntry(cInfo->Faction);
            ReputationRank reactionHum = PlayerbotAI::GetFactionReaction(humanFaction, factionEntry);
            ReputationRank reactionOrc = PlayerbotAI::GetFactionReaction(orcFaction, factionEntry);

            if (reactionHum >= REP_NEUTRAL || reactionOrc >= REP_NEUTRAL)
                continue;

            if (!point.getTerrain())
                continue;

            point.loadMapAndVMap();

            path.setArea(point.getMapId(), point.getX(), point.getY(), point.getZ(), 11, 50.0f);
            path.setArea(point.getMapId(), point.getX(), point.getY(), point.getZ(), 12, 20.0f);
        }
    }
#endif

    bool fullNavPointReload = false;
    bool storeNavPointReload = false;

    if(!fullNavPointReload)
        TravelNodeStore::loadNodes();

    for (auto node : sTravelNodeMap.getNodes())
    {
        node->setLinked(true);
    }

    bool reloadNavigationPoints = false || fullNavPointReload || storeNavPointReload;

    if (reloadNavigationPoints)
    {
        sLog.outString("Loading navigation points");        

        //Npc nodes

        WorldPosition pos;

        for (auto& u : units)
        {
            if (u.type != 0)
                continue;

            CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(u.entry);

            if (!cInfo)
                continue;

            vector<uint32> allowedNpcFlags;

            allowedNpcFlags.push_back(UNIT_NPC_FLAG_INNKEEPER);
            allowedNpcFlags.push_back(UNIT_NPC_FLAG_FLIGHTMASTER);
            //allowedNpcFlags.push_back(UNIT_NPC_FLAG_QUESTGIVER);

            for (vector<uint32>::iterator i = allowedNpcFlags.begin(); i != allowedNpcFlags.end(); ++i)
            {
                if ((cInfo->NpcFlags & *i) != 0)
                {

                    pos = WorldPosition(u.map, u.x, u.y, u.z, u.o);

                    string nodeName = pos.getAreaName(false);
                    if ((cInfo->NpcFlags & UNIT_NPC_FLAG_INNKEEPER) != 0)
                        nodeName += " innkeeper";
                    else
                        nodeName += " flightMaster";

                    sTravelNodeMap.addNode(&pos, nodeName, true, true);

                    break;
                }
            }
        }

        //Unique bosses
        for (auto& u : units)
        {
            if (u.type != 0)
                continue;

            CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(u.entry);

            if (!cInfo)
                continue;

            pos = WorldPosition(u.map, u.x, u.y, u.z, u.o);

            if (cInfo->Rank == 3 || (cInfo->Rank == 1 && !pos.isOverworld() && u.c == 1))
            {
                string nodeName = cInfo->Name;

                sTravelNodeMap.addNode(&pos, nodeName, true, true);
            }
        }

        map<uint8, string> startNames;
        startNames[RACE_HUMAN] = "Human";
        startNames[RACE_ORC] = "Orc and Troll";
        startNames[RACE_DWARF] = "Dwarf and Gnome";
        startNames[RACE_NIGHTELF] = "Night Elf";
        startNames[RACE_UNDEAD] = "Undead";
        startNames[RACE_TAUREN] = "Tauren";
        startNames[RACE_GNOME] = "Dwarf and Gnome";
        startNames[RACE_TROLL] = "Orc and Troll";
        startNames[RACE_GOBLIN] = "Goblin";

        for (uint32 i = 0; i < MAX_RACES; i++)
        {
            for (uint32 j = 0; j < MAX_CLASSES; j++)
            {
                PlayerInfo const* info = sObjectMgr.GetPlayerInfo(i, j);

                if (!info)
                    continue;

                pos = WorldPosition(info->mapId, info->positionX, info->positionY, info->positionZ, info->orientation);

                string nodeName = startNames[i] + " start";

                sTravelNodeMap.addNode(&pos, nodeName, true, true);
            }
        }

        //Entrance nodes

        for (int i = 0; i < 6000; i++)
        {
            AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(i);
            if (!atEntry)
                continue;

            AreaTrigger const* at = sObjectMgr.GetAreaTrigger(i);
            if (!at)
                continue;

            WorldPosition inPos = WorldPosition(atEntry->mapid, atEntry->x, atEntry->y, atEntry->z - 4.0f, 0);

            WorldPosition outPos = WorldPosition(at->target_mapId, at->target_X, at->target_Y, at->target_Z, at->target_Orientation);

            string nodeName;

            if (!outPos.isOverworld())
                nodeName = outPos.getAreaName(false) + " entrance";
            else if (!inPos.isOverworld())
                nodeName = inPos.getAreaName(false) + " exit";
            else
                nodeName = inPos.getAreaName(false) + " portal";

            sTravelNodeMap.addNode(&inPos, nodeName, true, true);
        }

        //Exit nodes

        for (int i = 0; i < 6000; i++)
        {
            AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(i);
            if (!atEntry)
                continue;

            AreaTrigger const* at = sObjectMgr.GetAreaTrigger(i);
            if (!at)
                continue;

            WorldPosition inPos = WorldPosition(atEntry->mapid, atEntry->x, atEntry->y, atEntry->z - 4.0f, 0);

            WorldPosition outPos = WorldPosition(at->target_mapId, at->target_X, at->target_Y, at->target_Z, at->target_Orientation);

            string nodeName;

            if (!outPos.isOverworld())
                nodeName = outPos.getAreaName(false) + " entrance";
            else if (!inPos.isOverworld())
                nodeName = inPos.getAreaName(false) + " exit";
            else
                nodeName = inPos.getAreaName(false) + " portal";

            TravelNode* entryNode = sTravelNodeMap.getNode(&outPos, NULL, 20.0f); //Entry side, portal exit.

            TravelNode* outNode = sTravelNodeMap.addNode(&outPos, nodeName, true, true); //Exit size, portal exit.

            TravelNode* inNode = sTravelNodeMap.getNode(&inPos, NULL, 5.0f); //Entry side, portal center.

            //Portal link from area trigger to area trigger destination.
            if (outNode && inNode)
            {
                TravelNodePath travelPath({ *inNode->getPosition(), *outNode->getPosition() }, 3.0f, true, i, false);
                inNode->setPathTo(outNode, travelPath);
            }

            /*
            //Possible to walk from destination back into the portal.
            if (outNode && entryNode)
            {
                TravelNodePath travelPath(outNode->getDistance(entryNode));
                travelPath.setPath({ *outNode->getPosition(), *entryNode->getPosition() });

                outNode->setPathTo(entryNode, travelPath);
            }
            */
        }

        //Transports
        for (uint32 entry = 1; entry <= sGOStorage.GetMaxEntry(); ++entry)
        {
            auto data = sGOStorage.LookupEntry<GameObjectInfo>(entry);
            if (data && (data->type == GAMEOBJECT_TYPE_TRANSPORT || data->type == GAMEOBJECT_TYPE_MO_TRANSPORT))
            {
                TransportAnimation const* animation = sTransportMgr.GetTransportAnimInfo(entry);

                uint32 pathId = data->moTransport.taxiPathId;
                float moveSpeed = data->moTransport.moveSpeed;
                if (pathId >= sTaxiPathNodesByPath.size())
                    continue;

                TaxiPathNodeList const& path = sTaxiPathNodesByPath[pathId];

                vector<WorldPosition> ppath;
                TravelNode* prevNode = nullptr;

                //Elevators/Trams
                if (path.empty())
                {
                    if (animation)
                    {
                        TransportPathContainer aPath = animation->Path;
                        float timeStart;

                        for (auto& u : units)
                        {
                            if (u.type != 1)
                                continue;

                            if (u.entry != entry)
                                continue;

                            prevNode = nullptr;
                            WorldPosition lPos = WorldPosition(u.map, 0, 0, 0, 0);

                            for (auto& p : aPath)
                            {
#ifndef MANGOSBOT_TWO
                                float dx = cos(u.o) * p.second->X - sin(u.o) * p.second->Y;
                                float dy = sin(u.o) * p.second->X + cos(u.o) * p.second->Y;
#else
                                float dx = -1 * p.second->X;
                                float dy = -1 * p.second->Y;
#endif
                                WorldPosition pos = WorldPosition(u.map, u.x + dx, u.y + dy, u.z + p.second->Z, u.o);

                                if (prevNode)
                                {
                                    ppath.push_back(pos);
                                }

                                if (pos.distance(&lPos) == 0)
                                {
                                    TravelNode* node = sTravelNodeMap.addNode(&pos, data->name, true, true, true, entry);
                        
                                    if (!prevNode)
                                    {
                                        ppath.push_back(pos);
                                        timeStart = p.second->TimeSeg;
                                    }
                                    else
                                    {
                                        float totalTime = (p.second->TimeSeg - timeStart) / 1000.0f;
                                        TravelNodePath travelPath(ppath, totalTime,false, 0, true);
                                        node->setPathTo(prevNode, travelPath);
                                        ppath.clear();
                                        ppath.push_back(pos);
                                        timeStart = p.second->TimeSeg;
                                    }

                                    prevNode = node;
                                }

                                lPos = pos;
                            }

                            if (prevNode)
                            {
                                for (auto& p : aPath)
                                {
#ifndef MANGOSBOT_TWO
                                    float dx = cos(u.o) * p.second->X - sin(u.o) * p.second->Y;
                                    float dy = sin(u.o) * p.second->X + cos(u.o) * p.second->Y;
#else
                                    float dx = -1 * p.second->X;
                                    float dy = -1 * p.second->Y;
#endif
                                    WorldPosition pos = WorldPosition(u.map, u.x + dx, u.y + dy, u.z + p.second->Z, u.o);

                                    ppath.push_back(pos);

                                    if (pos.distance(&lPos) == 0)
                                    {
                                        TravelNode* node = sTravelNodeMap.addNode(&pos, data->name, true, true, true, entry);
                                        if (node != prevNode) {
                                            float totalTime = (p.second->TimeSeg - timeStart) / 1000.0f;
                                            TravelNodePath travelPath(ppath, totalTime, false, 0, true);
                                            node->setPathTo(prevNode, travelPath);
                                            ppath.clear();
                                            ppath.push_back(pos);
                                            timeStart = p.second->TimeSeg;
                                        }
                                    }

                                    lPos = pos;
                                }
                            }

                            ppath.clear();
                        }
                    }
                }
                else //Boats/Zepelins
                {
                    //Loop over the path and connect stop locations.
                    for (auto& p : path)
                    {
                        WorldPosition pos = WorldPosition(p->mapid, p->x, p->y, p->z, 0);

                        //if (data->displayId == 3015) 
                        //    pos.setZ(pos.getZ() + 6.0f);
                        //else if(data->displayId == 3031)
                       //     pos.setZ(pos.getZ() - 17.0f);

                        if (prevNode)
                        {
                            ppath.push_back(pos);
                        }
                        
                        if (p->delay > 0)
                        {
                            TravelNode* node = sTravelNodeMap.addNode(&pos, data->name, true, true, true, entry);

                            if (!prevNode)
                            {
                                ppath.push_back(pos);
                            }
                            else
                            {
                                TravelNodePath travelPath(ppath, 0.1f, false, 0, true, moveSpeed);
                                node->setPathTo(prevNode, travelPath);
                                ppath.clear();
                                ppath.push_back(pos);
                            }

                            prevNode = node;
                        }
                    }

                    if (prevNode)
                    {
                        //Continue from start until first stop and connect to end.
                        for (auto& p : path)
                        {
                            WorldPosition pos = WorldPosition(p->mapid, p->x, p->y, p->z, 0);

                            //if (data->displayId == 3015)
                            //    pos.setZ(pos.getZ() + 6.0f);
                            //else if (data->displayId == 3031)
                            //    pos.setZ(pos.getZ() - 17.0f);

                            ppath.push_back(pos);

                            if (p->delay > 0)
                            {
                                TravelNode* node = sTravelNodeMap.getNode(&pos, NULL, 5.0f);

                                if (node != prevNode) {
                                    TravelNodePath travelPath(ppath, 0.1f, false, 0, true, moveSpeed);
                                    node->setPathTo(prevNode, travelPath);
                                }
                            }
                        }
                    }

                    ppath.clear();
                }
            }
        }

        BarGoLink bar(exploreLocs.size());

        //Zone means   
        for (auto& loc : exploreLocs)
        {
            bar.step();
            vector<WorldPosition*> points;

            for (auto p : loc.second->getPoints(true))
                if (!p->isUnderWater())
                    points.push_back(p);

            if (points.empty())
                points = loc.second->getPoints(true);

            WorldPosition  pos = WorldPosition(points, WP_MEAN_CENTROID);

            TravelNode* node = sTravelNodeMap.addNode(&pos, pos.getAreaName(), true, true, false);
        }

        sLog.outString(">> Loaded " SIZEFMTD " navigation points.", sTravelNodeMap.getNodes().size());
    }

    TravelNodeStore::loadUserNodes();

    sTravelNodeMap.calcMapOffset();
    loadMapTransfers();

    //Clear these logs files
    sPlayerbotAIConfig.openLog("zones.csv", "w");
    sPlayerbotAIConfig.openLog("creatures.csv", "w");
    sPlayerbotAIConfig.openLog("gos.csv", "w");
    sPlayerbotAIConfig.openLog("bot_movement.csv", "w");
    sPlayerbotAIConfig.openLog("bot_pathfinding.csv", "w");    
    sPlayerbotAIConfig.openLog("pathfind_attempt.csv", "w");
    sPlayerbotAIConfig.openLog("pathfind_attempt_point.csv", "w");
    sPlayerbotAIConfig.openLog("pathfind_result.csv", "w");
    sPlayerbotAIConfig.openLog("load_map_grid.csv", "w");

    bool preloadNodePaths = false || fullNavPointReload || storeNavPointReload;             //Calculate paths using pathfinder.
    bool preloadReLinkFullyLinked = false || fullNavPointReload || storeNavPointReload;      //Retry nodes that are fully linked.
    bool preloadUnlinkedPaths = false || fullNavPointReload;        //Try to connect points currently unlinked.
    bool preloadWorldPaths = true;            //Try to load paths in overworld.
    bool preloadInstancePaths = true;         //Try to load paths in instances.
    bool preloadSubPrint = true;              //Print output every 2%.

    if (preloadNodePaths)
    {
        std::unordered_map<uint32, Map*> instances;       

        //Pathfinder
        BarGoLink bar(sTravelNodeMap.getNodes().size());
        vector<WorldPosition> ppath;

        uint32 cur = 0, max = sTravelNodeMap.getNodes().size();

        for (auto& startNode : sTravelNodeMap.getNodes())
        {
            if (!preloadReLinkFullyLinked && startNode->isLinked())
                continue;

            for (auto& endNode : sTravelNodeMap.getNodes())
            {
                if (startNode == endNode)
                    continue;

                if (startNode->getPosition()->isOverworld() && !preloadWorldPaths)
                    continue;

                if (!startNode->getPosition()->isOverworld() && !preloadInstancePaths)
                    continue;

                if (startNode->hasCompletePathTo(endNode))
                    continue;

                if (!preloadUnlinkedPaths && !startNode->hasLinkTo(endNode))
                    continue;

                if (startNode->getMapId() != endNode->getMapId())
                    continue;

                //if (preloadUnlinkedPaths && !startNode->hasLinkTo(endNode) && startNode->isUselessLink(endNode))
                //    continue;

                startNode->buildPath(endNode, NULL, false);

                if (startNode->hasLinkTo(endNode) && !startNode->getPathTo(endNode)->getComplete())
                    startNode->removeLinkTo(endNode);
            }

            startNode->setLinked(true);

            cur++;

            if (preloadSubPrint && (cur * 50) / max > ((cur - 1) * 50) / max)
            {
                sTravelNodeMap.printMap();
                sTravelNodeMap.printNodeStore();
            }

            bar.step();
        }

        if (!preloadSubPrint)
        {
            sTravelNodeMap.printNodeStore();
            sTravelNodeMap.printMap();
        }

        sLog.outString(">> Loaded paths for " SIZEFMTD " nodes.", sTravelNodeMap.getNodes().size());
    }

    bool removeLowLinkNodes = false || fullNavPointReload;

    if (removeLowLinkNodes)
    {
        BarGoLink bar(sTravelNodeMap.getNodes().size());
        
        vector<TravelNode*> goodNodes;
        vector<TravelNode*> remNodes;
        for (auto& node : sTravelNodeMap.getNodes())
        {
            bar.step();

            if (std::find(goodNodes.begin(), goodNodes.end(), node) != goodNodes.end())
                continue;

            if (std::find(remNodes.begin(), remNodes.end(), node) != remNodes.end())
                continue;

            vector<TravelNode*> nodes = node->getNodeMap(true);

            if (nodes.size() < 5)
                remNodes.insert(remNodes.end(), nodes.begin(), nodes.end());
            else
                goodNodes.insert(goodNodes.end(), nodes.begin(), nodes.end());
        }

        for (auto& node : remNodes)
            sTravelNodeMap.removeNode(node);

        sLog.outString(">> Checked " SIZEFMTD " nodes.", sTravelNodeMap.getNodes().size());
    }
  
    bool cleanUpNodeLinks = false || fullNavPointReload || storeNavPointReload;
    bool cleanUpSubPrint = true;              //Print output every 2%.

    if (cleanUpNodeLinks)
    {
        //Routes
        BarGoLink bar(sTravelNodeMap.getNodes().size());

        uint32 cur = 0, max = sTravelNodeMap.getNodes().size();
        
        //Clean up node links
        for (auto& startNode : sTravelNodeMap.getNodes())
        {
             startNode->cropUselessLinks();            

             cur++;
             if (cleanUpSubPrint && (cur * 10) / max > ((cur - 1) * 10) / max)
             {
                 sTravelNodeMap.printMap();
                 sTravelNodeMap.printNodeStore();
             }

             bar.step();
        }

        sLog.outString(">> Cleaned paths for " SIZEFMTD " nodes.", sTravelNodeMap.getNodes().size());
    }

    bool reCalculateCost = false || fullNavPointReload || storeNavPointReload;
    bool forceReCalculate = false;

    if (reCalculateCost)
    {
        BarGoLink bar(sTravelNodeMap.getNodes().size());

        for (auto& startNode : sTravelNodeMap.getNodes())
        {
            for (auto& path : *startNode->getLinks())
            {
                TravelNodePath* nodePath = path.second;

                if (nodePath->getPortal() || nodePath->getTransport())
                    continue;

                if (nodePath->getCalculated() && !forceReCalculate)
                    continue;

                nodePath->calculateCost();
            }

            bar.step();
        }

        sLog.outString(">> Calculated pathcost for " SIZEFMTD " nodes.", sTravelNodeMap.getNodes().size());
    }

    sTravelNodeMap.printMap();
    sTravelNodeMap.printNodeStore();

    //Creature/gos/zone export.
    if (sPlayerbotAIConfig.hasLog("creatures.csv"))
    {
        for (auto& creaturePair : WorldPosition().getCreaturesNear())
        {
            CreatureData const cData = creaturePair->second;
            CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(cData.id);

            if (!cInfo)
                continue;

            WorldPosition point = WorldPosition(cData.mapid, cData.posX, cData.posY, cData.posZ, cData.orientation);

            string name = cInfo->Name;
            name.erase(remove(name.begin(), name.end(), ','), name.end());
            name.erase(remove(name.begin(), name.end(), '\"'), name.end());

            ostringstream out;
            out << name << ",";
            point.printWKT(out);
            out << cInfo->MaxLevel << ",";
            out << cInfo->Rank << ",";
            out << cInfo->Faction << ",";
            out << cInfo->NpcFlags << ",";
            out << point.getAreaName() << ",";
            out << std::fixed;

            sPlayerbotAIConfig.log("creatures.csv", out.str().c_str());
        }
    }

    if (sPlayerbotAIConfig.hasLog("gos.csv"))
    {
        for (auto& gameObjectPair : WorldPosition().getGameObjectsNear())
        {
            GameObjectData const gData = gameObjectPair->second;
            auto data = sGOStorage.LookupEntry<GameObjectInfo>(gData.id);

            if (!data)
                continue;

            WorldPosition point = WorldPosition(gData.mapid, gData.posX, gData.posY, gData.posZ, gData.orientation);

            string name = data->name;
            name.erase(remove(name.begin(), name.end(), ','), name.end());
            name.erase(remove(name.begin(), name.end(), '\"'), name.end());

            ostringstream out;
            out << name << ",";
            point.printWKT(out);
            out << data->type << ",";
            out << point.getAreaName() << ",";
            out << std::fixed;

            sPlayerbotAIConfig.log("gos.csv", out.str().c_str());
        }
    }

    if (sPlayerbotAIConfig.hasLog("zones.csv"))
    {
        std::unordered_map<string, vector<WorldPosition>> zoneLocs;

        vector<WorldPosition> Locs = {};
        
        for (auto& u : units)
        {
            WorldPosition point = WorldPosition(u.map, u.x, u.y, u.z, u.o);
            string name = to_string(u.map) + point.getAreaName();

            if (zoneLocs.find(name) == zoneLocs.end())
                zoneLocs.insert_or_assign(name, Locs);

            zoneLocs.find(name)->second.push_back(point);            
        }        

        for (auto& loc : zoneLocs)
        {
            if (loc.second.empty())
                continue;

            vector<WorldPosition> points = loc.second;;
           
            ostringstream out; 

            WorldPosition pos = WorldPosition(points, WP_MEAN_CENTROID);

            out << "\"center\"" << ",";
            out << points.begin()->getMapId() << ",";
            out << points.begin()->getAreaName() << ",";
            out << points.begin()->getAreaName(true, true) << ",";

            pos.printWKT(out);

            out << "\n";
            
            out << "\"area\"" << ",";
            out << points.begin()->getMapId() << ",";
            out << points.begin()->getAreaName() << ",";
            out << points.begin()->getAreaName(true, true) << ",";

            point.printWKT(points, out, 0);

            sPlayerbotAIConfig.log("zones.csv", out.str().c_str());
        }
    }
    /*

    sPlayerbotAIConfig.openLog(7, "w");

    //Zone area map REMOVE!
    uint32 k = 0;
    for (auto& node : sTravelNodeMap.getNodes())
    {
        WorldPosition* pos = node->getPosition();
        //map area
        for (uint32 x = 0; x < 2000; x++)
        {
            for (uint32 y = 0; y < 2000; y++)
            {
                if (!pos->getMap())
                    continue;

                float nx = pos->getX() + (x*5)-5000.0f;
                float ny = pos->getY() + (y*5)-5000.0f;
                float nz = pos->getZ() + 100.0f;

                //pos->getMap()->GetHitPosition(nx, ny, nz + 200.0f, nx, ny, nz, -0.5f);

                if (!pos->getMap()->GetHeightInRange(nx, ny, nz, 5000.0f)) // GetHeight can fail
                    continue;

                WorldPosition  npos = WorldPosition(pos->getMapId(), nx, ny, nz, 0.0);
                uint32 area = path.getArea(npos.getMapId(), npos.getX(), npos.getY(), npos.getZ());

                ostringstream out;
                out << std::fixed << area << "," << npos.getDisplayX() << "," << npos.getDisplayY();
                sPlayerbotAIConfig.log(7, out.str().c_str());
            }
        }
        k++;

        if (k > 0)
            break;
    }

    //Explore map output (REMOVE!)

    sPlayerbotAIConfig.openLog(5, "w");
    for (auto i : exploreLocs)
    {
        for (auto j : i.second->getPoints())
        {
            ostringstream out;
            string name = i.second->getTitle();
            name.erase(remove(name.begin(), name.end(), '\"'), name.end());
            out << std::fixed << std::setprecision(2) << name.c_str() << "," << i.first << "," << j->getDisplayX() << "," << j->getDisplayY() << "," << j->getX() << "," << j->getY() << "," << j->getZ();
            sPlayerbotAIConfig.log(5, out.str().c_str());
        }
    }

    */    
}

uint32 TravelMgr::getDialogStatus(Player* pPlayer, int32 questgiver, Quest const* pQuest)
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

//Selects a random WorldPosition from a list. Use a distance weighted distribution.
vector<WorldPosition*> TravelMgr::getNextPoint(WorldPosition* center, vector<WorldPosition*> points) {
    vector<WorldPosition*> retVec;

    if (points.size() == 1)
    {
        retVec.push_back(points[0]);
        return retVec;
    }

    //List of weights based on distance (Gausian curve that starts at 100 and lower to 1 at 1000 distance)
    vector<uint32> weights;

    std::transform(points.begin(), points.end(), std::back_inserter(weights), [center](WorldPosition* point) { return 1 + 1000 * exp(-1 * pow(point->distance(center) / 400.0, 2)); });

    //Total sum of all those weights.
    uint32 sum = std::accumulate(weights.begin(), weights.end(), 0);

    //Pick a random number in that range.
    uint32 rnd = urand(0, sum);

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

bool TravelMgr::getObjectiveStatus(Player* bot, Quest const* pQuest, uint32 objective)
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

vector<TravelDestination*> TravelMgr::getQuestTravelDestinations(Player* bot, uint32 questId, bool ignoreFull, bool ignoreInactive, float maxDistance, bool ignoreObjectives)
{
    WorldPosition botLocation(bot);

    vector<TravelDestination*> retTravelLocations;

    if (questId == -1)
    {
        for (auto& dest : questGivers)
        {
            if (!ignoreInactive && !dest->isActive(bot))
                continue;

            if (dest->isFull(ignoreFull))
                continue;

            if (maxDistance > 0 && dest->distanceTo(&botLocation) > maxDistance)
                continue;

            retTravelLocations.push_back(dest);
        }
    }
    else
    {
        auto i = quests.find(questId);

        if (i != quests.end())
        {
            for (auto& dest : i->second->questTakers)
            {
                if (!ignoreInactive && !dest->isActive(bot))
                    continue;

                if (dest->isFull(ignoreFull))
                    continue;

                if (maxDistance > 0 && dest->distanceTo(&botLocation) > maxDistance)
                    continue;

                retTravelLocations.push_back(dest);
            }

            if (!ignoreObjectives)
                for (auto& dest : i->second->questObjectives)
                {
                    if (!ignoreInactive && !dest->isActive(bot))
                        continue;

                    if (dest->isFull(ignoreFull))
                        continue;

                    if (maxDistance > 0 && dest->distanceTo(&botLocation) > maxDistance)
                        continue;

                    retTravelLocations.push_back(dest);
                }
        }
    }

    return retTravelLocations;
}

vector<TravelDestination*> TravelMgr::getRpgTravelDestinations(Player* bot, bool ignoreFull, bool ignoreInactive, float maxDistance)
{
    WorldPosition botLocation(bot);

    vector<TravelDestination*> retTravelLocations;

    for (auto& dest : rpgNpcs)
    {
        if (!ignoreInactive && !dest->isActive(bot))
            continue;

        if (dest->isFull(ignoreFull))
            continue;

        if (maxDistance > 0 && dest->distanceTo(&botLocation) > maxDistance)
            continue;

        retTravelLocations.push_back(dest);
    }

    return retTravelLocations;
}

vector<TravelDestination*> TravelMgr::getExploreTravelDestinations(Player* bot, bool ignoreFull, bool ignoreInactive)
{
    WorldPosition botLocation(bot);

    vector<TravelDestination*> retTravelLocations;

    for (auto& dest : exploreLocs)
    {
        if (!ignoreInactive && !dest.second->isActive(bot))
            continue;

        if (dest.second->isFull(ignoreFull))
            continue;

        retTravelLocations.push_back(dest.second);
    }

    return retTravelLocations;
}


void TravelMgr::setNullTravelTarget(Player* player)
{
    if (!player)
        return;

    if (!player->GetPlayerbotAI())
        return;

    TravelTarget* target = player->GetPlayerbotAI()->GetAiObjectContext()->GetValue<TravelTarget*>("travel target")->Get();

    if (target)
        target->setTarget(sTravelMgr.nullTravelDestination, sTravelMgr.nullWorldPosition, true);
}

void TravelMgr::addMapTransfer(WorldPosition start, WorldPosition end, float portalDistance)
{
    if (start.getMapId() == end.getMapId())
        return;

    //Calculate shortcuts.
    for (auto& mapTrans : mapTransfers)
    {
        if (mapTrans.isTo(start) && !mapTrans.isFrom(end)) // [S1 >MT> E1 -> S2] >THIS> E2
        {
            float newDistToEnd = mapTransDistance(*mapTrans.getPointFrom(), start) + portalDistance;
            if (mapTransDistance(*mapTrans.getPointFrom(), end) > newDistToEnd)
                mapTransfers.push_back(mapTransfer(*mapTrans.getPointFrom(), end, newDistToEnd));
        }

        if (mapTrans.isFrom(end) && !mapTrans.isTo(start)) // S1 >THIS> [E1 -> S2 >MT> E2]
        {
            float newDistToEnd = portalDistance + mapTransDistance(end, *mapTrans.getPointTo());
            if (mapTransDistance(start, *mapTrans.getPointTo()) > newDistToEnd)
                mapTransfers.push_back(mapTransfer(start, *mapTrans.getPointTo(), newDistToEnd));
        }
    }

    //Add actual transfer.
    mapTransfers.push_back(mapTransfer(start, end, portalDistance));
};

void TravelMgr::loadMapTransfers()
{
    for (auto& node : sTravelNodeMap.getNodes())
    {
        for (auto& link : *node->getLinks())
        {
            addMapTransfer(*node->getPosition(), *link.first->getPosition(), link.second->getDistance());
        }
    }
}

float TravelMgr::mapTransDistance(WorldPosition start, WorldPosition end)
{
    if (start.getMapId() == end.getMapId())
        return start.distance(end);

    float minDist = 200000;

    for (auto & mapTrans : mapTransfers)
    {
        if (!mapTrans.isUsefull(start, end))
            continue;

        float dist = mapTrans.distance(start, end);

        if (dist < minDist)
            minDist = dist;
    }

    return minDist;
}