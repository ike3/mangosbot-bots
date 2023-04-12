#include "WorldPosition.h"
#include "GuidPosition.h"
#include "TravelMgr.h"
#include "TravelNode.h"

#include "Map.h"

#ifdef MANGOSBOT_TWO
    #include "Vmap/VMapFactory.h"
#else
    #include "vmap/VMapFactory.h"
#endif


#include "MoveMap.h"
#include "World.h"
#include "PathFinder.h"
#include "Grids/CellImpl.h"
#include "ObjectAccessor.h"
#include "Entities/Transports.h"

#include <numeric>
#include <iomanip>

using namespace ai;
using namespace MaNGOS;

WorldPosition::WorldPosition(const uint32 mapId, const GuidPosition& guidP)
{
    if (guidP.mapid !=0 || guidP.coord_x != 0 || guidP.coord_y != 0 || guidP.coord_z !=0) {
        set(WorldPosition(guidP.mapid, guidP.coord_x, guidP.coord_y, guidP.coord_z, guidP.orientation));
        return;
    }

    set(ObjectGuid(guidP), guidP.mapid);
 }

void WorldPosition::set(const ObjectGuid& guid, const uint32 mapId)
{
    switch (guid.GetHigh())
    {
    case HIGHGUID_PLAYER:
    {
        Player* player = sObjectAccessor.FindPlayer(guid);
        if (player)
            set(player);
        break;
    }
    case HIGHGUID_GAMEOBJECT:
    {        
        GameObjectDataPair const* gpair = sObjectMgr.GetGODataPair(guid.GetCounter());
        if (gpair)
            set(gpair);

        break;
    }
    case HIGHGUID_UNIT:
    {
        setMapId(mapId);
        Creature* creature = getMap()->GetAnyTypeCreature(guid);
        if (creature)
        {
            set(creature);
            return;
        }

        CreatureDataPair const* cpair = sObjectMgr.GetCreatureDataPair(guid.GetCounter());
        if (cpair)
            set(cpair);
        break;
    }
    case HIGHGUID_TRANSPORT:
    case HIGHGUID_MO_TRANSPORT:
    case HIGHGUID_ITEM:
    case HIGHGUID_PET:
    case HIGHGUID_DYNAMICOBJECT:
    case HIGHGUID_CORPSE:
        return;
    }
}

WorldPosition::WorldPosition(const vector<WorldPosition*>& list, const WorldPositionConst conType)
{
    uint32 size = list.size();
    if (size == 0)
        return;
    else if (size == 1)
        set(*list.front());
    else if (conType == WP_RANDOM)
        set(*list[urand(0, size - 1)]);
    else if (conType == WP_CENTROID)
        set(std::accumulate(list.begin(), list.end(), WorldLocation(list[0]->getMapId(), 0, 0, 0, 0), [size](WorldLocation i, WorldPosition* j) {i.coord_x += j->getX() / size; i.coord_y += j->getY() / size; i.coord_z += j->getZ() / size; i.orientation += j->getO() / size; return i; }));
    else if (conType == WP_MEAN_CENTROID)
    {
        WorldPosition pos = WorldPosition(list, WP_CENTROID);
        set(*pos.closestSq(list));
    }
}

WorldPosition::WorldPosition(const vector<WorldPosition>& list, const WorldPositionConst conType)
{    
    uint32 size = list.size();
    if (size == 0)
        return;
    else if (size == 1)
        set(list.front());
    else if (conType == WP_RANDOM)
        set(list[urand(0, size - 1)]);
    else if (conType == WP_CENTROID)
        set(std::accumulate(list.begin(), list.end(), WorldLocation(list[0].getMapId(), 0, 0, 0, 0), [size](WorldLocation i, WorldPosition j) {i.coord_x += j.getX() / size; i.coord_y += j.getY() / size; i.coord_z += j.getZ() / size; i.orientation += j.getO() / size; return i; }));
    else if (conType == WP_MEAN_CENTROID)
    {
        WorldPosition pos = WorldPosition(list, WP_CENTROID);
        set(pos.closestSq(list));
    }    
}

