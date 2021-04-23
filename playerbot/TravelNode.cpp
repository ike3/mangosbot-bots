#pragma once

#include "TravelNode.h"
#include "TravelMgr.h"

#include <iomanip>
#include <regex>

#include "MoveMapSharedDefines.h"
#include "MotionGenerators/PathFinder.h"

using namespace ai;
using namespace MaNGOS;

uint32 TravelNode::doPathStep(WorldPosition startPos, WorldPosition endPos, Unit* bot, vector<WorldPosition>& ppath)
{
    Vector3 startVector = Vector3(startPos.getX(), startPos.getY(), startPos.getZ());
    Vector3 endVector = Vector3(endPos.getX(), endPos.getY(), endPos.getZ());

    if (bot)
    {
        //if (!startPos.getMap()->IsLoaded(startPos.getX(), startPos.getY()))
            if (startPos.getTerrain())
                startPos.getTerrain()->GetTerrainType(startPos.getX(), startPos.getY());

        //if (!endPos.getMap()->IsLoaded(endPos.getX(), endPos.getY()))
            if (endPos.getTerrain())
                endPos.getTerrain()->GetTerrainType(endPos.getX(), endPos.getY());
    }

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
        path.calculate(startVector, endVector, false);

        points = path.getPath();
        type = path.getPathType();

    }
    else
#ifdef IKE_PATHFINDER
    {
        PathFinder path(startPos.getMapId(), startPos.getMapEntry()->Instanceable() ? startPos.getInstanceId() : 0);

        path.setAreaCost(8, 10.0f);
        path.setAreaCost(11, 5.0f);
        path.setAreaCost(12, 20.0f);

        path.calculate(startVector, endVector, false);

        points = path.getPath();
        type = path.getPathType();
    }
#else
        return PATHFIND_NOPATH;
#endif

    if (type == PATHFIND_INCOMPLETE || type == PATHFIND_NORMAL)
    {
        for (auto& p : points)
        {
            WorldPosition subPos = WorldPosition(startPos.getMapId(), p.x, p.y, p.z, 0);

            ppath.push_back(subPos);
        }
    }
    else
    {
        ppath.push_back(startPos);
    }

    return type;
}

//Generic routine to remove references to nodes. 
void TravelNode::unlinkNode(TravelNode* node, bool unlinkPaths) {

    if (node) //Unlink this specific node
    {
        //Link to this node
        links.erase(node);
        if (unlinkPaths)
        {
            //Remove path to this node
            paths.erase(node);
        }

            //Remove route to this node
            routes.erase(node);

            //Remove route that passes this node
            for (auto it = begin(routes); it != end(routes);)
            {
                if (it->second.hasNode(node))
                {
                    it = routes.erase(it);
                }
                else
                    ++it;
            }
            
        
    }
    else { //Remove all references to this node.        
        for (auto& node : sTravelNodeMap.getNodes())
        {
            if (node->hasLinkTo(this) || node->hasPathTo(this) || node->hasRouteTo(this))
                node->unlinkNode(this);
        }

        links.clear();
        paths.clear();
        routes.clear();
    }
}

vector<TravelNode*> TravelNode::getNodeMap(bool importantOnly, vector<TravelNode*> ignoreNodes)
{
    vector<TravelNode*> openList;
    vector<TravelNode*> closeList;

    openList.push_back(this);

    uint32 i = 0;

    while (i < openList.size())
    {
        TravelNode* currentNode = openList[i];

        i++;

        if (!importantOnly || currentNode->isImportant())
            closeList.push_back(currentNode);

        for (auto nextLink : currentNode->getLinks())
        {
            TravelNode* nextNode = nextLink.first;
            if (std::find(openList.begin(), openList.end(), nextNode) == openList.end())
            {
                if (ignoreNodes.empty() || std::find(ignoreNodes.begin(), ignoreNodes.end(), nextNode) == ignoreNodes.end())
                    openList.push_back(nextNode);
            }
        }
    }

    return closeList;
}

void TravelNode::cropUselessLinks()
{
    vector<TravelNode*> toRemove;
    for (auto& firstLink : getLinks())
    {
        TravelNode* firstNode = firstLink.first;
        float firstLength = firstLink.second;
        for (auto& secondLink : getLinks())
        {
            TravelNode* secondNode = secondLink.first;
            float secondLength = secondLink.second;

            if (firstNode == secondNode)
                continue;

            if (std::find(toRemove.begin(), toRemove.end(), firstNode) != toRemove.end())
                continue;

            if (firstNode->hasLinkTo(secondNode))
            {
                //Is it quicker to go past first node to reach second node instead of going directly?
                if (firstLength + firstNode->linkLengthTo(secondNode) < secondLength * 1.1)
                {
                    toRemove.push_back(secondNode);
                }
            }
            else if(firstNode->hasCompleteRouteTo(secondNode))
            {
                //Is it quicker to go past first (and multiple) nodes to reach the second node instead of going directly?
                if (firstLength + firstNode->getRouteTo(secondNode).getLength() < secondLength * 1.1)
                {
                    toRemove.push_back(secondNode);
                }
            }
        }

        
    }    

    for (auto& node : toRemove)
        unlinkNode(node, false);
}

