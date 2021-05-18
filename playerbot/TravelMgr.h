#pragma once

#include "MoveSplineInitArgs.h"

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

    //Extension of WorldLocation with distance functions.
    class WorldPosition
    {
    public:
        //Constructors
        WorldPosition() { wLoc = WorldLocation(); }
        WorldPosition(uint32 mapid, float x, float y, float z = 0, float orientation = 0) { wLoc = WorldLocation(mapid, x, y, z, orientation); }
        WorldPosition(WorldObject* wo) { wLoc = WorldLocation(wo->GetMapId(), wo->GetPositionX(), wo->GetPositionY(), wo->GetPositionZ(), wo->GetOrientation()); }
        WorldPosition(vector<WorldPosition*> list, WorldPositionConst conType);
        WorldPosition(vector<WorldPosition> list, WorldPositionConst conType);
        WorldPosition(uint32 mapid, std::pair<int, int> grid) { wLoc = WorldLocation(mapid, (32 - grid.first) * SIZE_OF_GRIDS, (32 - grid.second) * SIZE_OF_GRIDS, 0, 0); }

        //Setters
        void setX(float x) { wLoc.coord_x = x; }
        void setY(float y) { wLoc.coord_y = y; }
        void setZ(float z) { wLoc.coord_z = z; }

        void addVisitor() { visitors++; }
        void remVisitor() { visitors--; }

        //Getters
        operator bool() const { return  wLoc.mapid != 0 || wLoc.coord_x != 0 || wLoc.coord_y != 0 || wLoc.coord_z != 0;}
        bool operator==(const WorldPosition& p1) { return wLoc.mapid == wLoc.mapid && wLoc.coord_x == p1.wLoc.coord_x && wLoc.coord_y == p1.wLoc.coord_y && wLoc.coord_z == p1.wLoc.coord_z && wLoc.orientation == p1.wLoc.orientation; }
        bool operator!=(const WorldPosition& p1) { return wLoc.mapid != wLoc.mapid || wLoc.coord_x != p1.wLoc.coord_x || wLoc.coord_y != p1.wLoc.coord_y || wLoc.coord_z != p1.wLoc.coord_z || wLoc.orientation != p1.wLoc.orientation; }

        WorldLocation getLocation() { return wLoc; }
        uint32 getMapId() { return wLoc.mapid; }
        float getX() { return wLoc.coord_x; }
        float getY() { return wLoc.coord_y; }
        float getZ() { return wLoc.coord_z; }
        float getO() { return wLoc.orientation; }
        G3D::Vector3 getVector3();
        string print();

        void printWKT(vector<WorldPosition> points, ostringstream& out, uint32 dim = 0, bool loop = false);
        void printWKT(ostringstream& out) { printWKT({ *this }, out); }

        uint32 getVisitors() { return visitors; }

        bool isOverworld() { return wLoc.mapid == 0 || wLoc.mapid == 1 || wLoc.mapid == 530 || wLoc.mapid == 571; }
        bool isInWater() { return getTerrain() ? getTerrain()->IsInWater(wLoc.coord_x, wLoc.coord_y, wLoc.coord_z) : false; };
        bool isUnderWater() { return getTerrain() ? getTerrain()->IsUnderWater(wLoc.coord_x, wLoc.coord_y, wLoc.coord_z) : false; };

        WorldPosition relPoint(WorldPosition* center) { return WorldPosition(wLoc.mapid, wLoc.coord_x - center->wLoc.coord_x, wLoc.coord_y - center->wLoc.coord_y, wLoc.coord_z - center->wLoc.coord_z, wLoc.orientation); }
        WorldPosition offset(WorldPosition* center) { return WorldPosition(wLoc.mapid, wLoc.coord_x + center->wLoc.coord_x, wLoc.coord_y + center->wLoc.coord_y, wLoc.coord_z + center->wLoc.coord_z, wLoc.orientation); }
        float size() { return sqrt(pow(wLoc.coord_x, 2.0) + pow(wLoc.coord_y, 2.0) + pow(wLoc.coord_z, 2.0)); }

        //Slow distance function using possible map transfers.
        float distance(WorldPosition* center);
        float distance(WorldPosition center) { return distance(&center); };

        //Returns the closest point from the list.
        WorldPosition* closest(vector<WorldPosition*> list) { return *std::min_element(list.begin(), list.end(), [this](WorldPosition* i, WorldPosition* j) {return this->distance(i) < this->distance(j); }); }
        WorldPosition closest(vector<WorldPosition> list) { return *std::min_element(list.begin(), list.end(), [this](WorldPosition i, WorldPosition j) {return this->distance(i) < this->distance(j); }); }

        //Quick square distance in 2d plane.
        float sqDistance2d(WorldPosition center) { return (getX() - center.getX()) * (getX() - center.getX()) + (getY() - center.getY()) * (getY() - center.getY()); };

        //Quick square distance calculation without map check. Used for getting the minimum distant points.
        float sqDistance(WorldPosition center) { return (getX() - center.getX()) * (getX() - center.getX()) + (getY() - center.getY()) * (getY() - center.getY()) + (getZ() - center.getZ()) * (getZ() - center.getZ()); };
        float sqDistance(WorldPosition* center) { return (getX() - center->getX()) * (getX() - center->getX()) + (getY() - center->getY()) * (getY() - center->getY()) + (getZ() - center->getZ()) * (getZ() - center->getZ()); };

        //Returns the closest point of the list. Fast but only works for the same map.
        WorldPosition* closestSq(vector<WorldPosition*> list) { return *std::min_element(list.begin(), list.end(), [this](WorldPosition* i, WorldPosition* j) {return this->sqDistance(i) < this->sqDistance(j); }); }
        WorldPosition closestSq(vector<WorldPosition> list) { return *std::min_element(list.begin(), list.end(), [this](WorldPosition i, WorldPosition j) {return this->sqDistance(i) < this->sqDistance(j); }); }

        float getAngleTo(WorldPosition endPos) {float ang = atan2(endPos.getY() - getY(), endPos.getX() - getX()); return (ang >= 0) ? ang : 2 * M_PI_F + ang;};

        WorldPosition lastInRange(vector<WorldPosition> list, float minDist = -1, float maxDist = -1);
        WorldPosition firstOutRange(vector<WorldPosition> list, float minDist = -1, float maxDist = -1);

        float mSign(WorldPosition* p1, WorldPosition* p2) {return(getX() - p2->getX()) * (p1->getY() - p2->getY()) - (p1->getX() - p2->getX()) * (getY() - p2->getY());}
        bool isInside(WorldPosition* p1, WorldPosition* p2, WorldPosition* p3);

        //Map functions. Player independent.
        const MapEntry * getMapEntry() { return sMapStore.LookupEntry(wLoc.mapid); };
        uint32 getInstanceId() { for (auto& map : sMapMgr.Maps()) { if (map.second->GetId() == getMapId()) return map.second->GetInstanceId(); }; return 0; }
        Map*  getMap() { return sMapMgr.FindMap(wLoc.mapid, getMapEntry()->Instanceable() ? wLoc.mapid : 0); }
        const TerrainInfo* getTerrain() { return getMap() ? getMap()->GetTerrain() : NULL; }

        std::set<Transport*> getTransports(uint32 entry = 0);

        std::pair<int,int> getGrid();
        std::vector<pair<int, int>> getGrids(WorldPosition secondPos);
        vector<WorldPosition> fromGrid(int x, int y);

        void loadMapAndVMap(uint32 mapId, int x, int y);
        void loadMapAndVMap() {loadMapAndVMap(getMapId(), getGrid().first, getGrid().second); }
        void loadMapAndVMaps(WorldPosition secondPos);

        //Display functions
        WorldPosition getDisplayLocation();
        float getDisplayX() { return getDisplayLocation().getY() * -1.0; }
        float getDisplayY() { return getDisplayLocation().getX(); }

        uint16 getAreaFlag() { return sTerrainMgr.GetAreaFlag(getMapId(), getX(), getY(), getZ()); };
        AreaTableEntry const* getArea();
        string getAreaName(bool fullName = true, bool zoneName = false);

        vector<WorldPosition> fromPointsArray(std::vector<G3D::Vector3> path);

        //Pathfinding
        vector<WorldPosition> getPathStepFrom(WorldPosition startPos, Unit* bot);
        vector<WorldPosition> getPathFromPath(vector<WorldPosition> startPath, Unit* bot);
        vector<WorldPosition> getPathFrom(WorldPosition startPos, Unit* bot) { return getPathFromPath({ startPos }, bot); };
        vector<WorldPosition> getPathTo(WorldPosition endPos, Unit* bot) { return endPos.getPathFrom(*this, bot); }
        bool isPathTo(vector<WorldPosition> path, float maxDistance = sPlayerbotAIConfig.targetPosRecalcDistance) { return !path.empty() && distance(path.back()) < maxDistance; };
        bool canPathTo(WorldPosition endPos, Unit* bot) { return endPos.isPathTo(getPathTo(endPos, bot)); }

        //Creatures
        vector<CreatureDataPair const*> getCreaturesNear(float radius = 0, uint32 entry = 0);
        //GameObjects
        vector<GameObjectDataPair const*> getGameObjectsNear(float radius = 0, uint32 entry = 0);
    private:
        WorldLocation wLoc;
        uint32 visitors = 0;        
    };

    class mapTransfer
    {
    public:
        mapTransfer(WorldPosition pointFrom1, WorldPosition pointTo1, float portalLength1 = 0.1f)
            : pointFrom(pointFrom1), pointTo(pointTo1), portalLength(portalLength1) {}

        bool isFrom(WorldPosition point) { return point.getMapId() == pointFrom.getMapId(); }
        bool isTo(WorldPosition point) { return point.getMapId() == pointTo.getMapId(); }

        WorldPosition* getPointFrom() { return &pointFrom; }
        WorldPosition* getPointTo() { return &pointTo; }

        bool isUsefull(WorldPosition point) { return isFrom(point) || isTo(point); }
        float distance(WorldPosition point) { return isUsefull(point) ? (isFrom(point) ? point.distance(pointFrom) : point.distance(pointTo)) : 200000; }

        bool isUsefull(WorldPosition start, WorldPosition end) { return isFrom(start) && isTo(end); }
        float distance(WorldPosition start, WorldPosition end) { return (isUsefull(start, end) ? (start.distance(pointFrom) + portalLength + pointTo.distance(end)) : 200000); }
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
        void setExpireDelay(uint32 delay) { expireDelay = delay; }
        void setCooldownDelay(uint32 delay) { cooldownDelay = delay; }
        void setMaxVisitors(uint32 maxVisitors1 = 0, uint32 maxVisitorsPerPoint1 = 0) { maxVisitors = maxVisitors1; maxVisitorsPerPoint = maxVisitorsPerPoint1; }

        vector<WorldPosition*> getPoints(bool ignoreFull = false);
        uint32 getExpireDelay() { return expireDelay; }
        uint32 getCooldownDelay() { return cooldownDelay; }
        void addVisitor() { visitors++; }
        void remVisitor() { visitors--; }
        uint32 getVisitors() { return visitors; }

        virtual Quest const* GetQuestTemplate() { return NULL; }

        virtual bool isActive(Player* bot) { return false; }
        bool isFull(bool ignoreFull = false);

        virtual string getName() { return "TravelDestination"; }
        virtual uint32 getEntry() { return NULL; }
        virtual string getTitle() { return "generic travel destination"; }

        WorldPosition* nearestPoint(WorldPosition* pos);
        float distanceTo(WorldPosition* pos) { return nearestPoint(pos)->distance(pos); }
        virtual bool isIn(WorldPosition* pos) { return distanceTo(pos) <= radiusMin; }
        virtual bool isOut(WorldPosition* pos) { return distanceTo(pos) > radiusMax; }
        float getRadiusMin() { return radiusMin; }

        vector<WorldPosition*> touchingPoints(WorldPosition* pos);
        vector<WorldPosition*> sortedPoints(WorldPosition* pos);
        vector<WorldPosition*> nextPoint(WorldPosition* pos, bool ignoreFull = true);
    protected:
        vector<WorldPosition*> points;
        float radiusMin = 0;
        float radiusMax = 0;

        uint32 visitors = 0;
        uint32 maxVisitors = 0;
        uint32 maxVisitorsPerPoint = 0;
        uint32 expireDelay = 5 * 1000;
        uint32 cooldownDelay = 60 * 1000;
    };

    //Generic creature finder
    class FindPointCreatureData
    {
    public:
        FindPointCreatureData(WorldPosition point1 = WorldPosition(), float radius1 = 0, uint32 entry1 = 0) { point = point1; radius = radius1; entry = entry1; }

        bool operator()(CreatureDataPair const&  dataPair);
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

    //A travel target that is always inactive and jumps to cooldown.
    class NullTravelDestination : public TravelDestination
    {
    public:
        NullTravelDestination(uint32 coodownDelay1 = 5 * 60 * 1000) : TravelDestination() { cooldownDelay = coodownDelay1;};

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
        QuestTravelDestination(uint32 questId1, float radiusMin1, float radiusMax1) : TravelDestination(radiusMin1, radiusMax1) { questId = questId1; questTemplate = sObjectMgr.GetQuestTemplate(questId);}

        virtual Quest const* GetQuestTemplate() { return questTemplate;  }

        virtual bool isActive(Player* bot) { return bot->IsActiveQuest(questId); }

        virtual string getName() { return "QuestTravelDestination"; }
        virtual uint32 getEntry() { return NULL; }
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

        virtual string getName() { return "QuestRelationTravelDestination"; }
        virtual uint32 getEntry() { return entry; }
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
        QuestObjectiveTravelDestination(uint32 quest_id1, uint32 entry1, uint32 objective1, float radiusMin1, float radiusMax1, uint32 itemId1 = 0) : QuestTravelDestination(quest_id1, radiusMin1, radiusMax1) {
            objective = objective1; entry = entry1; itemId = itemId1;
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

        virtual uint32 getEntry() { return entry; }

        virtual string getTitle();
    private:
        uint32 objective;
        int32 entry;
        uint32 itemId = 0;
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
        virtual uint32 getEntry() { return NULL; }
        virtual string getTitle();
    protected:
        uint32 entry;
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
        virtual uint32 getEntry() { return NULL; }
        virtual string getTitle();
        virtual uint32 getAreaId() { return areaId; }
    protected:
        uint32 areaId;
    };
   
    //A quest destination container for quick lookup of all destinations related to a quest.
    struct QuestContainer
    {
        vector<QuestTravelDestination*> questGivers;
        vector<QuestTravelDestination*> questTakers;
        vector<QuestTravelDestination*> questObjectives;
    };

    //
    enum TravelState
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

    enum TravelStatus
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
        void incRetry(bool isMove) { if (isMove) moveRetryCount++; else extendRetryCount++; }
        void setRetry(bool isMove, uint32 newCount = 0) { if (isMove) moveRetryCount = newCount; else extendRetryCount = newCount; }
        void setForced(bool forced1) { forced = forced1; }

        void copyTarget(TravelTarget* target);
        void addVisitors();
        void releaseVisitors();

        float distance(Player* bot) { WorldPosition pos(bot);  return wPosition->distance(&pos); };
        WorldLocation getLocation() { return wPosition->getLocation(); };
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
        bool isMaxRetry(bool isMove) { return isMove ? (moveRetryCount > 5) : (extendRetryCount > 5); }
        TravelStatus getStatus() { return m_status; }

        TravelState getTravelState();


        bool isGroupCopy() { return groupCopy; }
        bool isForced() { return forced; }
    protected:
        TravelStatus m_status = TRAVEL_STATUS_NONE;

        uint32 startTime = WorldTimer::getMSTime();
        uint32 statusTime = 0;

        bool forced = false;
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
        void LoadQuestTravelTable();

        vector <WorldPosition*> getNextPoint(WorldPosition* center, vector<WorldPosition*> points);
        QuestStatusData* getQuestStatus(Player* bot, uint32 questId);
        bool getObjectiveStatus(Player* bot, Quest const* pQuest, uint32 objective);
        uint32 getDialogStatus(Player* pPlayer, int32 questgiver, Quest const* pQuest);
        vector<TravelDestination *> getQuestTravelDestinations(Player* bot, uint32 questId = -1, bool ignoreFull = false, bool ignoreInactive = false, float maxDistance = 5000, bool ignoreObjectives = false);
        vector<TravelDestination*> getRpgTravelDestinations(Player* bot, bool ignoreFull = false, bool ignoreInactive = false, float maxDistance = 5000);
        vector<TravelDestination*> getExploreTravelDestinations(Player* bot, bool ignoreFull = false, bool ignoreInactive = false);


        void setNullTravelTarget(Player* player);

        void addMapTransfer(WorldPosition start, WorldPosition end, float portalDistance = 0.1f);
        void loadMapTransfers();
        float mapTransDistance(WorldPosition start, WorldPosition end);

        NullTravelDestination* nullTravelDestination = new NullTravelDestination();
        WorldPosition* nullWorldPosition = new WorldPosition();

        void addBadVmap(uint32 mapId, int x, int y) { badVmap.push_back(make_tuple(mapId, x, y)); }
        void addBadMmap(uint32 mapId, int x, int y) { badMmap.push_back(make_tuple(mapId, x, y)); }
        bool isBadVmap(uint32 mapId, int x, int y) { return std::find(badVmap.begin(), badVmap.end(), make_tuple(mapId, x, y)) != badVmap.end();}
        bool isBadMmap(uint32 mapId, int x, int y) { return std::find(badMmap.begin(), badMmap.end(), make_tuple(mapId, x, y)) != badMmap.end(); }
    protected:
        void logQuestError(uint32 errorNr, Quest * quest, uint32 objective = 0, uint32 unitId = 0, uint32 itemId = 0);

        vector<QuestTravelDestination*> questGivers;
        vector<RpgTravelDestination*> rpgNpcs;
        
        std::unordered_map<uint32, ExploreTravelDestination*> exploreLocs;
        std::unordered_map<uint32, QuestContainer *> quests;
        std::unordered_map<int32, WorldPosition> pointsMap;

        vector<tuple<uint32,int,int>> badVmap, badMmap;

        vector<mapTransfer> mapTransfers;
    };
}

#define sTravelMgr MaNGOS::Singleton<TravelMgr>::Instance()