float WorldPosition::distance(const WorldPosition& to) const
{
    if(mapid == to.getMapId())
        return relPoint(to).size();

    //this -> mapTransfer | mapTransfer -> center
    return sTravelMgr.mapTransDistance(*this, to);
};

float WorldPosition::fDist(const WorldPosition& to) const
{
    if (mapid == to.getMapId())
        return sqrt(sqDistance2d(to));

    //this -> mapTransfer | mapTransfer -> center
    return sTravelMgr.fastMapTransDistance(*this, to);
};

//When moving from this along list return last point that falls within range.
//Distance is move distance along path.
WorldPosition WorldPosition::lastInRange(const vector<WorldPosition>& list, const float minDist, const float maxDist) const
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
            totalDist += p.distance(*std::prev(&p, 1));

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
WorldPosition WorldPosition::firstOutRange(const vector<WorldPosition>& list, const float minDist, const float maxDist) const
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
bool WorldPosition::isInside(const WorldPosition* p1, const WorldPosition* p2, const WorldPosition* p3) const
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

void WorldPosition::distancePartition(const vector<float>& distanceLimits, WorldPosition* to, vector<vector<WorldPosition*>>& partitions) const
{
    float dist = distance(*to);

    for (uint8 l = 0; l < distanceLimits.size(); l++)
        if (dist <= distanceLimits[l])
            partitions[l].push_back(to);
}

vector<vector<WorldPosition*>> WorldPosition::distancePartition(const vector<float>& distanceLimits, vector<WorldPosition*> points) const
{
    vector<vector<WorldPosition*>> partitions;

    for (auto lim : distanceLimits)
        partitions.push_back({});

    for (auto& point : points)
    {
        distancePartition(distanceLimits, point, partitions);
    }

    return partitions;
}

bool WorldPosition::canFly() const
{
#ifdef MANGOSBOT_ZERO
    return false;
#endif
    if (!getTerrain())
        return false;

    uint32 zoneid, areaid;
    getTerrain()->GetZoneAndAreaId(zoneid, areaid, getX(), getY(), getZ());

#ifdef MANGOSBOT_ONE  
    uint32 v_map = GetVirtualMapForMapAndZone(getMapId(), zoneid);
    MapEntry const* mapEntry = sMapStore.LookupEntry(v_map);
    if (!mapEntry || mapEntry->addon < 1 || !mapEntry->IsContinent())
        return false;
#endif
#ifdef MANGOSBOT_TWO
    // Disallow mounting in wintergrasp when battle is in progress
    if (OutdoorPvP* outdoorPvP = sOutdoorPvPMgr.GetScript(zoneid))
    {
        if (outdoorPvP->IsBattlefield())
            return ((Battlefield*)outdoorPvP)->GetBattlefieldStatus() != BF_STATUS_IN_PROGRESS;
    }

    // don't allow flying in Dalaran restricted areas
    // (no other zones currently has areas with AREA_FLAG_CANNOT_FLY)
    if (AreaTableEntry const* atEntry = GetAreaEntryByAreaID(areaid))
        return (!(atEntry->flags & AREA_FLAG_CANNOT_FLY));
#endif

    return true;
}

G3D::Vector3 WorldPosition::getVector3() const
{
    return G3D::Vector3(coord_x, coord_y, coord_z); 
}

string WorldPosition::print() const
{
    ostringstream out;
    out << mapid << std::fixed << std::setprecision(2);
    out << ';'<< coord_x;
    out << ';' << coord_y;
    out << ';' << coord_z;
    out << ';' << orientation;

    return out.str();
}