bool TravelNode::isEqual(TravelNode* compareNode)
{
    if (!hasLinkTo(compareNode))
        return false;

    if (!compareNode->hasLinkTo(this))
        return false;

    for (auto& node : sTravelNodeMap.getNodes())
    {
        if (node == this || node == compareNode)
            continue;

        if (node->hasLinkTo(this) != node->hasLinkTo(compareNode))
            return false;

        if (hasLinkTo(node) != compareNode->hasLinkTo(node))
            return false;
    }

    return true;
}

bool TravelNode::canPathNode(WorldPosition* startPos, WorldPosition* endPos, Unit* bot, vector<WorldPosition>& vpath)
{
    if (startPos->getMapId() != endPos->getMapId())
        return false;

    WorldPosition currentPos = *startPos;
    vector<WorldPosition> ppath;

    for (uint32 i = 0; i < 40; i++)
    {
        uint32 type = doPathStep(currentPos, *endPos, bot, ppath);

        if (type == PATHFIND_INCOMPLETE || type == PATHFIND_NORMAL)
        {
            for (auto& p : ppath)
            {
                vpath.push_back(p);
            }
        }

        if (type == PATHFIND_NORMAL)
        {
            return true;
        }
        else if (currentPos.distance(&ppath.back()) < sPlayerbotAIConfig.targetPosRecalcDistance)
        {
            return false;
        }

        currentPos = ppath.back();

        ppath.clear();      
    }

    return false;
}

bool TravelNode::canPathNode(TravelNode* endNode, Unit* bot, vector<WorldPosition>& ppath) 
{ 
    if (hasPathTo(endNode))
    {
        ppath = getPathTo(endNode);
        return true;
    }

    //if (!bot || bot->GetMapId() != getMapId() || bot->GetMapId() != endNode->getMapId())
    //    return false;

    WorldPosition* endPos = endNode->getPosition();

    //if (!point.getMap()->IsLoaded(point.getX(), point.getY()) || !endPos->getMap()->IsLoaded(endPos->getX(), endPos->getY()))
    //    return false;

    bool canPath = canPathNode(endNode->getPosition(), bot, ppath);
    if(canPath)
        pathNode(endNode, ppath);
    return canPath; 
}

float TravelNode::getPathLength(vector<WorldPosition>& ppath)
{
    bool first = true;
    float length = 0;
    WorldPosition* prevPos;
    for (auto& p : ppath)
    {
        if (!first)
            length += p.distance(prevPos);

        prevPos = &p;  
        first = false;
    }

    return length;
}

void TravelNode::routeNode(TravelNode* node, TravelNodeRoute route)
{
    if(this != node)
        routes.insert_or_assign(node, route);
}

void TravelNode::routeNode(TravelNode* node, vector<TravelNode*> route)
{
    if (this != node)
        routeNode(node, TravelNodeRoute(route));
}

bool TravelNode::hasRouteTo(TravelNode* node)
{
    auto routeIt = routes.find(node);
    if (routeIt == routes.end())
        return false;
        
    return true;
};

bool TravelNode::hasCompleteRouteTo(TravelNode* node)
{
    auto routeIt = routes.find(node);
    if (routeIt == routes.end())
        return false;

    if (!&routeIt->second)
        return false;

    if (!routeIt->second.hasNode(node))
        return false;
        
    return true;
};

TravelNodeRoute TravelNode::getRouteTo(TravelNode* node)
{
    return routes.find(node)->second;    
}

void TravelNode::clearEmptyRoutes()
{ 
    for (auto it = begin(routes); it != end(routes);)
    {
        if (!&it->second)
        {
            it = routes.erase(it); // previously this was something like m_map.erase(it++);
        }
        else
            ++it;
    }
};

