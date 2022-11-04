#pragma once

#include "strategy/AiObject.h"
#include "MoveSplineInitArgs.h"
#include <boost/functional/hash.hpp>
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "PlayerbotAIConfig.h"
#include "GridDefines.h"
#include "SharedDefines.h"

class ByteBuffer;

namespace G3D
{
    class Vector2;
    class Vector3;
    class Vector4;
}

namespace ai
{
    //Constructor types for WorldPosition
    enum WorldPositionConst
    {
        WP_RANDOM = 0,
        WP_CENTROID = 1,
        WP_MEAN_CENTROID = 2,
        WP_CLOSEST = 3
    };

    class GuidPosition;

    typedef pair<int, int> mGridPair;

    //Extension of WorldLocation with distance functions.
    class WorldPosition : public WorldLocation
    {
    public:
        //Constructors
        WorldPosition() : WorldLocation() {};
        WorldPosition(const WorldLocation loc) : WorldLocation(loc) {}
        WorldPosition(const WorldPosition& pos) : WorldLocation(pos) {}
        WorldPosition(const string str) { stringstream  out(str); out >> mapid; out >> coord_x; out >> coord_y; out >> coord_z;  out >> orientation; }
        WorldPosition(const uint32 mapid, const float x, const float y, const float z = 0, float orientation = 0) : WorldLocation(mapid, x, y, z, orientation) {}
        WorldPosition(const uint32 mapId, const Position& pos) : WorldLocation(mapId, pos.GetPositionX(), pos.GetPositionY(),pos.GetPositionZ(), pos.GetPositionO()) {}
        WorldPosition(const WorldObject* wo) { if (wo) { set(WorldLocation(wo->GetMapId(), wo->GetPositionX(), wo->GetPositionY(), wo->GetPositionZ(), wo->GetOrientation())); } }
        WorldPosition(const CreatureDataPair* cdPair) { if (cdPair) { set(WorldLocation(cdPair->second.mapid, cdPair->second.posX, cdPair->second.posY, cdPair->second.posZ, cdPair->second.orientation)); } }
        WorldPosition(const GameObjectDataPair* cdPair) { if (cdPair) { set(WorldLocation(cdPair->second.mapid, cdPair->second.posX, cdPair->second.posY, cdPair->second.posZ, cdPair->second.orientation)); } }
        WorldPosition(const GuidPosition& guidP);
        WorldPosition(const vector<WorldPosition*>& list, const WorldPositionConst conType);
        WorldPosition(const vector<WorldPosition>& list, const WorldPositionConst conType);
        WorldPosition(const uint32 mapid, const GridPair grid) : WorldLocation(mapid, (int32(grid.x_coord) - CENTER_GRID_ID - 0.5)* SIZE_OF_GRIDS + CENTER_GRID_OFFSET, (int32(grid.y_coord) - CENTER_GRID_ID - 0.5)* SIZE_OF_GRIDS + CENTER_GRID_OFFSET, 0, 0) {}
        WorldPosition(const uint32 mapid, const CellPair cell) : WorldLocation(mapid, (int32(cell.x_coord) - CENTER_GRID_CELL_ID - 0.5)* SIZE_OF_GRID_CELL + CENTER_GRID_CELL_OFFSET, (int32(cell.y_coord) - CENTER_GRID_CELL_ID - 0.5)* SIZE_OF_GRID_CELL + CENTER_GRID_CELL_OFFSET, 0, 0) {}
        WorldPosition(const uint32 mapid, const mGridPair grid) : WorldLocation(mapid, (32 - grid.first)* SIZE_OF_GRIDS, (32 - grid.second)* SIZE_OF_GRIDS, 0, 0) {}
        WorldPosition(const SpellTargetPosition* pos) : WorldLocation(pos->target_mapId, pos->target_X, pos->target_Y, pos->target_Z) {}
        WorldPosition(const TaxiNodesEntry* pos) : WorldLocation(pos->map_id, pos->x, pos->y, pos->z) {}

        //Setters
        void set(const WorldLocation& pos) { mapid = pos.mapid; coord_x = pos.coord_x; coord_y = pos.coord_y; coord_z = pos.coord_z; orientation = pos.orientation; }
        void set(const WorldPosition& pos) { mapid = pos.mapid; coord_x = pos.coord_x; coord_y = pos.coord_y; coord_z = pos.coord_z; orientation = pos.orientation; }
        void set(const ObjectGuid& guid);
        void setMapId(const uint32 id) { mapid = id; }
        void setX(const float x) { coord_x = x; }
        void setY(const float y) { coord_y = y; }
        void setZ(const float z) { coord_z = z; }
        void setO(const float o) {orientation = o;}

        //Getters
        operator bool() const { return  mapid != 0 || coord_x != 0 || coord_y != 0 || coord_z != 0; }
        bool operator==(const WorldPosition& p1) const { return mapid == p1.mapid && coord_x == p1.coord_x && coord_y == p1.coord_y && coord_z == p1.coord_z && orientation == p1.orientation; }
        bool operator!=(const WorldPosition& p1) const { return mapid != p1.mapid || coord_x != p1.coord_x || coord_y != p1.coord_y || coord_z != p1.coord_z || orientation != p1.orientation; }
        
        WorldPosition& operator+=(const WorldPosition& p1) { coord_x += p1.coord_x; coord_y += p1.coord_y; coord_z += p1.coord_z; return *this; }
        WorldPosition& operator-=(const WorldPosition& p1) { coord_x -= p1.coord_x; coord_y -= p1.coord_y; coord_z -= p1.coord_z; return *this; }