void WorldPosition::printWKT(const vector<WorldPosition>& points, ostringstream& out, const uint32 dim, const bool loop) const
{
    switch (dim) {
    case 0:
        if(points.size() == 1)
            out << "\"POINT(";
        else
            out << "\"MULTIPOINT(";
        break;
    case 1:
        out << "\"LINESTRING(";
        break;
    case 2:
        out << "\"POLYGON((";
    }

    for (auto& p : points)
        out << p.getDisplayX() << " " << p.getDisplayY() << (!loop && &p == &points.back() ? "" : ",");

    if (loop)
        out << points.front().getDisplayX() << " " << points.front().getDisplayY();

    out << (dim == 2 ? "))\"," : ")\",");
}

WorldPosition WorldPosition::getDisplayLocation() const
{ 
    WorldPosition mapOffset = sTravelNodeMap.getMapOffset(getMapId());
    return offset(mapOffset);
};

AreaTableEntry const* WorldPosition::getArea() const
{
    uint16 areaFlag = getAreaFlag();

    //if(!areaFlag)
    //    return NULL;

    return GetAreaEntryByAreaFlagAndMap(areaFlag, getMapId());
}

string WorldPosition::getAreaName(const bool fullName, const bool zoneName) const
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

int32 WorldPosition::getAreaLevel() const
{
    if(getArea())
        return sTravelMgr.getAreaLevel(getArea()->ID);

    return 0;
}

std::set<GenericTransport*> WorldPosition::getTransports(uint32 entry)
{
    std::set<GenericTransport*> transports;
    for (auto transport : getMap()->GetTransports()) //Boats&Zeppelins.
        if (!entry || transport->GetEntry() == entry)
            transports.insert(transport);

    if (transports.empty() || !entry) //Elevators&rams
    {
        for (auto gopair : getGameObjectsNear(0.0f, entry))
            if (GameObject* go = getMap()->GetGameObject(gopair->first))
                if (GenericTransport* transport = dynamic_cast<GenericTransport*>(go))
                    transports.insert(transport);
    }

    return transports;
}

void WorldPosition::CalculatePassengerPosition(GenericTransport* transport)
{
    transport->CalculatePassengerPosition(coord_x, coord_y, coord_z, &orientation);
}

void WorldPosition::CalculatePassengerOffset(GenericTransport* transport)
{
    transport->CalculatePassengerOffset(coord_x, coord_y, coord_z, &orientation);
}

bool WorldPosition::isOnTransport(GenericTransport* transport)
{
    if (!transport)
        return false;

    WorldPosition trans(transport);

    if (distance(trans) > 40.0f)
        return false;

    WorldPosition below(*this);

    below.setZ(below.getZ() - 5.0f);

    bool result0 = VMAP::VMapFactory::createOrGetVMapManager()->getObjectHitPos(mapid, coord_x, coord_y, coord_z + 0.5f, below.getX(), below.getY(), below.getZ(), below.coord_x, below.coord_y, below.coord_z, 0.0f);

    if (result0)
        return false;

    return GetHitPosition(below);
}

std::vector<GridPair> WorldPosition::getGridPairs(const WorldPosition& secondPos) const
{
    std::vector<GridPair> retVec;

    int lx = std::min(getGridPair().x_coord, secondPos.getGridPair().x_coord);
    int ly = std::min(getGridPair().y_coord, secondPos.getGridPair().y_coord);
    int ux = std::max(getGridPair().x_coord, secondPos.getGridPair().x_coord);
    int uy = std::max(getGridPair().y_coord, secondPos.getGridPair().y_coord);
    int border = 1;

    lx = std::min(std::max(border, lx), MAX_NUMBER_OF_GRIDS - border);
    ly = std::min(std::max(border, ly), MAX_NUMBER_OF_GRIDS - border);
    ux = std::min(std::max(border, ux), MAX_NUMBER_OF_GRIDS - border);
    uy = std::min(std::max(border, uy), MAX_NUMBER_OF_GRIDS - border);

    for (int x = lx - border; x <= ux + border; x++)
    {
        for (int y = ly - border; y <= uy + border; y++)
        {
            retVec.push_back(GridPair(x, y));
        }
    }

    return retVec;
}