void TravelNode::print(bool printFailed)
{
    WorldPosition* startPosition = getPosition();

    uint32 mapSize = getNodeMap(true).size();

    ostringstream out;
    out << sPlayerbotAIConfig.GetTimestampStr();
    string name = getName();
    name.erase(remove(name.begin(), name.end(), '\"'), name.end());
    out << std::fixed << std::setprecision(2) << "+00, "<< name.c_str() << "," << (isImportant()?1:0) << "," << mapSize << "," << point.getDisplayX() << "," << point.getDisplayY() << "," << getZ();
    sPlayerbotAIConfig.log(1,out.str().c_str());

    vector<WorldPosition> ppath1;
    vector<WorldPosition> ppath2;

    for (auto& endNode : sTravelNodeMap.getNodes())
    {
        if (endNode == this)
            continue;

        if (!printFailed && !hasLinkTo(endNode))
            continue;

        bool canPath1 = hasLinkTo(endNode);

        ppath1 = getPathTo(endNode);

        if (!canPath1 && urand(0,20))
            continue;

        if (ppath1.size() < 2 && canPath1 && getMapId() == endNode->getMapId())
        {
            ppath1.push_back(point);
            ppath1.push_back(*endNode->getPosition());
        }

        if (ppath1.size() > 1)
        {
            ostringstream out;

            uint32 pathType = 1;
            if (!canPath1)
                pathType = 0;
            else if (doTransport(endNode))
                pathType = 2;
            else if (doPortal(endNode))
                pathType = 3;

            out << sPlayerbotAIConfig.GetTimestampStr();
            out << std::fixed << std::setprecision(2) << "+00," << pathType << ", " << (isImportant() ? 1 : 0) << "," << mapSize << ",\"LINESTRING(";

            float x, y = 0;
            for (auto i : ppath1)
            {
                if (i.getY() != y || i.getX() != x)
                    out << i.getDisplayX() << " " << i.getDisplayY() + frand(-2, 2) << ",";
                x = i.getX();
                y = i.getY();
            }

            out << ")\"";

            sPlayerbotAIConfig.log(2, out.str().c_str());
        }
    }
}

float TravelNodeRoute::getLength()
{
    float totalLength = 0;
    for (uint32 i = 0; i<nodes.size()-2 ;i++)
    {
        totalLength += nodes[i]->linkLengthTo(nodes[i + 1]);
    }

    return totalLength;
}

vector<vector<WorldPosition>> TravelNodeRoute::getPaths(Unit* bot, uint32 nodeLimit)
{
    uint32 limit = nodes.size();
    if (nodeLimit > 0 && nodeLimit < limit)
        limit = nodeLimit;

    vector<WorldPosition> path;
    vector<vector<WorldPosition>> paths;

    for (uint32 i = 0; i < limit - 1; i++)
    {
        //If closest point is start of transport/portal go to that node.
        if (nodes[i]->doTransport(nodes[i + 1]) || nodes[i]->doPortal(nodes[i + 1]))
        {
            WorldPosition* pos = nodes[i]->getPosition();
            path.push_back(*pos);
            paths.push_back(path);
            path.clear();
            continue;
        }

        if (nodes[i]->canPathNode(nodes[i + 1], bot, path))
        {
            paths.push_back(path);
            path.clear();
        }
        else
            break;
    }

    return paths;
}

/// Determines the best destination from a route for a bot to head to.
/// 
///  @param[in]		bot   	            The unit to be used for pathfinding details
///  @param[in]		startPosition		The current location of the bot to start searching from
///  @param[in]		endPosition		    The end goal. After the last node we want to move here
/// 
///  @param[out]	isTeleport	        Flag to indiciate destination is a teleport
///  @param[in/out]	longPath		    This already contains the path from the start position to the start node 
///                                     and will have the route past the closest point to the route to the next node after that.
///  @param[in]		shortPath		    This is a shorter path going from the nearest point on the route to the node after that.
/// 
///  @returns The node at the end of the path that has a location nearest to the bot. This will be the teleport destination in case of a teleport/transport.
TravelNode* TravelNodeRoute::getNextNode(Unit* bot, WorldPosition* startPosition, WorldPosition* endPosition, bool& isTeleport, vector<WorldPosition>& longPath, vector<WorldPosition>& shortPath)
{
    float closestDistance = nodes[0]->getDistance(startPosition);
    WorldPosition nextPosition = *nodes[0]->getPosition();
    TravelNode* nextNode = nodes[0];

    vector<WorldPosition> path, currentPath, finalPath, endPath;
    vector<vector<WorldPosition>> paths;

    //Get the paths for the next nodes
    paths = getPaths(bot, 10);

    //The initial situation is that we are heading to or at the start node. 
    //In this case the bot should head past the node to the next. (If start node is not a portal/teleport)
    if (!paths.empty() && (nodes.size() > 1 || (!nodes[0]->doTransport(nodes[1]) && !nodes[0]->doPortal(nodes[1]))))
        finalPath = paths[0];

    if (nodes.size() > 1 && (nodes[0]->doTransport(nodes[1]) || nodes[0]->doPortal(nodes[1])))
    {
        isTeleport = true;
        nextNode = nodes[1];
    }

    if (paths.empty())
    {
        if (isTeleport)
            longPath.push_back(*nodes[0]->getPosition());
        else
            longPath.push_back(*nextNode->getPosition());
    }

    //We now check if the bot is perhaps already on or near the route somewhere.
    for (uint32 i = 0; i < paths.size(); i++)
    {
        path = paths[i];

        //Continue the path
        currentPath.insert(currentPath.end(), path.begin(), path.end());

        for (auto& p : path)
        {
            float dist = startPosition->distance(&p);

            //We found a spot on the path that is closer to the bot than the start node.
            if (!nextNode || dist < closestDistance)
            {
                closestDistance = dist;

                //This spot is a portal or transport node.
                isTeleport = (nodes[i]->doTransport(nodes[i + 1]) || nodes[i]->doPortal(nodes[i + 1]));

                //The spot is along a path between nodes so we want to try to move to the next node.
                nextNode = nodes[i + 1];

                //The long path will be the entire route up to this point up to the next node.
                finalPath = currentPath;

                //We start the short path here.
                shortPath.clear();
            }

            //The short path will get points up to the next node.
            shortPath.push_back(p);
        }
    }

    //Append the path to the start node with the complete path until the next node.
    longPath.insert(longPath.end(), finalPath.begin(), finalPath.end());

    //The path goes all the way to the final node. Extend it to the end position.
    if (nodes.size()-1 == paths.size())
    {
        nodes.back()->canPathNode(endPosition, bot, endPath);

        longPath.insert(longPath.end(), endPath.begin(), endPath.end());
        shortPath.insert(shortPath.end(), endPath.begin(), endPath.end());
    }

    return nextNode;
}

