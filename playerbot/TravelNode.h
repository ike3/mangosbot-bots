#pragma once

#include "TravelMgr.h"
#include <shared_mutex>


//THEORY
// 
// Pathfinding in (c)mangos is based on detour recast an opensource nashmesh creation and pathfinding codebase.
// This system is used for mob and npc pathfinding and in this codebase also for bots.
// Because mobs and npc movement is based on following a player or a set path the pathfinder is limited to 296y.
// This means that when trying to find a path from A to B distances beyond 296y will be a best guess often moving in a straight path.
// Bots would get stuck moving from Northshire to Stormwind because there is no 296y path that doesn't go (initially) the wrong direction.
// 
// To remedy this limitation without altering the pathfinder limits too much this node system was introduced.
// 
//  <S> ---> [N1] ---> [N2] ---> [N3] ---> <E>
//
// Bot at <S> wants to move to <E>
// [N1],[N2],[N3] are predefined nodes for wich we know we can move from [N1] to [N2] and from [N2] to [N3] but not from [N1] to [N3]
// If we can move fom [S] to [N1] and from [N3] to [E] we have a complete route to travel.
// 
// Termonology:
// Node: a location on a map for which we know bots are likely to want to travel to or need to travel past to reach other nodes.
// Link: the connection between two nodes. A link signifies that the bot can travel from one node to another. A link is one-directional.
// Path: the waypointpath returned by the standard pathfinder to move from one node (or position) to another. A path can be imcomplete or empty which means there is no link.
// Route: the list of nodes that give the shortest route from a node to a distant node. Routes are calculated using a standard A* search based on links.
// 
// On server start saved nodes and links are loaded. Paths and routes are calculated on the fly but saved for future use.
// Nodes can be added and removed realtime however because bots access the nodes from different threads this requires a locking mechanism.
// 
// Initially the current nodes have been made:
// Flightmasters and Inns (Bots can use these to fast-travel so eventually they will be included in the route calculation)
// WorldBosses and Unique bosses in instances (These are a logical places bots might want to go in instances)
// Player start spawns (Obviously all lvl1 bots will spawn and move from here)
// Area triggers locations with teleport and their teleport destinations (These used to travel in or between maps)
// Transports including elevators (Again used to travel in and in maps)
// (sub)Zone means (These are the center most point for each sub-zone which is good for global coverage)
// 
// To increase coverage/linking extra nodes can be automatically be created.
// Current implentation places nodes on paths (including complete) at sub-zone transitions or randomly.
// After calculating possible links the node is removed if it does not create local coverage.
//

    //A connection between two nodes. 
    class TravelNodePath
    {
    public:
        //Constructor for travelnodestore
        TravelNodePath(float distance1 = 0.1f, float extraCost1 = 0, bool portal1 = false, uint32 portalId1 = 0, bool transport1 = false, bool calculated = false, uint32 maxLevelMob1 = 0, uint32 maxLevelAlliance1 = 0, uint32 maxLevelHorde1 = 0, float swimDistance1 = 0);
        //Constructor for portals/transports
        TravelNodePath(vector<WorldPosition> path1, float extraCost1, bool portal1 = false, uint32 portalId1 = 0, bool transport1 = false, uint32 moveSpeed = 0);
        //Getters
        bool getComplete() { return complete; }
        vector<WorldPosition> getPath() { return path; }
        float getDistance() { return distance; }
        bool getPortal() { return portal; }
        uint32 gePortalId() { return portalId; }
        bool getTransport() { return transport; }        
        bool getCalculated() { return calculated; }

        string print();

        //Setters
        void setComplete(bool complete1) { complete = complete1; }
        void setPath(vector<WorldPosition> path1) { path = path1; }
        void setPortal(bool portal1, uint32 portalId1 = 0) { portal = portal1; portalId = portalId1; }
        void setTransport(bool transport1) { transport = transport1; }

        void calculateCost(bool distanceOnly = false);

        float getCost(Unit* bot = nullptr);
    private:
        //Does the path have all the points to get to the destination?
        bool complete = false;

        //List of WorldPositions to get to the destination.
        vector<WorldPosition> path = {};

        //The extra (loading/transport) time it takes to take this path.
        float extraCost = 0;

        bool calculated = false;

        //Derived distance in yards
        float distance = 0.1f;

        //Calculated mobs level along the way.
        uint32 maxLevelMob = 0;      //Hostile mobs
        uint32 maxLevelAlliance = 0; //Hostile to horde
        uint32 maxLevelHorde = 0;    //Hostile to alliance

        //Calculated swiming distances along the way.
        float swimDistance = 0;

        //Is the path a portal/teleport to the destination?
        bool portal = false;
        //Area trigger Id
        uint32 portalId = 0;

        //Is the path transport based?
        bool transport = false;
    };

    //A waypoint to travel from or to.
    //Each node knows which other nodes can be reached without help.
    class TravelNode
    {
    public:
        //Constructors
        TravelNode(WorldPosition* point1, string nodeName1 = "Travel Node", bool important1 = false) { nodeName = nodeName1; point = *point1; important = important1; }
        TravelNode(WorldPosition* point1, string nodeName1, bool important1, bool transport1, uint32 transportId1) { nodeName = nodeName1; point = *point1; important = important1; transport = transport1; transportId = transportId1; }
        TravelNode(TravelNode* baseNode) { nodeName = baseNode->nodeName; point = baseNode->point; important = baseNode->important; transport = baseNode->transport; transportId = baseNode->transportId; }

        //Setters
        void setLinked(bool linked1) { linked = linked1; }

        //Getters
        string getName() { return nodeName; };
        WorldPosition* getPosition() { return &point; };
        std::unordered_map<TravelNode*, TravelNodePath>* getPaths() { return &paths; }
        std::unordered_map<TravelNode*, TravelNodePath*>* getLinks() { return &links; }
        bool isImportant() { return important; };
        bool isLinked() { return linked; }
        bool isTransport() { return transport; }
        uint32 getTransportId() { return transportId; }
        bool isPortal() { for (auto link : *getLinks()) if (link.second->getPortal()) return true; return false; }
               
        //WorldLocation shortcuts
        WorldLocation* getLocation() { return &point.getLocation(); };
        uint32 getMapId() { return point.getMapId(); }
        float getX() { return point.getX(); }
        float getY() { return point.getY(); }
        float getZ() { return point.getZ(); }
        float getO() { return point.getO(); }
        float getDistance(WorldPosition* pos) { return point.distance(pos); }
        float getDistance(TravelNode* node) { return point.distance(node->getPosition()); }

        TravelNodePath* setPathTo(TravelNode* node, TravelNodePath path = TravelNodePath(), bool isLink = true) { if (this != node) { paths.insert_or_assign(node, path); TravelNodePath* tPath = &paths.find(node)->second;  if (isLink) links.insert_or_assign(node, tPath); return tPath; } else return nullptr; }
        bool hasPathTo(TravelNode* node) { return paths.find(node) != paths.end(); }
        TravelNodePath* getPathTo(TravelNode* node) { return &paths.find(node)->second; }
        bool hasCompletePathTo(TravelNode* node) { return hasPathTo(node) && getPathTo(node)->getComplete(); }
        TravelNodePath* buildPath(TravelNode* endNode, Unit* bot, bool postProcess = false);

        void setLinkTo(TravelNode* node, float distance = 0.1f) { if (this != node) { if (!hasPathTo(node)) setPathTo(node, TravelNodePath(distance)); else links.insert_or_assign(node, &paths.find(node)->second);} }
        bool hasLinkTo(TravelNode* node) { return links.find(node) != links.end();}
        float linkCostTo(TravelNode* node) { return paths.find(node)->second.getDistance(); }
        float linkDistanceTo(TravelNode* node) { return paths.find(node)->second.getDistance(); }
        void removeLinkTo(TravelNode* node, bool removePaths = false);

        bool isEqual(TravelNode* compareNode);

        //Removes links to other nodes that can also be reached by passing another node.
        bool isUselessLink(TravelNode* farNode);
        void cropUselessLink(TravelNode* farNode);
        void cropUselessLinks();        

        //Returns all nodes that can be reached from this node.
        vector<TravelNode*> getNodeMap(bool importantOnly = false, vector<TravelNode*> ignoreNodes = {});
        
        void print(bool printFailed = true);
    private:
        //Logical name of the node
        string nodeName;
        //WorldPosition of the node.
        WorldPosition point;

        //List of paths to other nodes.
        std::unordered_map<TravelNode*, TravelNodePath> paths;
        //List of links to other nodes.
        std::unordered_map<TravelNode*, TravelNodePath*> links;

        //This node should not be removed
        bool important = false;

        //This node has been checked for nearby links
        bool linked = false;

        //This node is a (moving) transport.
        bool transport = false;
        //Entry of transport.
        uint32 transportId = 0;
    };

    //Route step type
    enum PathNodeType
    {
        NODE_PREPATH = 0,
        NODE_PATH = 1,
        NODE_NODE = 2,
        NODE_PORTAL = 3,
        NODE_TRANSPORT = 4
    };

    struct PathNodePoint
    {
        WorldPosition point;
        PathNodeType type = NODE_PATH;
        uint32 entry = 0;
    };

    //A complete list of points the bots has to walk to or teleport to.
    class TravelPath
    {
    public:
        TravelPath() {};
        TravelPath(vector<PathNodePoint> fullPath1) { fullPath = fullPath1; }

        void addPoint(PathNodePoint point) { fullPath.push_back(point); }
        void addPoint(WorldPosition point, PathNodeType type = NODE_PATH, uint32 entry = 0) { fullPath.push_back(PathNodePoint{ point, type, entry }); }
        void addPath(vector<WorldPosition> path, PathNodeType type = NODE_PATH, uint32 entry = 0) { for (auto& p : path) { fullPath.push_back(PathNodePoint{ p, type, entry }); }; }
        void addPath(vector<PathNodePoint> newPath) { fullPath.insert(fullPath.end(), newPath.begin(), newPath.end()); }
        void clear() { fullPath.clear(); }

        bool empty() { return fullPath.empty(); }
        vector<PathNodePoint> getPath() { return fullPath; }
        WorldPosition getFront() {return fullPath.front().point; }
        WorldPosition getBack() { return fullPath.back().point; }

        bool makeShortCut(WorldPosition startPos, float maxDist);
        WorldPosition getNextPoint(WorldPosition startPos, float maxDist, bool &isTeleport, bool &isTransport, uint32& entry);

        ostringstream print();
    private:
        vector<PathNodePoint> fullPath;
    };

    //An stored A* search that gives a complete route from one node to another.
    class TravelNodeRoute
    {
    public:
        TravelNodeRoute() {}
        TravelNodeRoute(vector<TravelNode*> nodes1) { nodes = nodes1; /*currentNode = route.begin();*/ }

        bool isEmpty() { return nodes.empty(); }

        bool hasNode(TravelNode* node) { return findNode(node) != nodes.end(); }
        float getTotalDistance();

        vector<TravelNode*> getNodes() { return nodes; }
        
        TravelPath buildPath(vector<WorldPosition> pathToStart = {}, vector<WorldPosition> pathToEnd = {}, Unit* bot = nullptr);

        ostringstream print();
    private:
        vector<TravelNode*>::iterator findNode(TravelNode* node) { return std::find(nodes.begin(), nodes.end(), node); }
        vector<TravelNode*> nodes;
    };
   
    //A node container to aid A* calculations with nodes.
    class TravelNodeStub
    {
    public:
        TravelNodeStub(TravelNode* dataNode1) { dataNode = dataNode1; }

        void addChild(TravelNodeStub* stub, float weight) { children.push_back(make_pair(stub, weight)); }

        TravelNode* dataNode;
        float m_f = 0.0, m_g = 0.0, m_h = 0.0;
        bool open = false, close = false;
        TravelNodeStub* parent = nullptr;
        vector<pair<TravelNodeStub*, float>> children;
    };

    //The container of all nodes.
    class TravelNodeMap
    {
    public:
        TravelNodeMap() {};
        TravelNodeMap(TravelNodeMap* baseMap);

        TravelNode* addNode(WorldPosition* pos, string preferedName = "Travel Node", bool isImportant = false, bool checkDuplicate = true, bool transport = false, uint32 transportId = 0);
        void removeNode(TravelNode* node);
        void fullLinkNode(TravelNode* startNode, Unit* bot);

        //Get all nodes
        vector<TravelNode*> getNodes() { return m_nodes; }
        vector<TravelNode*> getNodes(WorldPosition* pos, float range = -1);

        //Find nearest node.
        TravelNode* getNode(TravelNode* sameNode) { for (auto& node : m_nodes) { if (node->getName() == sameNode->getName() && node->getPosition() == sameNode->getPosition()) return node; } return nullptr; }
        TravelNode* getNode(WorldPosition* pos, vector<WorldPosition>& ppath, Unit* bot = nullptr, float range = -1);
        TravelNode* getNode(WorldPosition* pos, Unit* bot = nullptr, float range = -1) {vector<WorldPosition> ppath; return getNode(pos, ppath, bot, range);}

        //Get Random Node
        TravelNode* getRandomNode(WorldPosition* pos) {vector<TravelNode*> rNodes = getNodes(pos); if (rNodes.empty()) return nullptr; return  rNodes[urand(0, rNodes.size() - 1)]; }

        //Finds the best nodePath between two nodes
        TravelNodeRoute getRoute(TravelNode* start, TravelNode* goal, Unit* bot = nullptr);

        //Find the best node between two positions
        TravelNodeRoute getRoute(WorldPosition* startPos, WorldPosition* endPos, vector<WorldPosition>& startPath, Unit* bot = nullptr);

        //Manage/update nodes
        void manageNodes(Unit* bot, bool mapFull = false);

        //Print map
        void printMap();
        //Print nodStore;
        void printNodeStore();

        bool cropUselessNode(TravelNode* startNode);
        TravelNode* addZoneLinkNode(TravelNode* startNode);
        TravelNode* addRandomExtNode(TravelNode* startNode);

        void calcMapOffset();
        WorldPosition getMapOffset(uint32 mapId);        

        std::shared_mutex m_nMapMtx;
    private:
        vector<TravelNode*> m_nodes;

        vector<pair<uint32, WorldPosition>> mapOffsets;
    };
  

#define sTravelNodeMap MaNGOS::Singleton<TravelNodeMap>::Instance()