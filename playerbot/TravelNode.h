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

    class TravelNodeRoute;

    //A waypoint to travel from or to.
    //Each node knows which other nodes can be reached without help.
    class TravelNode
    {
    public:
        TravelNode(WorldPosition* point1, string nodeName1 = "Travel Node", bool important1 = false) { nodeName = nodeName1; point = *point1; important = important1; }
        TravelNode(WorldPosition* point1, string nodeName1, bool important1, bool transport1, uint32 transportId1) { nodeName = nodeName1; point = *point1; important = important1; transport = transport1; portal = !transport1; transportId = transportId1; }

        void setLinked(bool linked1) { linked = linked1; }

        string getName() { return nodeName; };
        WorldPosition* getPosition() { return &point; };
        std::unordered_map<TravelNode*, float> getLinks() { return links; }
        std::unordered_map<TravelNode*, vector<WorldPosition>> getPaths() { return paths; }
        bool isImportant() { return important; };
        bool isLinked() { return linked; }

        bool isTransport() { return transport; }
        bool isPortal() { return portal; }
        uint32 getTransportId() { return transportId; }
       
        bool doTransport(TravelNode* nextNode) { return (isTransport() && nextNode->isTransport() && getTransportId() == nextNode->getTransportId()); }
        bool doPortal(TravelNode* nextNode) { return (isPortal() && nextNode->isPortal() && getTransportId() == nextNode->getTransportId()); }

        WorldLocation* getLocation() { return &point.getLocation(); };
        uint32 getMapId() { return point.getMapId(); }
        float getX() { return point.getX(); }
        float getY() { return point.getY(); }
        float getZ() { return point.getZ(); }
        float getO() { return point.getO(); }

        float getDistance(WorldPosition* pos) { return point.distance(pos); }
        float getDistance(TravelNode* node) { return point.distance(node->getPosition()); }

        uint32 doPathStep(WorldPosition startPos, WorldPosition endPos, Unit* bot, vector<WorldPosition>& ppath);
        bool canPathNode(WorldPosition* startPos, WorldPosition* endPos, Unit* bot, vector<WorldPosition>& ppath);
        bool canPathNode(WorldPosition* endPos, Unit* bot, vector<WorldPosition>& ppath) {return canPathNode(&point, endPos, bot, ppath);};
        bool canPathNode(WorldPosition* endPos, Unit* bot) { vector<WorldPosition> ppath; return canPathNode(endPos, bot, ppath); }
        bool canPathNode(TravelNode* endNode, Unit* bot, vector<WorldPosition>& ppath);
        bool canPathFrom(WorldPosition* startPos, Unit* bot, vector<WorldPosition>& ppath) { return canPathNode(startPos, &point, bot, ppath); };

        void linkNode(TravelNode* node, float pathLength = -1) { if (this != node) { links.insert_or_assign(node, (pathLength == -1 ? point.distance(node->getPosition()) : (pathLength == 0 ? 0.1 : pathLength))); }; }        
        bool hasLinkTo(TravelNode* node) { return links.find(node) != links.end(); }
        float linkLengthTo(TravelNode* node) { for (auto& link : links) { if (link.first == node) return link.second; } return NULL; }
        void unlinkNode(TravelNode* node, bool unlinkPaths = true);

        //Checks if this node has exactly the same links.
        bool isEqual(TravelNode* node);
        //Removes links to other nodes that can also be reached by passing another node.
        void cropUselessLinks();
        //Returns all nodes that can be reached from this node.
        vector<TravelNode*> getNodeMap(bool importantOnly = false, vector<TravelNode*> ignoreNodes = {});

        void pathNode(TravelNode* node, vector<WorldPosition> path) { if (this != node) { paths.insert_or_assign(node, path); } }
        bool hasPathTo(TravelNode* node) { return paths.find(node) != paths.end(); }
        bool hasCompletePathto(TravelNode* node) { return hasPathTo(node) && !getPathTo(node).empty() ? getPathTo(node).back() == *node->getPosition() : false; }
        float getPathLength(vector<WorldPosition>& ppath);
        vector<WorldPosition> getPathTo(TravelNode* node) { for (auto& path : paths) { if (path.first == node) return path.second; }; vector<WorldPosition> retVec;  return retVec; }
        float getPathLength(TravelNode* node) { return getPathLength(getPathTo(node)); };

        void routeNode(TravelNode* node, TravelNodeRoute route);
        void routeNode(TravelNode* node, vector<TravelNode*> route);

        //void routeNode(TravelNode* node) { routes.insert_or_assign(node, NULL); };
        bool hasRouteTo(TravelNode* node);
        bool hasCompleteRouteTo(TravelNode* node);
        TravelNodeRoute getRouteTo(TravelNode* node);
        void clearEmptyRoutes();
        
        void print(bool printFailed = true);
    private:
        string nodeName;
        WorldPosition point;

        //The nodes bots can travel to directly from this node.
        std::unordered_map<TravelNode*, float> links;
        //The way a unit will travel when asked to travel to a node. Includes incomplete paths.
        std::unordered_map<TravelNode*, vector<WorldPosition>> paths;
        //The nodes a bot has to visit to get to a (distant) node. 
        std::unordered_map<TravelNode*, TravelNodeRoute> routes;

        //This node should not be removed
        bool important = false;

        //This node has been checked for nearby links
        bool linked = false;

        //This node is a (moving) transport stop.
        bool transport = false;
        bool portal = false;
        uint32 transportId = 0;
    };

    //An stored A* search that gives a complete route from one node to another.
    class TravelNodeRoute
    {
    public:
        TravelNodeRoute() {}
        TravelNodeRoute(vector<TravelNode*> nodes1) { nodes = nodes1; /*currentNode = route.begin();*/ }

        bool isEmpty() { return nodes.empty(); }

        bool hasNode(TravelNode* node) { return findNode(node) != nodes.end(); }
        float getLength();

        vector<TravelNode*> getNodes() { return nodes; }
        
        vector<vector<WorldPosition>> TravelNodeRoute::getPaths(Unit* bot, uint32 nodeLimit = -1);

        TravelNode* getNextNode(Unit* bot, WorldPosition* startPosition, WorldPosition* endPosition, bool& isTeleport, vector<WorldPosition>& longPath, vector<WorldPosition>& shortPath);
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
        TravelNodeStub* parent = NULL;
        vector<pair<TravelNodeStub*, float>> children;
    };

    //The container of all nodes.
    class TravelNodeMap
    {
    public:
        TravelNodeMap() {};

        TravelNode* addNode(WorldPosition* pos, string preferedName = "Travel Node", bool isImportant = false, bool checkDuplicate = true, bool transport = false, bool portal = false, uint32 transportId = 0);
        void removeNode(TravelNode* node);
        void fullLinkNode(TravelNode* startNode, Unit* bot);

        //Get all nodes
        vector<TravelNode*> getNodes() { return m_nodes; }
        vector<TravelNode*> getNodes(WorldPosition* pos, float range = -1);

        //Find nearest node.
        TravelNode* getNode(WorldPosition* pos, vector<WorldPosition>& ppath, Unit* bot = NULL, float range = -1);
        TravelNode* getNode(WorldPosition* pos, Unit* bot = NULL, float range = -1) {vector<WorldPosition> ppath; return getNode(pos, ppath, bot, range);}

        //Get Random Node
        TravelNode* getRandomNode(WorldPosition* pos) {vector<TravelNode*> rNodes = getNodes(pos); if (rNodes.empty()) return NULL; return  rNodes[urand(0, rNodes.size() - 1)]; }

        //Finds the best nodePath between two nodes
        TravelNodeRoute getRoute(TravelNode* start, TravelNode* goal, bool saveRoute = true);

        //Find the best node between two positions
        TravelNodeRoute getRoute(WorldPosition* startPos, WorldPosition* endPos, vector<WorldPosition>& startPath, Unit* bot = NULL);

        //Manage/update nodes
        void manageNodes(Unit* bot, bool mapFull = false);

        //Print map
        void printMap();

        bool pathNodeLink(TravelNode* startNode, Unit* bot, bool pathLinked = true, bool pathUnlinked = false, uint32 numPaths = 1);
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