TravelNode* TravelNodeMap::addNode(WorldPosition* pos, string preferedName, bool isImportant, bool checkDuplicate, bool transport, bool portal,uint32 transportId)
{
    TravelNode* newNode;

    if (checkDuplicate)
    {
        newNode = getNode(pos, NULL, 5.0f);
        if (newNode)
            return newNode;
    }

    string finalName = preferedName;

    if (!isImportant)
    {
        std::regex last_num("[[:digit:]]+$");
        finalName = std::regex_replace(finalName, last_num, "");
        uint32 nameCount = 1;

        for (auto& node : getNodes())
        {
            if (node->getName().find(preferedName + to_string(nameCount)) != std::string::npos)
                nameCount++;
        }

        finalName += to_string(nameCount);
    }

    if(!transport && !portal)
        newNode = new TravelNode(pos, finalName, isImportant);
    else
        newNode = new TravelNode(pos, finalName, isImportant, transport, transportId);

    m_nodes.push_back(newNode);

    return newNode;
}

void TravelNodeMap::removeNode(TravelNode* node)
{
    node->unlinkNode(NULL);

    for (auto& tnode : m_nodes)
    {
        if (tnode == node)
        {
            delete tnode;
            tnode = nullptr;
        }
    }

    m_nodes.erase(std::remove(m_nodes.begin(), m_nodes.end(), nullptr), m_nodes.end());
}

void TravelNodeMap::fullLinkNode(TravelNode* startNode, Unit* bot)
{
    if (bot->GetMapId() != startNode->getMapId())
        return;

    WorldPosition* startPosition = startNode->getPosition();
    vector<TravelNode*> linkNodes = getNodes(startPosition);

    for (auto& endNode : linkNodes)
    {
        if (endNode == startNode)
            continue;

        if (startNode->hasLinkTo(endNode))
            continue;

        WorldPosition* endPosition = endNode->getPosition();

        vector<WorldPosition> ppath1;
        vector<WorldPosition> ppath2;

        bool canPath1 = startNode->canPathNode(endPosition, bot, ppath1);
        bool canPath2 = endNode->canPathNode(startPosition, bot, ppath2);

        startNode->pathNode(endNode, ppath1);

        if (canPath1)
        {
            startNode->linkNode(endNode, startNode->getPathLength(ppath1));
        }

        if (canPath2)
        {
            endNode->linkNode(startNode, startNode->getPathLength(ppath2));
        }
    }

    startNode->cropUselessLinks();

    for (auto& Link : startNode->getLinks())
    {
        Link.first->cropUselessLinks();
    }

    startNode->setLinked(true);
}

vector<TravelNode*> TravelNodeMap::getNodes(WorldPosition* pos, float range)
{
    vector<TravelNode*> retVec;
    for (auto& node : m_nodes)
    {
        if (node->getMapId() == pos->getMapId())
            if(range == -1 || node->getDistance(pos) <= range)
                retVec.push_back(node);
    }

    std::sort(retVec.begin(), retVec.end(), [pos](TravelNode* i, TravelNode* j) { return i->getPosition()->distance(pos) < j->getPosition()->distance(pos); });
    return retVec;
}