        WorldPosition& operator*=(const float s) { coord_x *= s; coord_y *= s; coord_z *= s; return *this; }
        WorldPosition& operator/=(const float s) { coord_x /= s; coord_y /= s; coord_z /= s; return *this; }

        uint32 getMapId() const { return mapid; }
        float getX() const { return coord_x; }
        float getY() const { return coord_y; }
        float getZ() const { return coord_z; }
        float getO() const { return orientation; }
        G3D::Vector3 getVector3() const;
        virtual string print() const;
        string to_string() const {stringstream out; out << mapid; out << coord_x; out << coord_y; out << coord_z;  out << orientation; return out.str();};

        void printWKT(const vector<WorldPosition>& points, ostringstream& out, const uint32 dim = 0, const bool loop = false) const;
        void printWKT(ostringstream& out) const { printWKT({ *this }, out); }

        bool isOverworld() const { return mapid == 0 || mapid == 1 || mapid == 530 || mapid == 571; }
        bool isInWater() const { return getTerrain() ? getTerrain()->IsInWater(coord_x, coord_y, coord_z) : false; };
        bool isUnderWater() const { return getTerrain() ? getTerrain()->IsUnderWater(coord_x, coord_y, coord_z) : false; };

        WorldPosition relPoint(const WorldPosition& center) const { return WorldPosition(mapid, coord_x - center.coord_x, coord_y - center.coord_y, coord_z - center.coord_z, orientation); }
        WorldPosition offset(const WorldPosition& center) const { return WorldPosition(mapid, coord_x + center.coord_x, coord_y + center.coord_y, coord_z + center.coord_z, orientation); }
        float size() const { return sqrt(pow(coord_x, 2.0) + pow(coord_y, 2.0) + pow(coord_z, 2.0)); }

        //Slow distance function using possible map transfers.
        float distance(const WorldPosition& to) const;

        float fDist(const WorldPosition& to) const;

        //Returns the closest point from the list.
        WorldPosition* closest(const vector<WorldPosition*>& list) const { return *std::min_element(list.begin(), list.end(), [this](WorldPosition* i, WorldPosition* j) {return this->distance(*i) < this->distance(*j); }); }
        WorldPosition closest(const vector<WorldPosition>& list) const { return *std::min_element(list.begin(), list.end(), [this](WorldPosition i, WorldPosition j) {return this->distance(i) < this->distance(j); }); }

        WorldPosition* furtest(const vector<WorldPosition*>& list) const { return *std::max_element(list.begin(), list.end(), [this](WorldPosition* i, WorldPosition* j) {return this->distance(*i) < this->distance(*j); }); }
        WorldPosition furtest(const vector<WorldPosition>& list) const { return *std::max_element(list.begin(), list.end(), [this](WorldPosition i, WorldPosition j) {return this->distance(i) < this->distance(j); }); }

        template<class T>
        pair<T, WorldPosition>  closest(const list<pair<T, WorldPosition>>& list) const { return *std::min_element(list.begin(), list.end(), [this](pair<T, WorldPosition> i, pair<T, WorldPosition> j) {return this->distance(i.second) < this->distance(j.second); }); }
        template<class T>
        pair<T, WorldPosition> closest(const list<T>& list) const { return closest(GetPosList(list)); }

        template<class T>
        pair<T, WorldPosition>  closest(const vector<pair<T, WorldPosition>>& list) const { return *std::min_element(list.begin(), list.end(), [this](pair<T, WorldPosition> i, pair<T, WorldPosition> j) {return this->distance(i.second) < this->distance(j.second); }); }
        template<class T>
        pair<T, WorldPosition> closest(const vector<T>& list) const { return closest(GetPosVector(list)); }


        //Quick square distance in 2d plane.
        float sqDistance2d(const WorldPosition& to) const { return (coord_x - to.coord_x) * (coord_x - to.coord_x) + (coord_y - to.coord_y) * (coord_y - to.coord_y); };

        //Quick square distance calculation without map check. Used for getting the minimum distant points.
        float sqDistance(const WorldPosition& to) const { return (coord_x - to.coord_x) * (coord_x - to.coord_x) + (coord_y - to.coord_y) * (coord_y - to.coord_y) + (coord_z - to.coord_z) * (coord_z - to.coord_z); };

        //Returns the closest point of the list. Fast but only works for the same map.
        WorldPosition* closestSq(const vector<WorldPosition*>& list) const { return *std::min_element(list.begin(), list.end(), [this](WorldPosition* i, WorldPosition* j) {return sqDistance(*i) < sqDistance(*j); }); }
        WorldPosition closestSq(const vector<WorldPosition>& list) const { return *std::min_element(list.begin(), list.end(), [this](WorldPosition i, WorldPosition j) {return sqDistance(i) < sqDistance(j); }); }

        float getAngleTo(const WorldPosition& endPos) const { float ang = atan2(endPos.coord_y - coord_y, endPos.coord_x - coord_x); return (ang >= 0) ? ang : 2 * M_PI_F + ang; };
        float getAngleBetween(const WorldPosition& dir1, const WorldPosition& dir2) const { return abs(getAngleTo(dir1) - getAngleTo(dir2)); };

        WorldPosition limit(const WorldPosition& center, const float maxDistance) { WorldPosition pos(*this); pos -= center; float size = pos.size(); if (size > maxDistance) { pos /= pos.size(); pos *= maxDistance; pos += center; } return pos; }