vector<WorldPosition> WorldPosition::fromGridPair(const GridPair& gridPair) const
{
    vector<WorldPosition> retVec;
    GridPair g;

    for (uint32 d = 0; d < 4; d++)
    {
        g = gridPair;

        if (d == 1 || d == 2)
            g >> 1;
        if (d == 2 || d == 3)
            g += 1;

        retVec.push_back(WorldPosition(getMapId(), g));
    }
   
    return retVec;
}

vector<WorldPosition> WorldPosition::fromCellPair(const CellPair& cellPair) const
{
    vector<WorldPosition> retVec;
    CellPair p;

    for (uint32 d = 0; d < 4; d++)
    {
        p = cellPair;

        if (d == 1 || d == 2)
            p >> 1;
        if (d == 2 || d == 3)
            p += 1;

        retVec.push_back(WorldPosition(getMapId(), p));
    }
    return retVec;
}

vector<WorldPosition> WorldPosition::gridFromCellPair(const CellPair& cellPair) const
{    
    Cell c(cellPair);

    return fromGridPair(GridPair(c.GridX(), c.GridY()));
}

vector<pair<int,int>> WorldPosition::getmGridPairs(const WorldPosition& secondPos) const
{
    std::vector<mGridPair> retVec;

    int lx = std::min(getmGridPair().first, secondPos.getmGridPair().first);
    int ly = std::min(getmGridPair().second, secondPos.getmGridPair().second);
    int ux = std::max(getmGridPair().first, secondPos.getmGridPair().first);
    int uy = std::max(getmGridPair().second, secondPos.getmGridPair().second);
    int border = 1;

    //lx = std::min(std::max(border, lx), MAX_NUMBER_OF_GRIDS - border);
    //ly = std::min(std::max(border, ly), MAX_NUMBER_OF_GRIDS - border);
    //ux = std::min(std::max(border, ux), MAX_NUMBER_OF_GRIDS - border);
    //uy = std::min(std::max(border, uy), MAX_NUMBER_OF_GRIDS - border);

    for (int x = lx - border; x <= ux + border; x++)
    {
        for (int y = ly - border; y <= uy + border; y++)
        {
            retVec.push_back(make_pair(x, y));
        }
    }

    return retVec;
}

vector<WorldPosition> WorldPosition::frommGridPair(const mGridPair& gridPair) const
{
    vector<WorldPosition> retVec;
    mGridPair g;

    for (uint32 d = 0; d < 4; d++)
    {
        g = gridPair;

        if (d == 1 || d == 2)
            g.second++;
        if (d == 2 || d == 3)
            g.first++;

        retVec.push_back(WorldPosition(getMapId(), g));
    }

    return retVec;
}

void WorldPosition::loadMapAndVMap(uint32 mapId, uint32 instanceId, int x, int y) const
{
    string fileName = "load_map_grid.csv";

    if (isOverworld() && false || false)
    {
#ifdef MANGOSBOT_TWO
        if (!MMAP::MMapFactory::createOrGetMMapManager()->IsMMapTileLoaded(mapId, instanceId, x, y))
#else
        if (!MMAP::MMapFactory::createOrGetMMapManager()->IsMMapIsLoaded(mapId, x, y))
#endif
            if (sPlayerbotAIConfig.hasLog(fileName))
            {
                ostringstream out;
                out << sPlayerbotAIConfig.GetTimestampStr();
                out << "+00,\"mmap\", " << x << "," << y << "," << (sTravelMgr.isBadMmap(mapId, x, y) ? "0" : "1") << ",";
                printWKT(fromGridPair(GridPair(x, y)), out, 1, true);
                sPlayerbotAIConfig.log(fileName, out.str().c_str());
            }

        int px = (float)(32 - x) * SIZE_OF_GRIDS;
        int py = (float)(32 - y) * SIZE_OF_GRIDS;

#ifdef MANGOSBOT_TWO
        if (!MMAP::MMapFactory::createOrGetMMapManager()->IsMMapTileLoaded(mapId, instanceId, x, y))
#else
        if (!MMAP::MMapFactory::createOrGetMMapManager()->IsMMapIsLoaded(mapId, x, y))
#endif
            if(getTerrain())
                getTerrain()->GetTerrainType(px, py);

    }
    else
    {

        //This needs to be disabled or maps will not load.
        //Needs more testing to check for impact on movement.
        if (false)
            if (!VMAP::VMapFactory::createOrGetVMapManager()->IsTileLoaded(mapId, x, y) && !sTravelMgr.isBadVmap(mapId, x, y))
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
                    out << "+00,\"vmap\", " << x << "," << y << ", " << (sTravelMgr.isBadVmap(mapId, x, y) ? "0" : "1") << ",";
                    printWKT(fromGridPair(GridPair(x, y)), out, 1, true);
                    sPlayerbotAIConfig.log(fileName, out.str().c_str());
                }
            }