TravelNode* TravelNodeMap::getNode(WorldPosition* pos, vector<WorldPosition>& ppath, Unit* bot, float range)
{
    float x = pos->getX();
    float y = pos->getY();
    float z = pos->getZ();

    if (bot && !bot->GetMap())
        return NULL;

    uint32 c = 0;

    vector<TravelNode*> nodes = sTravelNodeMap.getNodes(pos, range);
    for (auto& node : nodes)
    {
        if (!bot || node->canPathFrom(pos, bot, ppath))
            return node;

        c++;

        if (c > 5) //Max 5 attempts
            break;
    }

    return NULL;
}

TravelNodeRoute TravelNodeMap::getRoute(TravelNode* start, TravelNode* goal)
{
    //sLog.outError("Path from %s to %s", start->getName().c_str(), goal->getName().c_str());

    if(start == goal)
        return TravelNodeRoute();

    if (start->hasRouteTo(goal))
    {
        if (!start->hasCompleteRouteTo(goal))
            return TravelNodeRoute();

        return start->getRouteTo(goal);
    }

    //Basic A* algoritm
    std::unordered_map<TravelNode*, TravelNodeStub> m_stubs;

    for (auto& node : m_nodes)
    {
        m_stubs.insert(make_pair(node, TravelNodeStub(node)));
    }

    for (auto& i : m_stubs)
    {
        TravelNodeStub* stub = &i.second;
        for (auto& j : i.first->getLinks())
        {
            TravelNode* node = j.first;
            TravelNodeStub* cStub = &m_stubs.find(node)->second;
            if (cStub)
                stub->addChild(cStub, j.second);
        }
    }

    TravelNodeStub* startStub = &m_stubs.find(start)->second;
    TravelNodeStub* goalStub = &m_stubs.find(goal)->second;

    TravelNodeStub* currentNode, * childNode;
    float f, g, h;

    std::vector<TravelNodeStub*> open, closed;

    std::make_heap(open.begin(), open.end(), [](TravelNodeStub* i, TravelNodeStub* j) {return i->m_f < j->m_f; });

    open.push_back(startStub);
    std::push_heap(open.begin(), open.end(), [](TravelNodeStub* i, TravelNodeStub* j) {return i->m_f < j->m_f; });
    startStub->open = true;

    while (!open.empty())
    {
        std::sort(open.begin(), open.end(), [](TravelNodeStub* i, TravelNodeStub* j) {return i->m_f < j->m_f; });

        currentNode = open.front(); // pop n node from open for which f is minimal

        std::pop_heap(open.begin(), open.end(), [](TravelNodeStub* i, TravelNodeStub* j) {return i->m_f < j->m_f; });
        open.pop_back();
        currentNode->open = false;

        currentNode->close = true;
        closed.push_back(currentNode);

        if (currentNode == goalStub)
        {
            TravelNodeStub* parent = currentNode->parent;

            vector<TravelNode*> path;

            path.push_back(currentNode->dataNode);

            while (parent != nullptr)
            {
                path.push_back(parent->dataNode);
                parent = parent->parent;
            }

            reverse(path.begin(), path.end());

            start->routeNode(goal, path);

            return start->getRouteTo(goal);
        }

        for (const auto& children : currentNode->children)// for each successor n' of n
        {
            childNode = static_cast<TravelNodeStub*>(children.first);
            g = currentNode->m_g + children.second; // stance from start + distance between the two nodes
            if ((childNode->open || childNode->close) && childNode->m_g < g) // n' is already in opend or closed with a lower cost g(n')
                continue; // consider next successor

            h = childNode->dataNode->linkLengthTo(goalStub->dataNode);
            f = g + h; // compute f(n')
            childNode->m_f = f;
            childNode->m_g = g;
            childNode->m_h = h;
            childNode->parent = currentNode;

            if (childNode->close)
                childNode->close = false;
            if (!childNode->open)
            {
                open.push_back(childNode);
                std::push_heap(open.begin(), open.end(), [](TravelNodeStub* i, TravelNodeStub* j) {return i->m_f < j->m_f; });
                childNode->open = true;
            }
        }
    }

    start->routeNode(goal, TravelNodeRoute());

    return TravelNodeRoute();
}