        WorldPosition lastInRange(const vector<WorldPosition>& list, const float minDist = -1, const float maxDist = -1) const;
        WorldPosition firstOutRange(const vector<WorldPosition>& list, const float minDist = -1, const float maxDist = -1) const;

        float mSign(const WorldPosition* p1, const WorldPosition* p2) const { return(coord_x - p2->coord_x) * (p1->coord_y - p2->coord_y) - (p1->coord_x - p2->coord_x) * (coord_y - p2->coord_y); }
        bool isInside(const WorldPosition* p1, const WorldPosition* p2, const WorldPosition* p3) const;

        void distancePartition(const vector<float>& distanceLimits, WorldPosition* to, vector<vector<WorldPosition*>>& partition) const;
        vector<vector<WorldPosition*>> distancePartition(const vector<float>& distanceLimits, vector<WorldPosition*> points) const;

        //Map functions. Player independent.
        const MapEntry* getMapEntry() const { return sMapStore.LookupEntry(mapid); }
        uint32 getInstanceId() const { for (auto& map : sMapMgr.Maps()) { if (map.second->GetId() == getMapId()) return map.second->GetInstanceId(); }; return 0; }
        Map* getMap() const { return sMapMgr.FindMap(mapid, getMapEntry()->Instanceable() ? getInstanceId() : 0); }
        const TerrainInfo* getTerrain() const { return getMap() ? getMap()->GetTerrain() : NULL; }

#if defined(MANGOSBOT_TWO) || MAX_EXPANSION == 2
        bool IsInLineOfSight(WorldPosition pos, float heightMod = 0.5f) const { return mapid == pos.mapid && getMap() && getMap()->IsInLineOfSight(coord_x, coord_y, coord_z + heightMod, pos.coord_x, pos.coord_y, pos.coord_z + heightMod, 0, true); }
#else
        bool IsInLineOfSight(WorldPosition pos, float heightMod = 0.5f) const { return mapid == pos.mapid && getMap() && getMap()->IsInLineOfSight(coord_x, coord_y, coord_z + heightMod, pos.coord_x, pos.coord_y, pos.coord_z + heightMod, true); }
#endif

        bool isOutside() const { WorldPosition high(*this); high.setZ(coord_z + 500.0f); return IsInLineOfSight(high); }
        bool canFly() const;

#if defined(MANGOSBOT_TWO) || MAX_EXPANSION == 2
        const float getHeight() const { return getMap()->GetHeight(0, coord_x, coord_y, coord_z); }
#else
        float getHeight() const { return getMap() ? getMap()->GetHeight(coord_x, coord_y, coord_z) : coord_z; }
#endif

        float currentHeight() const { return coord_z - getHeight(); }

        std::set<Transport*> getTransports(uint32 entry = 0);

        GridPair getGridPair() const { return MaNGOS::ComputeGridPair(coord_x, coord_y); };
        std::vector<GridPair> getGridPairs(const WorldPosition& secondPos) const;
        vector<WorldPosition> fromGridPair(const GridPair& gridPair) const;

        CellPair getCellPair() const { return MaNGOS::ComputeCellPair(coord_x, coord_y); }
        vector<WorldPosition> fromCellPair(const CellPair& cellPair) const;
        vector<WorldPosition> gridFromCellPair(const CellPair& cellPair) const;

        mGridPair getmGridPair() const {
            return make_pair((int)(32 - coord_x / SIZE_OF_GRIDS), (int)(32 - coord_y / SIZE_OF_GRIDS)); }

        vector<mGridPair> getmGridPairs(const WorldPosition& secondPos) const;
        vector<WorldPosition> frommGridPair(const mGridPair& gridPair) const;

        void loadMapAndVMap(uint32 mapId, uint32 instanceId, int x, int y) const;
        void loadMapAndVMap(uint32 instanceId) const {loadMapAndVMap(getMapId(), instanceId, getmGridPair().first, getmGridPair().second); }
        void loadMapAndVMaps(const WorldPosition& secondPos, uint32 instanceId) const;

        //Display functions
        WorldPosition getDisplayLocation() const;
        float getDisplayX() const { return getDisplayLocation().coord_y * -1.0; }
        float getDisplayY() const { return getDisplayLocation().coord_x; }

        bool isValid() const { return MaNGOS::IsValidMapCoord(coord_x, coord_y, coord_z, orientation); };
        uint16 getAreaFlag() const { return isValid() ? sTerrainMgr.GetAreaFlag(getMapId(), coord_x, coord_y, coord_z) : 0; };
        AreaTableEntry const* getArea() const;
        string getAreaName(const bool fullName = true, const bool zoneName = false) const;
        int32 getAreaLevel() const;

        vector<WorldPosition> fromPointsArray(const std::vector<G3D::Vector3>& path) const;

        //Pathfinding
        vector<WorldPosition> getPathStepFrom(const WorldPosition& startPos, const Unit* bot) const;
        vector<WorldPosition> getPathFromPath(const vector<WorldPosition>& startPath, const Unit* bot, const uint8 maxAttempt = 40) const;
        vector<WorldPosition> getPathFrom(const WorldPosition& startPos, const Unit* bot) { return getPathFromPath({ startPos }, bot); };
        vector<WorldPosition> getPathTo(WorldPosition endPos, const Unit* bot) const { return endPos.getPathFrom(*this, bot); }
        bool isPathTo(const vector<WorldPosition>& path, float const maxDistance = sPlayerbotAIConfig.targetPosRecalcDistance) const { return !path.empty() && distance(path.back()) < maxDistance; };
        bool cropPathTo(vector<WorldPosition>& path, const float maxDistance = sPlayerbotAIConfig.targetPosRecalcDistance) const;
        bool canPathTo(const WorldPosition& endPos, const Unit* bot) const { return endPos.isPathTo(getPathTo(endPos, bot)); }