#ifdef MANGOSBOT_TWO
        if (!MMAP::MMapFactory::createOrGetMMapManager()->IsMMapTileLoaded(mapId, instanceId, x, y) && !sTravelMgr.isBadMmap(mapId, x, y))
#else
        if (!MMAP::MMapFactory::createOrGetMMapManager()->IsMMapIsLoaded(mapId, x, y) && !sTravelMgr.isBadMmap(mapId, x, y))
#endif
        {
            // load navmesh
#ifdef MANGOSBOT_TWO
            if (!MMAP::MMapFactory::createOrGetMMapManager()->loadMap(mapId, instanceId, x, y, 0))
#else
            if (!MMAP::MMapFactory::createOrGetMMapManager()->loadMap(mapId, x, y))
#endif
                sTravelMgr.addBadMmap(mapId, x, y);

            if (sPlayerbotAIConfig.hasLog(fileName))
            {
                ostringstream out;
                out << sPlayerbotAIConfig.GetTimestampStr();
                out << "+00,\"mmap\", " << x << "," << y << "," << (sTravelMgr.isBadMmap(mapId, x, y) ? "0" : "1") << ",";
                printWKT(frommGridPair(mGridPair(x, y)), out, 1, true);
                sPlayerbotAIConfig.log(fileName, out.str().c_str());
            }
        }
    }
}

void WorldPosition::loadMapAndVMaps(const WorldPosition& secondPos, uint32 instanceId) const
{
    for (auto& grid : getmGridPairs(secondPos))
    {
        loadMapAndVMap(getMapId(), instanceId, grid.first, grid.second);
    }
}

vector<WorldPosition> WorldPosition::fromPointsArray(const std::vector<G3D::Vector3>& path) const
{
    vector<WorldPosition> retVec;
    for (auto p : path)
        retVec.push_back(WorldPosition(getMapId(), p.x, p.y, p.z, getO()));

    return retVec;
}