TravelNodeRoute TravelNodeMap::getRoute(WorldPosition* startPos, WorldPosition* endPos, vector<WorldPosition>& startPath, Unit* bot)
{    
    if(m_nodes.empty())
       return TravelNodeRoute();

    vector<TravelNode*> startNodes = m_nodes, endNodes = m_nodes;
    //Partial sort to get the closest 5 nodes at the begin of the array.        
    std::partial_sort(startNodes.begin(), startNodes.begin() + 5, startNodes.end(), [startPos](TravelNode* i, TravelNode* j) {return i->getDistance(startPos) < j->getDistance(startPos); });
    std::partial_sort(endNodes.begin(), endNodes.begin() + 5, endNodes.end(), [endPos](TravelNode* i, TravelNode* j) {return i->getDistance(endPos) < j->getDistance(endPos); });

    //Cycle over the combinations of these 5 nodes.
    uint32 startI = 0, endI = 0;
    while (startI < 5 && endI < 5)
    {
        TravelNode* startNode = startNodes[startI];
        TravelNode* endNode = endNodes[endI];

        //Check if there is a route between begin and end. This is relativly cheap since results are stored.
        TravelNodeRoute route = getRoute(startNode, endNode);

        if (!route.isEmpty())
        {
            //Check if the bot can actually walk to this start position.
            //We probably should check (if possible) if the bot can reach the endPos too but the result is not really usefull until the end.
            if (!bot || startNode->canPathFrom(startPos, bot, startPath))
                return route;
            else
                startI++;
        }


        //Prefer a differnt end-node. 
        endI++;

        //Cycle to a different start-node if needed.
        if (endI > startI + 1)
        {
            startI++;
            endI = 0;
        }
    }

    return TravelNodeRoute();
}

bool TravelNodeMap::pathNodeLink(TravelNode* startNode, Unit* bot, bool pathLinked, bool pathUnlinked, uint32 numPaths)
{
    uint32 numRoutes = numPaths * 10;

    if (bot->GetMapId() != startNode->getMapId())
        return false;

    uint32 pathsDone = 0;

    WorldPosition* startPosition = startNode->getPosition();
    vector<TravelNode*> linkNodes = getNodes(startPosition);

    for (auto& endNode : linkNodes)
    {
        if (endNode == startNode)
            continue;

        if (startNode->hasPathTo(endNode))
            continue;

        if (startNode->linkLengthTo(endNode) == 0.1f)
            continue;

        bool hasLink = startNode->hasLinkTo(endNode);

        if (hasLink && !pathLinked)
            continue;

        if (!hasLink && !pathUnlinked)
            continue;

        vector<WorldPosition> ppath;

        bool canPath = startNode->canPathNode(endNode, bot, ppath);

        if (canPath)
            startNode->linkNode(endNode, startNode->getPathLength(ppath));
        else if (hasLink)
            startNode->unlinkNode(endNode, false);

        pathsDone++;

        if (pathsDone > numPaths)
            return true;
    }

    for (auto& endNode : linkNodes)
    {
        if (endNode == startNode)
            continue;

        if (endNode->hasPathTo(startNode))
            continue;

        bool hasLink = endNode->hasLinkTo(startNode);

        if (hasLink && !pathLinked)
            continue;

        if (!hasLink && !pathUnlinked)
            continue;

        vector<WorldPosition> ppath;

        bool canPath = endNode->canPathNode(startNode, bot, ppath);

        if (canPath)
            endNode->linkNode(startNode, endNode->getPathLength(ppath));
        else if (hasLink)
            endNode->unlinkNode(startNode, false);

        pathsDone++;

        if (pathsDone > numPaths)
            return true;
    }

    for (auto& endNode : linkNodes)
    {
        if (endNode == startNode)
            continue;

        if (startNode->hasLinkTo(endNode))
            continue;        

        if (startNode->hasRouteTo(endNode))
            continue;

        TravelNodeRoute route = getRoute(startNode, endNode);
        
        pathsDone++;

        if (pathsDone > numRoutes)
            return true;
    }

    for (auto& endNode : linkNodes)
    {
        if (endNode == startNode)
            continue;

        if (endNode->hasLinkTo(startNode))
            continue;

        if (endNode->hasRouteTo(startNode))
            continue;

        TravelNodeRoute route = getRoute(endNode, startNode);

        pathsDone++;

        if (pathsDone > numRoutes)
            return true;
    }

    startNode->cropUselessLinks();

    for (auto& link : startNode->getLinks())
    {
        link.first->cropUselessLinks();
    }

    //The node structure has changed. Attempt to find routes where none could be found before.
    for (auto& node : startNode->getNodeMap())
    {
        node->clearEmptyRoutes();
    }

    startNode->setLinked(true);

    return pathsDone > 0;
}

bool TravelNodeMap::cropUselessNode(TravelNode* startNode)
{
    if (!startNode->isLinked() || startNode->isImportant())
        return false;

    vector<TravelNode*> ignore = { startNode };

    for (auto& node : getNodes(startNode->getPosition(), 5000))
    {
        if (startNode == node)
            continue;

        if (node->getNodeMap(true).size() > node->getNodeMap(true, ignore).size())
            return false;
    }

    removeNode(startNode);

    return true;
}