        float getPathLength(const vector<WorldPosition>& points) const { float dist = 0.0f; for (auto& p : points) if (&p == &points.front()) dist = 0; else dist += std::prev(&p, 1)->distance(p); return dist; }

        bool GetReachableRandomPointOnGround(const Player* bot,const float radius, const bool randomRange = true) {
#ifndef MANGOSBOT_TWO         
            return getMap()->GetReachableRandomPointOnGround(coord_x, coord_y, coord_z, radius, randomRange);
#else
            return getMap()->GetReachableRandomPointOnGround(bot->GetPhaseMask(), coord_x, coord_y, coord_z, radius, randomRange);
#endif
        }

        uint32 getUnitsNear(const list<ObjectGuid>& units, const float radius) const;
        uint32 getUnitsAggro(const list<ObjectGuid>& units, const Player* bot) const;

        //Creatures
        vector<CreatureDataPair const*> getCreaturesNear(const float radius = 0, const uint32 entry = 0) const;
        //GameObjects
        vector<GameObjectDataPair const*> getGameObjectsNear(const float radius = 0, const uint32 entry = 0) const;
    };

    inline ByteBuffer& operator<<(ByteBuffer& b, WorldPosition& guidP)
    {
        b << guidP.getMapId();
        b << guidP.coord_x;
        b << guidP.coord_y;
        b << guidP.coord_z;
        b << guidP.orientation;
        return b;
    }

    inline ByteBuffer& operator>>(ByteBuffer& b, WorldPosition& g)
    {
        uint32 mapid;
        float coord_x;
        float coord_y;
        float coord_z;
        float orientation;
        b >> mapid;
        b >> coord_x;
        b >> coord_y;
        b >> coord_z;
        b >> orientation;

        return b;
    }

    //Generic creature finder
    class FindPointCreatureData
    {
    public:
        FindPointCreatureData(WorldPosition point1 = WorldPosition(), float radius1 = 0, uint32 entry1 = 0) { point = point1; radius = radius1; entry = entry1; }

        bool operator()(CreatureDataPair const& dataPair);
        vector<CreatureDataPair const*> GetResult() const { return data; };
    private:
        WorldPosition point;
        float radius;
        uint32 entry;

        vector<CreatureDataPair const*> data;
    };

    //Generic gameObject finder
    class FindPointGameObjectData
    {
    public:
        FindPointGameObjectData(WorldPosition point1 = WorldPosition(), float radius1 = 0, uint32 entry1 = 0) { point = point1; radius = radius1; entry = entry1; }

        bool operator()(GameObjectDataPair const& dataPair);
        vector<GameObjectDataPair const*> GetResult() const { return data; };
    private:
        WorldPosition point;
        float radius;
        uint32 entry;

        vector<GameObjectDataPair const*> data;
    };

    class GuidPosition : public ObjectGuid, public WorldPosition
    {
    public:
        GuidPosition() : ObjectGuid(), WorldPosition() {}
        GuidPosition(ObjectGuid guid) { ObjectGuid::Set(guid); WorldPosition::set(guid); };
        GuidPosition(ObjectGuid guid, WorldPosition pos) : ObjectGuid(guid), WorldPosition(pos) {};
        GuidPosition(uint64 const& guid, WorldPosition const& pos) : ObjectGuid(guid), WorldPosition(pos) {};
        //template<class T>
        //GuidPosition(ObjectGuid guid, T) : ObjectGuid(guid) {WorldPosition::set(WorldPosition(T))};
        GuidPosition(CreatureDataPair const* dataPair) : ObjectGuid(HIGHGUID_UNIT, dataPair->second.id, dataPair->first), WorldPosition(dataPair) {};
        GuidPosition(GameObjectDataPair const* dataPair) : ObjectGuid(HIGHGUID_GAMEOBJECT, dataPair->second.id, dataPair->first), WorldPosition(dataPair) {};
        GuidPosition(WorldObject* wo) : WorldPosition(wo) { ObjectGuid::Set(wo->GetObjectGuid()); };
        GuidPosition(HighGuid hi, uint32 entry, uint32 counter = 1, WorldPosition pos = WorldPosition()) : ObjectGuid(hi, entry, counter), WorldPosition(pos) {};
        //GuidPosition(const GuidPosition& guidp) {this->Set(guidp);  this->setLocation(((WorldPosition)guidp).getLocation()); };

        CreatureData* GetCreatureData() { return IsCreature() ? sObjectMgr.GetCreatureData(GetCounter()) : nullptr; }
        CreatureInfo const* GetCreatureTemplate()const {return IsCreature() ? sObjectMgr.GetCreatureTemplate(GetEntry()) : nullptr; };

        GameObjectInfo const* GetGameObjectInfo() { return IsGameObject() ? sObjectMgr.GetGameObjectInfo(GetEntry()) : nullptr; };