//A single pathfinding attempt from one position to another. Returns pathfinding status and path.
vector<WorldPosition> WorldPosition::getPathStepFrom(const WorldPosition& startPos, const Unit* bot) const
{
    std::hash<std::thread::id> hasher;
    uint32 instanceId;
    if (sTravelNodeMap.gethasToGen())
        instanceId = 0;
    else if (!bot || sPlayerbotAIConfig.tweakValue || bot->GetMapId() != startPos.getMapId())
        instanceId = hasher(std::this_thread::get_id());
    else
        instanceId = bot->GetInstanceId();
    //Load mmaps and vmaps between the two points.

    loadMapAndVMaps(startPos, instanceId);

    PointsArray points;
    PathType type;

    if (bot && instanceId == bot->GetInstanceId())
    {
        PathFinder path(bot);

        path.setAreaCost(NAV_AREA_WATER, 10.0f);  //Water
        path.setAreaCost(12, 5.0f);  //Mob proximity
        path.setAreaCost(13, 20.0f); //Mob agro

        WorldPosition start = startPos, end = *this;

        if (bot->GetTransport())
        {
            start.CalculatePassengerOffset(bot->GetTransport());
            end.CalculatePassengerOffset(bot->GetTransport());
        }

        path.calculate(start.getVector3(), end.getVector3(), false);

        points = path.getPath();

        if (bot->GetTransport())
        {
            for (auto& p : points)
                bot->GetTransport()->CalculatePassengerPosition(p.x, p.y, p.z);
        }

        type = path.getPathType();

    }
    else
    {
        PathFinder path(getMapId(), instanceId);

        path.setAreaCost(NAV_AREA_WATER, 10.0f);
        path.setAreaCost(12, 5.0f);
        path.setAreaCost(13, 20.0f);
        path.calculate(startPos.getVector3(), getVector3(), false);

        points = path.getPath();
        type = path.getPathType();
    }

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

bool WorldPosition::cropPathTo(vector<WorldPosition>& path, const float maxDistance) const
{
    if (path.empty())
        return false;

   auto bestPos = std::min_element(path.begin(), path.end(), [this](WorldPosition i, WorldPosition j) {return this->sqDistance(i) < this->sqDistance(j); });

   bool insRange = this->sqDistance(*bestPos) <= maxDistance * maxDistance;

   if (bestPos == path.end())
       return insRange;

   path.erase(std::next(bestPos), path.end());

   return insRange;
}

//A sequential series of pathfinding attempts. Returns the complete path and if the patfinder eventually found a way to the destination.
vector<WorldPosition> WorldPosition::getPathFromPath(const vector<WorldPosition>& startPath, const Unit* bot, uint8 maxAttempt) const
{
    //We start at the end of the last path.
    WorldPosition currentPos = startPath.back();

    //No pathfinding across maps.
    if (getMapId() != currentPos.getMapId())
        return { };

    vector<WorldPosition> subPath, fullPath = startPath;

    //Limit the pathfinding attempts
    for (uint32 i = 0; i < maxAttempt; i++)
    {
        //Try to pathfind to this position.
        subPath = getPathStepFrom(currentPos, bot);

        //If we could not find a path return what we have now.
        if (subPath.empty() || currentPos.distance(subPath.back()) < sPlayerbotAIConfig.targetPosRecalcDistance)
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

bool WorldPosition::GetReachableRandomPointOnGround(const Player* bot, const float radius, const bool randomRange) 
{
#ifndef MANGOSBOT_TWO         
    return getMap()->GetReachableRandomPointOnGround(coord_x, coord_y, coord_z, radius, randomRange);
#else
    return getMap()->GetReachableRandomPointOnGround(bot->GetPhaseMask(), coord_x, coord_y, coord_z, radius, randomRange);
#endif
}

vector<WorldPosition> WorldPosition::ComputePathToRandomPoint(const Player* bot, const float radius, const bool randomRange)
{
    WorldPosition start = *this;

    float angle = rand_norm_f() * 2 * M_PI_F;
    float range = radius;

    if (randomRange)
        range *= rand_norm_f();

    coord_x += range * cos(angle);
    coord_y += range * sin(angle);

    vector<WorldPosition> path = getPathStepFrom(start, bot);
    
    if (path.size())
        set(path.back());
    else
        set(start);

    return path;
}

uint32 WorldPosition::getUnitsAggro(const list<ObjectGuid>& units, const Player* bot) const
{
    uint32 count = 0;
    for (auto guid : units)
    {
        Unit* unit = GuidPosition(guid,bot->GetMapId()).GetUnit(); 
        
        if (!unit) continue; 
        
        if (this->sqDistance(unit) > unit->GetAttackDistance(bot) * unit->GetAttackDistance(bot))
            continue;

        count++;
    }

    return count;
};



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

vector<CreatureDataPair const*> WorldPosition::getCreaturesNear(const float radius, const uint32 entry) const
{
    FindPointCreatureData worker(*this, radius, entry);
    sObjectMgr.DoCreatureData(worker);
    return worker.GetResult();
}

vector<GameObjectDataPair const*> WorldPosition::getGameObjectsNear(const float radius, const uint32 entry) const
{
    FindPointGameObjectData worker(*this, radius, entry);
    sObjectMgr.DoGOData(worker);
    return worker.GetResult();
}