TravelNode* TravelNodeMap::addZoneLinkNode(TravelNode* startNode)
{
    for (auto& path : startNode->getPaths())
    {
        TravelNode* endNode = path.first;

        string zoneName = startNode->getPosition()->getAreaName(true, true);
        for (auto pos : path.second)
        {
            string newZoneName = pos.getAreaName(true, true);
            if (zoneName != newZoneName)
            {
                if (!getNode(&pos, NULL, 100.0f))
                {
                    string nodeName = zoneName + " to " + newZoneName;
                    return sTravelNodeMap.addNode(&pos, nodeName, false, true);
                }
                zoneName = newZoneName;
            }

        }
    }

    return NULL;
}

TravelNode* TravelNodeMap::addRandomExtNode(TravelNode* startNode)
{
    std::unordered_map<TravelNode*, vector<WorldPosition>> paths = startNode->getPaths();

    if (paths.empty())
        return NULL;

    for (uint32 i = 0; i < 20; i++)
    {
        auto random_it = std::next(std::begin(paths), urand(0, paths.size() - 1));

        TravelNode* endNode = random_it->first;
        vector<WorldPosition> path = random_it->second;

        if (path.empty())
            continue;

        //Prefer to skip complete links
        if (endNode->hasLinkTo(startNode) && startNode->hasLinkTo(endNode) && !urand(0, 20))
            continue;

        //Prefer to skip no links
        if (!startNode->hasLinkTo(endNode) && !urand(0, 20))
            continue;

        WorldPosition point = path[urand(0, path.size() - 1)];

        if (!getNode(&point, NULL, 100.0f))
            return sTravelNodeMap.addNode(&point, startNode->getName(), false, true);
    }

    return NULL;
}

void TravelNodeMap::manageNodes(Unit* bot, bool mapFull)
{
    bool rePrint = false;

    if (!bot->GetMap())
        return;

    if (m_nMapMtx.try_lock())
    {

        TravelNode* startNode;
        TravelNode* newNode;

        for (auto startNode : m_nodes)
        {
            cropUselessNode(startNode);
        }

        //Pick random Node
        for (uint32 i = 0; i < (mapFull ? 20 : 1); i++)
        {
            vector<TravelNode*> rnodes = getNodes(&WorldPosition(bot));

            if (!rnodes.empty())
            {
                uint32 j = urand(0, rnodes.size() - 1);

                startNode = rnodes[j];
                newNode = NULL;

                bool nodeDone = !pathNodeLink(startNode, bot, mapFull, true, mapFull ? 20 : 1);

                if (!nodeDone)
                    pathNodeLink(startNode, bot, true, false, mapFull ? 20 : 1);

                if (!nodeDone)
                    nodeDone = cropUselessNode(startNode);

                if (!nodeDone && !urand(0, 20))
                    newNode = addZoneLinkNode(startNode);

                if (!nodeDone && !newNode && !urand(0, 20))
                    newNode = addRandomExtNode(startNode);

                rePrint = nodeDone || rePrint || newNode;
            }

        }

        if (rePrint && (mapFull || !urand(0, 20)))
            printMap();

        m_nMapMtx.unlock();
    }
    
    sTravelNodeMap.m_nMapMtx.lock_shared();

    if (!rePrint && mapFull)
        printMap();

    m_nMapMtx.unlock_shared();
}