        WorldObject* GetWorldObject() { return getMap() ? getMap()->GetWorldObject(*this) : nullptr;}
        Creature* GetCreature() const;
        Unit* GetUnit() const;
        GameObject* GetGameObject();
        Player* GetPlayer() const;

        bool HasNpcFlag(NPCFlags flag) { return IsCreature() && GetCreatureTemplate()->NpcFlags & flag; }
        bool isGoType(GameobjectTypes type) { return IsGameObject() && GetGameObjectInfo()->type == type; }

        const FactionTemplateEntry* GetFactionTemplateEntry() const;
        const ReputationRank GetReactionTo(const GuidPosition& other);
        bool IsFriendlyTo(const GuidPosition& other) { return (GetFactionTemplateEntry() && other.GetFactionTemplateEntry()) ? (GetReactionTo(other) > REP_NEUTRAL) : false; }
        bool IsHostileTo(const GuidPosition& other) { return (GetFactionTemplateEntry() && other.GetFactionTemplateEntry()) ? (GetReactionTo(other) < REP_NEUTRAL) : false; }

        bool isDead(); //For loaded grids check if the unit/object is unloaded/dead.

        uint16 IsPartOfAPool();
        uint16 GetGameEventId();
        bool IsEventUnspawned();

        virtual string print();

        operator bool() const { return  WorldPosition(*this) && !IsEmpty(); }
        bool operator== (ObjectGuid const& guid) const { return GetRawValue() == guid.GetRawValue(); }
        bool operator!= (ObjectGuid const& guid) const { return GetRawValue() != guid.GetRawValue(); }
        bool operator< (ObjectGuid const& guid) const { return GetRawValue() < guid.GetRawValue(); }
    };

    inline ByteBuffer& operator<<(ByteBuffer& b, GuidPosition& guidP)
    {
        b << (ObjectGuid)guidP;
        b << (WorldPosition)guidP;


        return b;
    }
        
    inline ByteBuffer& operator>>(ByteBuffer& b, GuidPosition& g)
    {
        ObjectGuid guid;
        WorldPosition pos;

        b >> guid;
        b >> pos;

        g = GuidPosition(guid, pos);

        return b;
    }

    template<class T>
    list<pair<T, WorldPosition>> GetPosList(list<T> oList) { list<pair<T, WorldPosition>> retList; for (auto& obj : oList) retList.push_back(make_pair(obj, WorldPosition(obj))); return retList; };

    template<class T>
    vector<pair<T, WorldPosition>> GetPosVector(vector<T> oList) { vector<pair<T, WorldPosition>> retList; for (auto& obj : oList) retList.push_back(make_pair(obj, WorldPosition(obj))); return retList; };

    class mapTransfer
    {
    public:
        mapTransfer(WorldPosition pointFrom1, WorldPosition pointTo1, float portalLength1 = 0.1f)
            : pointFrom(pointFrom1), pointTo(pointTo1), portalLength(portalLength1) {}

        bool isFrom(WorldPosition point) { return point.getMapId() == pointFrom.getMapId(); }
        bool isTo(WorldPosition point) { return point.getMapId() == pointTo.getMapId(); }

        WorldPosition* getPointFrom() { return &pointFrom; }
        WorldPosition* getPointTo() { return &pointTo; }

        bool isUseful(WorldPosition point) { return isFrom(point) || isTo(point); }
        float distance(WorldPosition point) { return isUseful(point) ? (isFrom(point) ? point.distance(pointFrom) : point.distance(pointTo)) : 200000; }

        bool isUseful(WorldPosition start, WorldPosition end) { return isFrom(start) && isTo(end); }
        float distance(WorldPosition start, WorldPosition end) { return (isUseful(start, end) ? (start.distance(pointFrom) + portalLength + pointTo.distance(end)) : 200000); }
        float fDist(WorldPosition start, WorldPosition end) { return start.fDist(pointFrom) + portalLength + pointTo.fDist(end); }
    private:
        WorldPosition pointFrom, pointTo;
        float portalLength = 0.1f;
    };

    //A destination for a bot to travel to and do something.
    class TravelDestination
    {
    public:
        TravelDestination() {}        
        TravelDestination(float radiusMin1, float radiusMax1) { radiusMin = radiusMin1; radiusMax = radiusMax1; }
        TravelDestination(vector<WorldPosition*> points1, float radiusMin1, float radiusMax1) { points = points1;  radiusMin = radiusMin1; radiusMax = radiusMax1; }

        void addPoint(WorldPosition* pos) { points.push_back(pos); }
        bool hasPoint(WorldPosition* pos) { return std::find(points.begin(), points.end(), pos) != points.end(); }
        void setExpireDelay(uint32 delay) { expireDelay = delay; }
        void setCooldownDelay(uint32 delay) { cooldownDelay = delay; }

        vector<WorldPosition*> getPoints(bool ignoreFull = false);
        uint32 getExpireDelay() { return expireDelay; }
        uint32 getCooldownDelay() { return cooldownDelay; }

        virtual Quest const* GetQuestTemplate() { return NULL; }

        virtual bool isActive(Player* bot) { return false; }

        virtual string getName() { return "TravelDestination"; }
        virtual int32 getEntry() { return 0; }
        virtual string getTitle() { return "generic travel destination"; }

        WorldPosition* nearestPoint(WorldPosition* pos) {return nearestPoint(*pos);};
        WorldPosition* nearestPoint(WorldPosition pos);
        float distanceTo(WorldPosition* pos) { return nearestPoint(pos)->distance(*pos); }
        float distanceTo(WorldPosition pos) { return nearestPoint(pos)->distance(pos); }
        bool onMap(WorldPosition pos) {return nearestPoint(pos)->getMapId() == pos.getMapId();}
        virtual bool isIn(WorldPosition pos, float radius = 0) { return onMap(pos) && distanceTo(pos) <= (radius? radius : radiusMin); }
        virtual bool isOut(WorldPosition pos, float radius = 0) { return !onMap(pos) || distanceTo(pos) > (radius? radius : radiusMax); }
        float getRadiusMin() { return radiusMin; }

        vector<WorldPosition*> touchingPoints(WorldPosition* pos);
        vector<WorldPosition*> sortedPoints(WorldPosition* pos);
        vector<WorldPosition*> nextPoint(WorldPosition* pos, bool ignoreFull = true);
    protected:
        vector<WorldPosition*> points;
        float radiusMin = 0;
        float radiusMax = 0;

        uint32 expireDelay = 5 * 1000;
        uint32 cooldownDelay = 60 * 1000;
    };

    //A travel target that is always inactive and jumps to cooldown.
    class NullTravelDestination : public TravelDestination
    {
    public:
        NullTravelDestination(uint32 cooldownDelay1 = 5 * 60 * 1000) : TravelDestination() { cooldownDelay = cooldownDelay1;};

        virtual Quest const* GetQuestTemplate() { return NULL; }

        virtual bool isActive(Player* bot) { return false; }
        
        virtual string getName() { return "NullTravelDestination"; }
        virtual string getTitle() { return "no destination"; }

        virtual bool isIn(WorldPosition* pos) { return true; }
        virtual bool isOut(WorldPosition* pos) { return false; }

    protected:
    };


    //A travel target specifically related to a quest.
    class QuestTravelDestination : public TravelDestination
    {
    public:
        QuestTravelDestination(uint32 questId1, float radiusMin1, float radiusMax1) : TravelDestination(radiusMin1, radiusMax1) { questId = questId1; questTemplate = sObjectMgr.GetQuestTemplate(questId); }

        virtual Quest const* GetQuestTemplate() { return questTemplate;  }

        virtual bool isActive(Player* bot) { return bot->IsActiveQuest(questId); }
        
        virtual string getName() { return "QuestTravelDestination"; }
        virtual int32 getEntry() { return 0; }
        virtual string getTitle();
    protected:
        uint32 questId;
        Quest const* questTemplate;
    };

    //A quest giver or taker.
    class QuestRelationTravelDestination : public QuestTravelDestination
    {
    public:
        QuestRelationTravelDestination(uint32 quest_id1, uint32 entry1, uint32 relation1, float radiusMin1, float radiusMax1) :QuestTravelDestination(quest_id1, radiusMin1, radiusMax1) { entry = entry1; relation = relation1; }

        virtual bool isActive(Player* bot);

        virtual CreatureInfo const* getCreatureInfo() { return entry > 0 ? ObjectMgr::GetCreatureTemplate(entry) : nullptr; }

        virtual string getName() { return "QuestRelationTravelDestination"; }
        virtual int32 getEntry() { return entry; }
        virtual string getTitle();
        virtual uint32 getRelation() { return relation; }
    private:
        uint32 relation;
        int32 entry;
    };

    //A quest objective (creature/gameobject to grind/loot)
    class QuestObjectiveTravelDestination : public QuestTravelDestination
    {
    public:
        QuestObjectiveTravelDestination(uint32 quest_id1, uint32 entry1, uint32 objective1, float radiusMin1, float radiusMax1) : QuestTravelDestination(quest_id1, radiusMin1, radiusMax1) {
            objective = objective1; entry = entry1;
        }

        bool isCreature() { return GetQuestTemplate()->ReqCreatureOrGOId[objective] > 0; }

        int32 ReqCreature() {
            return isCreature() ? GetQuestTemplate()->ReqCreatureOrGOId[objective] : 0;
        }
        int32 ReqGOId() {
            return !isCreature() ? abs(GetQuestTemplate()->ReqCreatureOrGOId[objective]) : 0;
        }
        uint32 ReqCount() { return GetQuestTemplate()->ReqCreatureOrGOCount[objective]; }

        virtual bool isActive(Player* bot);

        virtual string getName() { return "QuestObjectiveTravelDestination"; }

        virtual int32 getEntry() { return entry; }

        virtual string getTitle();

        virtual uint32 getObjective() { return objective; }
    private:
        uint32 objective;
        int32 entry;
    };

    //A location with rpg target(s) based on race and level
    class RpgTravelDestination : public TravelDestination
    {
    public:
        RpgTravelDestination(uint32 entry1, float radiusMin1, float radiusMax1) : TravelDestination(radiusMin1, radiusMax1) {
            entry = entry1;
        }

        virtual bool isActive(Player* bot);

        virtual CreatureInfo const* getCreatureInfo() { return ObjectMgr::GetCreatureTemplate(entry); }
        virtual string getName() { return "RpgTravelDestination"; }
        virtual int32 getEntry() { return entry; }
        virtual string getTitle();
    protected:
        int32 entry;
    };

    //A location with zone exploration target(s) 
    class ExploreTravelDestination : public TravelDestination
    {
    public:
        ExploreTravelDestination(uint32 areaId1, float radiusMin1, float radiusMax1) : TravelDestination(radiusMin1, radiusMax1) {
            areaId = areaId1;
        }