void TravelNodeMap::printMap()
{

    if (!sPlayerbotAIConfig.hasLog(1) && !sPlayerbotAIConfig.hasLog(2) && !sPlayerbotAIConfig.hasLog(3))
        return;

    sLog.outError("Qgis output.");

    sPlayerbotAIConfig.openLog(1, "w");
    sPlayerbotAIConfig.openLog(2, "w");

    vector<TravelNode*> anodes = getNodes();

    vector<pair<WorldPosition, WorldPosition>> curPos;

    uint32 nr = 0;

    for (auto& node : anodes)
    {
        node->print(true);
        nr++;

        for (auto& snode : anodes)
        {
            if (snode != node)
                curPos.push_back(make_pair(*snode->getPosition(), *node->getPosition()));
        }
    }

    sLog.outError("NodeStore output.");

    sPlayerbotAIConfig.openLog(3, "w");

    std::unordered_map<TravelNode*, uint32> saveNodes;

    sPlayerbotAIConfig.log(3, "#pragma once");
    sPlayerbotAIConfig.log(3, "#include \"TravelMgr.h\"");
    sPlayerbotAIConfig.log(3, "namespace ai");
    sPlayerbotAIConfig.log(3, "    {");
    sPlayerbotAIConfig.log(3, "    class TravelNodeStore");
    sPlayerbotAIConfig.log(3, "    {");
    sPlayerbotAIConfig.log(3, "    public:");
    sPlayerbotAIConfig.log(3, "    static void loadNodes()");
    sPlayerbotAIConfig.log(3, "    {");
    sPlayerbotAIConfig.log(3, "        TravelNode* nodes[%d];", anodes.size());

    for (uint32 i = 0; i < anodes.size(); i++)
    {
        TravelNode* node = anodes[i];

        ostringstream out;

        string name = node->getName();
        name.erase(remove(name.begin(), name.end(), '\"'), name.end());

        out << std::fixed << std::setprecision(2) << "        nodes[" << i << "] = sTravelNodeMap.addNode(&WorldPosition(" << node->getMapId() << "," << node->getX() << "f," << node->getY() << "f," << node->getZ() << "f,0.0), \""
            << name << "\", " << (node->isImportant() ? "true" : "false") << ", true";
        if (node->isTransport() || node->isPortal())
            out << "," << (node->isTransport() ? "true" : "false") << "," << (node->isPortal() ? "true" : "false") << "," << node->getTransportId();

        out << ");";
        sPlayerbotAIConfig.log(3, out.str().c_str());

        saveNodes.insert(make_pair(node, i));
    }

    for (uint32 i = 0; i < anodes.size(); i++)
    {
        TravelNode* node = anodes[i];

        for (auto& Link : node->getLinks())
        {
            ostringstream out;
            out << std::fixed << std::setprecision(1) << "        nodes[" << i << "]->linkNode(nodes[" << saveNodes.find(Link.first)->second << "], " << Link.second << "f);";
            sPlayerbotAIConfig.log(3, out.str().c_str());
        }
    }

    sPlayerbotAIConfig.log(3, "	}");
    sPlayerbotAIConfig.log(3, "};");
    sPlayerbotAIConfig.log(3, "}");

    sLog.outError("saving done.");
}


void TravelNodeMap::calcMapOffset()
{   
    mapOffsets.push_back(make_pair(0, WorldPosition(0, 0, 0, 0, 0)));
    mapOffsets.push_back(make_pair(1, WorldPosition(1, -3680.0, 13670.0, 0, 0)));
    mapOffsets.push_back(make_pair(530, WorldPosition(530, 15000.0, -20000.0, 0, 0)));
    mapOffsets.push_back(make_pair(571, WorldPosition(571, 10000.0, 5000.0, 0, 0)));

    vector<uint32> mapIds;

    for (auto& node : m_nodes)
    {
        if (!node->getPosition()->isOverworld())
            if (std::find(mapIds.begin(), mapIds.end(), node->getMapId()) == mapIds.end())
                mapIds.push_back(node->getMapId());
    }

    std::sort(mapIds.begin(), mapIds.end());

    vector<WorldPosition> min, max;

    for (auto& mapId : mapIds)
    {
        bool doPush = true;
        for (auto& node : m_nodes)
        {
            if (node->getMapId() != mapId)
                continue;

            if (doPush)
            {
                min.push_back(*node->getPosition());
                max.push_back(*node->getPosition());
                doPush = false;
            }
            else
            {
                min.back().setX(std::min(min.back().getX(), node->getX()));
                min.back().setY(std::min(min.back().getY(), node->getY()));
                max.back().setX(std::max(max.back().getX(), node->getX()));
                max.back().setY(std::max(max.back().getY(), node->getY()));
            }
        }
    }

    WorldPosition curPos = WorldPosition(0, -13000, -13000, 0,0);
    WorldPosition endPos = WorldPosition(0, 3000, -13000, 0,0);

    uint32 i = 0;
    float maxY = 0;
    //+X -> -Y
    for (auto& mapId : mapIds)
    {
        mapOffsets.push_back(make_pair(mapId, WorldPosition(mapId, curPos.getX() - min[i].getX(), curPos.getY() - max[i].getY(),0, 0)));

        maxY = std::max(maxY, (max[i].getY() - min[i].getY() + 500));
        curPos.setX(curPos.getX() + (max[i].getX() - min[i].getX() + 500));

        if (curPos.getX() > endPos.getX())
        {
            curPos.setY(curPos.getY() - maxY);
            curPos.setX(-13000);
        }
        i++;
    }
}

WorldPosition TravelNodeMap::getMapOffset(uint32 mapId) 
{
    for (auto& offset : mapOffsets)
    {
        if (offset.first == mapId)
            return offset.second;
    }
    
    return WorldPosition(mapId, 0, 0, 0, 0);
}