        virtual bool isActive(Player* bot);

        virtual string getName() { return "ExploreTravelDestination"; }
        virtual int32 getEntry() { return NULL; }
        virtual string getTitle() { return title; }
        virtual void setTitle(std::string newTitle) { title = newTitle; }
        virtual uint32 getAreaId() { return areaId; }
    protected:
        uint32 areaId;
        std::string title = "";
    };

    //A location with zone exploration target(s) 
    class GrindTravelDestination : public TravelDestination
    {
    public:
        GrindTravelDestination(int32 entry1, float radiusMin1, float radiusMax1) : TravelDestination(radiusMin1, radiusMax1) {
            entry = entry1;
        }

        virtual bool isActive(Player* bot);
        virtual CreatureInfo const* getCreatureInfo() { return ObjectMgr::GetCreatureTemplate(entry); }
        virtual string getName() { return "GrindTravelDestination"; }
        virtual int32 getEntry() { return entry; }
        virtual string getTitle();
    protected:
        int32 entry;
    };

    //A location with a boss
    class BossTravelDestination : public TravelDestination
    {
    public:
        BossTravelDestination(int32 entry1, float radiusMin1, float radiusMax1) : TravelDestination(radiusMin1, radiusMax1) {
            entry = entry1; cooldownDelay = 1000;
        }

        virtual bool isActive(Player* bot);
        virtual CreatureInfo const* getCreatureInfo() { return ObjectMgr::GetCreatureTemplate(entry); }
        virtual string getName() { return "BossTravelDestination"; }
        virtual int32 getEntry() { return entry; }
        virtual string getTitle();
    protected:
        int32 entry;
    };
   
    //A quest destination container for quick lookup of all destinations related to a quest.
    struct QuestContainer
    {
        vector<QuestTravelDestination*> questGivers;
        vector<QuestTravelDestination*> questTakers;
        vector<QuestTravelDestination*> questObjectives;
    };

    //
    enum class TravelState : uint8
    {
        TRAVEL_STATE_IDLE = 0,
        TRAVEL_STATE_TRAVEL_PICK_UP_QUEST = 1,
        TRAVEL_STATE_WORK_PICK_UP_QUEST = 2,
        TRAVEL_STATE_TRAVEL_DO_QUEST = 3,
        TRAVEL_STATE_WORK_DO_QUEST = 4,
        TRAVEL_STATE_TRAVEL_HAND_IN_QUEST = 5,
        TRAVEL_STATE_WORK_HAND_IN_QUEST = 6,
        TRAVEL_STATE_TRAVEL_RPG = 7,
        TRAVEL_STATE_TRAVEL_EXPLORE = 8,
        MAX_TRAVEL_STATE
    };

    enum class TravelStatus : uint8
    {
        TRAVEL_STATUS_NONE = 0,
        TRAVEL_STATUS_PREPARE = 1,
        TRAVEL_STATUS_TRAVEL = 2,
        TRAVEL_STATUS_WORK = 3,
        TRAVEL_STATUS_COOLDOWN = 4,
        TRAVEL_STATUS_EXPIRED = 5,
        MAX_TRAVEL_STATUS
    };

    //Current target and location for the bot to travel to.
    //The flow is as follows:
    //PREPARE   (wait until no loot is near)
    //TRAVEL    (move towards target until close enough) (rpg and grind is disabled)
    //WORK      (grind/rpg until the target is no longer active) (rpg and grind is enabled on quest mobs)
    //COOLDOWN  (wait some time free to do what the bot wants)
    //EXPIRE    (if any of the above actions take too long pick a new target)
    class TravelTarget : AiObject
    {
    public:
        TravelTarget(PlayerbotAI* ai) : AiObject(ai) {};
        TravelTarget(PlayerbotAI* ai, TravelDestination* tDestination1, WorldPosition* wPosition1) : AiObject(ai) { setTarget(tDestination1, wPosition1);}
        ~TravelTarget();
        
        void setTarget(TravelDestination* tDestination1, WorldPosition* wPosition1, bool groupCopy1 = false);
        void setStatus(TravelStatus status);
        void setExpireIn(uint32 expireMs) { statusTime = getExpiredTime() + expireMs; }
        void incRetry(bool isMove) { if (isMove) moveRetryCount+=2; else extendRetryCount+=2; }
        void decRetry(bool isMove) { if (isMove && moveRetryCount > 0) moveRetryCount--; else if (extendRetryCount > 0) extendRetryCount--; }
        void setRetry(bool isMove, uint32 newCount = 0) { if (isMove) moveRetryCount = newCount; else extendRetryCount = newCount; }
        void setGroupCopy(bool isGroupCopy = true) { groupCopy = isGroupCopy; }
        void setForced(bool forced1) { forced = forced1; }
        void setRadius(float radius1) { radius = radius1; }

        void copyTarget(TravelTarget* target);

        float distance(Player* bot) { WorldPosition pos(bot);  return wPosition->distance(pos); };
        WorldPosition* getPosition() { return wPosition; };
        TravelDestination* getDestination() { return tDestination; };
        uint32 getEntry() { if (!tDestination) return 0; return tDestination->getEntry(); }
        PlayerbotAI* getAi() { return ai; }

        uint32 getExpiredTime() { return WorldTimer::getMSTime() - startTime; }
        uint32 getTimeLeft() { return statusTime - getExpiredTime(); }
        uint32 getMaxTravelTime() { return (1000.0 * distance(bot)) / bot->GetSpeed(MOVE_RUN); }
        uint32 getRetryCount(bool isMove) { return isMove ? moveRetryCount: extendRetryCount; }

        bool isTraveling(); 
        bool isActive();   
        bool isWorking();
        bool isPreparing();
        bool isMaxRetry(bool isMove) { return isMove ? (moveRetryCount > 10) : (extendRetryCount > 5); }
        TravelStatus getStatus() { return m_status; }

        TravelState getTravelState();

        bool isGroupCopy() { return groupCopy; }
        bool isForced() { return forced; }
    protected:
        TravelStatus m_status = TravelStatus::TRAVEL_STATUS_NONE;

        uint32 startTime = WorldTimer::getMSTime();
        uint32 statusTime = 0;

        bool forced = false;
        float radius = 0;
        bool groupCopy = false;
        bool visitor = true;

        uint32 extendRetryCount = 0;
        uint32 moveRetryCount = 0;

        TravelDestination* tDestination = NULL;
        WorldPosition* wPosition = NULL;
    };

    //General container for all travel destinations.
    class TravelMgr
    {
    public:
        TravelMgr() {};
        void Clear();

        void SetMobAvoidArea();
        void SetMobAvoidAreaMap(uint32 mapId);

        void LoadQuestTravelTable();

        template <class D, class W, class URBG>
        void weighted_shuffle
        (D first, D last
            , W first_weight, W last_weight
            , URBG&& g)
        {
            while (first != last && first_weight != last_weight)
            {
                std::discrete_distribution<int> dd(first_weight, last_weight);
                auto i = dd(g);

                if (i)
                {
                    swap(*first, *std::next(first, i));
                    swap(*first_weight, *std::next(first_weight, i));
                }
                ++first;
                ++first_weight;
            }
        }

        vector <WorldPosition*> getNextPoint(WorldPosition* center, vector<WorldPosition*> points, uint32 amount = 1);
        vector <WorldPosition> getNextPoint(WorldPosition center, vector<WorldPosition> points, uint32 amount = 1);
        QuestStatusData* getQuestStatus(Player* bot, uint32 questId);
        bool getObjectiveStatus(Player* bot, Quest const* pQuest, uint32 objective);
        uint32 getDialogStatus(Player* pPlayer, int32 questgiver, Quest const* pQuest);
        vector<TravelDestination*> getQuestTravelDestinations(Player* bot, int32 questId = -1, bool ignoreFull = false, bool ignoreInactive = false, float maxDistance = 5000, bool ignoreObjectives = false);
        vector<TravelDestination*> getRpgTravelDestinations(Player* bot, bool ignoreFull = false, bool ignoreInactive = false, float maxDistance = 5000);
        vector<TravelDestination*> getExploreTravelDestinations(Player* bot, bool ignoreFull = false, bool ignoreInactive = false);
        vector<TravelDestination*> getGrindTravelDestinations(Player* bot, bool ignoreFull = false, bool ignoreInactive = false, float maxDistance = 5000);
        vector<TravelDestination*> getBossTravelDestinations(Player* bot, bool ignoreFull = false, bool ignoreInactive = false, float maxDistance = 25000);


        void setNullTravelTarget(Player* player);

        void addMapTransfer(WorldPosition start, WorldPosition end, float portalDistance = 0.1f, bool makeShortcuts = true);
        void loadMapTransfers();
        float mapTransDistance(WorldPosition start, WorldPosition end);
        float fastMapTransDistance(WorldPosition start, WorldPosition end);

        NullTravelDestination* nullTravelDestination = new NullTravelDestination();
        WorldPosition* nullWorldPosition = new WorldPosition();

        void addBadVmap(uint32 mapId, int x, int y) { badVmap.push_back(make_tuple(mapId, x, y)); }
        void addBadMmap(uint32 mapId, int x, int y) { badMmap.push_back(make_tuple(mapId, x, y)); }
        bool isBadVmap(uint32 mapId, int x, int y) { return std::find(badVmap.begin(), badVmap.end(), make_tuple(mapId, x, y)) != badVmap.end(); }
        bool isBadMmap(uint32 mapId, int x, int y) { return std::find(badMmap.begin(), badMmap.end(), make_tuple(mapId, x, y)) != badMmap.end(); }


        void printGrid(uint32 mapId, int x, int y, string type);
        void printObj(WorldObject* obj, string type);

        int32 getAreaLevel(uint32 area_id);
        void loadAreaLevels();
    //protected:
        void logQuestError(uint32 errorNr, Quest* quest, uint32 objective = 0, uint32 unitId = 0, uint32 itemId = 0);

        vector<uint32> avoidLoaded;

        vector<QuestTravelDestination*> questGivers;
        vector<RpgTravelDestination*> rpgNpcs;
        vector<GrindTravelDestination*> grindMobs;
        vector<BossTravelDestination*> bossMobs;

        std::unordered_map<uint32, ExploreTravelDestination*> exploreLocs;
        std::unordered_map<uint32, QuestContainer*> quests;
        std::unordered_map<uint64, GuidPosition> pointsMap;

        std::unordered_map<uint32, int32> areaLevels;

        vector<tuple<uint32, int, int>> badVmap, badMmap;

        std::unordered_map<pair<uint32, uint32>, vector<mapTransfer>, boost::hash<pair<uint32, uint32>>> mapTransfersMap;
    };
}

#define sTravelMgr MaNGOS::Singleton<TravelMgr>::Instance()

