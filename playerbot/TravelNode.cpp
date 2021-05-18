#pragma once

#include "TravelNode.h"
#include "TravelMgr.h"

#include <iomanip>
#include <regex>

#include "MoveMapSharedDefines.h"
#include "MotionGenerators/PathFinder.h"
#include "Entities/Transports.h"

using namespace ai;
using namespace MaNGOS;

TravelNodePath::TravelNodePath(float distance1, float extraCost1, bool portal1, uint32 portalId1, bool transport1, bool calculated1, uint32 maxLevelMob1, uint32 maxLevelAlliance1, uint32 maxLevelHorde1, float swimDistance1)
{
    distance = distance1; 
    extraCost = extraCost1;
    portal = portal1; 
    portalId = portalId1; 
    transport = transport1;  
    calculated = calculated1;
    maxLevelMob = maxLevelMob1;
    maxLevelAlliance = maxLevelAlliance1; 
    maxLevelHorde = maxLevelHorde1;
    swimDistance = swimDistance1;

    if (portal || transport)
        complete = true;
}

TravelNodePath::TravelNodePath(vector<WorldPosition> path1, float extraCost1, bool portal1, uint32 portalId1, bool transport1, uint32 moveSpeed)
{
    complete = true;
    path = path1;
    extraCost = extraCost1;        
    portal = portal1;
    portalId = portalId1;
    transport = transport1;   

    calculateCost();

    if (moveSpeed)
        extraCost += distance / moveSpeed;
}

string TravelNodePath::print()
{
    ostringstream out; 
    out << std::fixed << std::setprecision(1);
    out << distance << "f,";
    out << extraCost << "f,";
    out << (portal ? "true" : "false") << ",";
    out << portalId << ",";
    out << (transport ? "true" : "false") << ",";
    out << (calculated ? "true" : "false") << ",";
    out << maxLevelMob << ",";
    out << maxLevelAlliance << ",";
    out << maxLevelHorde << ",";
    out << swimDistance << "f";

    return out.str().c_str();
}

//Gets the extra information needed to properly calculate the cost.
void TravelNodePath::calculateCost(bool distanceOnly)
{
    std::unordered_map<FactionTemplateEntry const*, bool> aReact, hReact;

    bool aFriend, hFriend;

    if (calculated)
        return;

    distance = 0.1f;
    maxLevelMob = 0;
    maxLevelAlliance = 0;
    maxLevelHorde = 0;
    swimDistance = 0;

    WorldPosition lastPoint = WorldPosition();
    for (auto& point : path)
    {
        if(!distanceOnly)
        for (auto& creaturePair : point.getCreaturesNear(50)) //Agro radius + 5
        {
            CreatureData const cData = creaturePair->second;
            CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(cData.id);

            if (cInfo)
            {
                FactionTemplateEntry const* factionEntry = sFactionTemplateStore.LookupEntry(cInfo->Faction); 

                if (aReact.find(factionEntry) == aReact.end())
                    aReact.insert(make_pair(factionEntry, PlayerbotAI::friendToAlliance(factionEntry)));
                aFriend = aReact.find(factionEntry)->second;

                if (hReact.find(factionEntry) == hReact.end())
                    hReact.insert(make_pair(factionEntry, PlayerbotAI::friendToHorde(factionEntry)));
                hFriend = hReact.find(factionEntry)->second;

                if (maxLevelMob < cInfo->MaxLevel && !aFriend && !hFriend)
                    maxLevelMob = cInfo->MaxLevel;
                if (maxLevelAlliance < cInfo->MaxLevel && aFriend && !hFriend)
                    maxLevelAlliance = cInfo->MaxLevel;
                if (maxLevelHorde < cInfo->MaxLevel && !aFriend && hFriend)
                    maxLevelHorde = cInfo->MaxLevel;
            }
        }

        if (lastPoint && point.getMapId() == lastPoint.getMapId())
        {
            if (!distanceOnly && (point.isInWater() || lastPoint.isInWater()))
                swimDistance += point.distance(lastPoint);

            distance += point.distance(lastPoint);
        }

        lastPoint = point;
    }

    if(!distanceOnly)
        calculated = true;
}

//The cost to travel this path. 
float TravelNodePath::getCost(Unit* bot)
{
    float modifier = 1.0f; //Global modifier
    float timeCost = 0.1f;
    float runDistance = distance - swimDistance; 
    float speed = 8.0f; //default run speed
    float swimSpeed = 4.0f; //default swim speed.

    if(bot)
    {
        //Check if we can use this area trigger.
        if (portal && portalId)
        {
            AreaTrigger const* at = sObjectMgr.GetAreaTrigger(portalId);
            if (at && at->conditionId && !sObjectMgr.IsConditionSatisfied(at->conditionId, bot, bot->GetMap(), nullptr, CONDITION_FROM_AREATRIGGER_TELEPORT))
                return -1;
        }

        speed = bot->GetSpeed(MOVE_RUN);
        swimSpeed = bot->GetSpeed(MOVE_SWIM);

        if (bot->HasSpell(1066))
            swimSpeed *= 1.5;

        uint32 level = bot->getLevel();
        bool isAlliance = PlayerbotAI::friendToAlliance(bot->GetFactionTemplateEntry());
                                  
        int factionAnnoyance = 0;
        int mobAnnoyance = (maxLevelMob - level) - 10; //Mobs 10 levels below do not bother us.

        if(isAlliance)
           factionAnnoyance = (maxLevelHorde - level) - 30;              //Opposite faction below 30 do not bother us.
        else if (!isAlliance)
           factionAnnoyance = (maxLevelAlliance - level) - 30;

        if (mobAnnoyance > 0)
            modifier += 10 * mobAnnoyance;          //For each level the whole path takes 10% longer.
        if (factionAnnoyance > 0)
            modifier += 1 + 0.1 * factionAnnoyance; //For each level the whole path takes 10% longer.
    }

    if (portal || transport)
        timeCost = extraCost * modifier;
    else
        timeCost = (runDistance / speed + swimDistance / swimSpeed) * modifier;

    return timeCost;
}

//Creates or appends the path from one node to another. Returns if the path.
TravelNodePath* TravelNode::buildPath(TravelNode* endNode, Unit* bot, bool postProcess)
{
    if (getMapId() != endNode->getMapId())
        return nullptr;

    TravelNodePath* returnNodePath;

    if (!hasPathTo(endNode))                                //Create path if it doesn't exists
        returnNodePath = setPathTo(endNode, TravelNodePath(), false);
    else
        returnNodePath = getPathTo(endNode);                //Get the exsisting path.

    if (returnNodePath->getComplete())                      //Path is already complete. Return it.
        return returnNodePath;

    vector<WorldPosition> path = returnNodePath->getPath();

    if (path.empty())
        path = { *getPosition() };                      //Start the path from the current Node.

    WorldPosition* endPos = endNode->getPosition();     //Build the path to the end Node.

    path = endPos->getPathFromPath(path, bot);          //Pathfind from the existing path to the end Node.

    bool canPath = endPos->isPathTo(path);              //Check if we reached our destination.

    //Transports are (probably?) not solid at this moment. We need to walk over them so we need extra code for this.
    //Some portals are 'too' solid so we can't properly walk in them. Again we need to bypass this.
    if (!isTransport() && !isPortal() && (endNode->isPortal() || endNode->isTransport()))
    {
        if (endNode->isTransport() && path.back().isInWater()) //Do not swim to boats.
            canPath = false;
        else if (!canPath && endPos->isPathTo(path, 20.0f)) //Cheat a little for transports and portals.
        {
            path.push_back(*endPos);
            canPath = true;

            if (!endNode->hasPathTo(this) || !endNode->getPathTo(this)->getComplete())
            {
                vector<WorldPosition> reversePath = path;
                reverse(reversePath.begin(), reversePath.end());

                TravelNodePath* backNodePath = endNode->setPathTo(this);

                backNodePath->setComplete(canPath);

                endNode->setLinkTo(this, true);

                backNodePath->setPath(reversePath);

                backNodePath->calculateCost(!postProcess);
            }
        }
    }

    if (isTransport() && path.size() > 1)
    {
        WorldPosition secondPos = *std::next(path.begin()); //This is to prevent bots from jumping in the water from a transport. Need to remove this when transports are properly handled.
        if (secondPos.getMap() && secondPos.getTerrain() && secondPos.isInWater())
            canPath = false;
    }

    returnNodePath->setComplete(canPath);

    if(canPath && !hasLinkTo(endNode))
        setLinkTo(endNode, true);

    returnNodePath->setPath(path);

    if (!returnNodePath->getCalculated())
    {
        returnNodePath->calculateCost(!postProcess);
    }
    
    return returnNodePath;
}


//Generic routine to remove references to nodes. 
void TravelNode::removeLinkTo(TravelNode* node, bool removePaths) {

    if (node) //Unlink this specific node
    {
        if (removePaths)
            paths.erase(node);

        links.erase(node);
    }
    else { //Remove all references to this node.        
        for (auto& node : sTravelNodeMap.getNodes())
        {
            if (node->hasPathTo(this))
                node->removeLinkTo(this, removePaths);
        }
        links.clear();
        paths.clear();
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

        for (auto & nextPath : *currentNode->getLinks())
        {
            TravelNode* nextNode = nextPath.first;
            if (std::find(openList.begin(), openList.end(), nextNode) == openList.end())
            {
                if (ignoreNodes.empty() || std::find(ignoreNodes.begin(), ignoreNodes.end(), nextNode) == ignoreNodes.end())
                    openList.push_back(nextNode);
            }
        }
    }

    return closeList;
}

bool TravelNode::isUselessLink(TravelNode* farNode)
{
    float farLength;
    if (hasLinkTo(farNode))
        farLength = getPathTo(farNode)->getDistance();
    else
        farLength = getDistance(farNode);

    for (auto& link : *getLinks())
    {
        TravelNode* nearNode = link.first;
        float nearLength = link.second->getDistance();

        if (farNode == nearNode)
            continue;

        if (farNode->hasLinkTo(this) && !nearNode->hasLinkTo(this))
            continue;

        if (nearNode->hasLinkTo(farNode))
        {
            //Is it quicker to go past second node to reach first node instead of going directly?
            if (nearLength + nearNode->linkDistanceTo(farNode) < farLength * 1.1)
                return true;
            
        }
        else
        {
            TravelNodeRoute route = sTravelNodeMap.getRoute(nearNode, farNode, false);

            if (route.isEmpty())
                continue;

            if (route.hasNode(this))
                continue;

            //Is it quicker to go past second (and multiple) nodes to reach the first node instead of going directly?
            if (nearLength + route.getTotalDistance() < farLength * 1.1)
                return true;
        }
    }

    return false;
}

void TravelNode::cropUselessLink(TravelNode* farNode)
{
    if (isUselessLink(farNode))
        removeLinkTo(farNode);
}

void TravelNode::cropUselessLinks()
{
    for (auto& firstLink : *getPaths())
    {
        TravelNode* farNode = firstLink.first;
        if (this->hasLinkTo(farNode) && this->isUselessLink(farNode))
            this->removeLinkTo(farNode);
        if (farNode->hasLinkTo(this) && farNode->isUselessLink(this))
            farNode->removeLinkTo(this);        
    }

    /*

    //vector<pair<TravelNode*, TravelNode*>> toRemove;
    for (auto& firstLink : getLinks())
    {


        TravelNode* firstNode = firstLink.first;
        float firstLength = firstLink.second.getDistance();
        for (auto& secondLink : getLinks())
        {
            TravelNode* secondNode = secondLink.first;
            float secondLength = secondLink.second.getDistance();

            if (firstNode == secondNode)
                continue;

            if (std::find(toRemove.begin(), toRemove.end(), [firstNode, secondNode](pair<TravelNode*, TravelNode*> pair) {return pair.first == firstNode || pair.first == secondNode;}) != toRemove.end())
                continue;

            if (firstNode->hasLinkTo(secondNode))
            {
                //Is it quicker to go past first node to reach second node instead of going directly?
                if (firstLength + firstNode->linkLengthTo(secondNode) < secondLength * 1.1)
                {
                    if (secondNode->hasLinkTo(this) && !firstNode->hasLinkTo(this))
                        continue;

                    toRemove.push_back(make_pair(this, secondNode));
                }
            }
            else
            {
                TravelNodeRoute route = sTravelNodeMap.getRoute(firstNode, secondNode, false);

                if (route.isEmpty())
                    continue;

                if (route.hasNode(this))
                    continue;

                //Is it quicker to go past first (and multiple) nodes to reach the second node instead of going directly?
                if (firstLength + route.getLength() < secondLength * 1.1)
                {
                    if (secondNode->hasLinkTo(this) && !firstNode->hasLinkTo(this))
                        continue;

                    toRemove.push_back(make_pair(this, secondNode));
                }
            }
        }

        //Reverse cleanup. This is needed when we add a node in an existing map.
        if (firstNode->hasLinkTo(this))
        {
            firstLength = firstNode->getPathTo(this)->getDistance();

            for (auto& secondLink : firstNode->getLinks())
            {
                TravelNode* secondNode = secondLink.first;
                float secondLength = secondLink.second.getDistance();

                if (this == secondNode)
                    continue;

                if (std::find(toRemove.begin(), toRemove.end(), [firstNode, secondNode](pair<TravelNode*, TravelNode*> pair) {return pair.first == firstNode || pair.first == secondNode; }) != toRemove.end())
                    continue;

                if (firstNode->hasLinkTo(secondNode))
                {
                    //Is it quicker to go past first node to reach second node instead of going directly?
                    if (firstLength + firstNode->linkLengthTo(secondNode) < secondLength * 1.1)
                    {
                        if (secondNode->hasLinkTo(this) && !firstNode->hasLinkTo(this))
                            continue;

                        toRemove.push_back(make_pair(this, secondNode));
                    }
                }
                else
                {
                    TravelNodeRoute route = sTravelNodeMap.getRoute(firstNode, secondNode, false);

                    if (route.isEmpty())
                        continue;

                    if (route.hasNode(this))
                        continue;

                    //Is it quicker to go past first (and multiple) nodes to reach the second node instead of going directly?
                    if (firstLength + route.getLength() < secondLength * 1.1)
                    {
                        if (secondNode->hasLinkTo(this) && !firstNode->hasLinkTo(this))
                            continue;

                        toRemove.push_back(make_pair(this, secondNode));
                    }
                }
            }
        }

    }    

    for (auto& nodePair : toRemove)
        nodePair.first->unlinkNode(nodePair.second, false);
        */
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

void TravelNode::print(bool printFailed)
{
    WorldPosition* startPosition = getPosition();

    uint32 mapSize = getNodeMap(true).size();

    ostringstream out;    
    string name = getName();
    name.erase(remove(name.begin(), name.end(), '\"'), name.end());
    out << name.c_str() << ",";
    out << std::fixed << std::setprecision(2);
    point.printWKT(out);
    out << getZ() << ",";
    out << getO() << ",";
    out << (isImportant() ? 1 : 0) << ",";
    out << mapSize;

    sPlayerbotAIConfig.log("travelNodes.csv",out.str().c_str());

    vector<WorldPosition> ppath;

    for (auto& endNode : sTravelNodeMap.getNodes())
    {
        if (endNode == this)
            continue;

        if (!hasPathTo(endNode))
            continue;

        TravelNodePath* path = getPathTo(endNode);

        if (!hasLinkTo(endNode) && urand(0,20) && !printFailed)
            continue;

        ppath = path->getPath();

        if (ppath.size() < 2 && hasLinkTo(endNode))
        {
            ppath.push_back(point);
            ppath.push_back(*endNode->getPosition());
        }

        if (ppath.size() > 1)
        {
            ostringstream out;

            uint32 pathType = 1;
            if (!hasLinkTo(endNode))
                pathType = 0;
            else if (path->getTransport())
                pathType = 2;
            else if (path->getPortal() && getMapId() == endNode->getMapId())
                pathType = 3;
            else if (path->getPortal())
                pathType = 4;

            out << pathType << ",";
            out << std::fixed << std::setprecision(2);
            point.printWKT(ppath, out, 1);
            out << path->getPortalId() << ",";
            out << path->getDistance() << ",";
            out << path->getCost();

            sPlayerbotAIConfig.log("travelPaths.csv", out.str().c_str());
        }
    }
}

//Attempts to move ahead of the path.
bool TravelPath::makeShortCut(WorldPosition startPos, float maxDist)
{
    if (getPath().empty())
        return false;

    float minDist = -1;
    vector<PathNodePoint> newPath;
    WorldPosition firstNode;
    for (auto p : fullPath) //cycle over the full path
    {
        if (p.point.getMapId() != startPos.getMapId())
            continue;

        float curDist = p.point.sqDistance(startPos);

        if (curDist < sPlayerbotAIConfig.tooCloseDistance * sPlayerbotAIConfig.tooCloseDistance) //We are on the path. No shortcut.
            return false;

        if (p.type != NODE_PREPATH) //Only look at the part after the first node and in the same map.
        {
            if (!firstNode)
                firstNode = p.point;

            if (minDist == -1 || curDist < minDist) //Start building from the last closest point.
            {
                minDist = curDist;
                newPath.clear();
            }
        }
        newPath.push_back(p);
    }

    if (newPath.empty() || minDist > maxDist)
    {
        clear();
        return false;
    }

    WorldPosition beginPos = newPath.begin()->point;

    //The old path seems to be the best.
    if (beginPos.distance(firstNode) < sPlayerbotAIConfig.tooCloseDistance)
       return false;

    //We are (nearly) on the new path. Just follow the rest.
    if (beginPos.distance(startPos) < sPlayerbotAIConfig.tooCloseDistance)
    {
        fullPath = newPath;
        return true;
    }

    vector<WorldPosition> toPath = startPos.getPathTo(beginPos, NULL);
    
    //We can not reach the new begin position. Follow the complete path.
    if (!beginPos.isPathTo(toPath))
        return false;

    //Move to the new path and continue.
    fullPath.clear();
    addPath(toPath);
    addPath(newPath);  

    return true;
}

//Next position to move to
WorldPosition TravelPath::getNextPoint(WorldPosition startPos, float maxDist, bool& isTeleport, bool& isTransport, uint32& entry)
{
    if (getPath().empty())
        return WorldPosition();

    auto beg = fullPath.begin();
    auto ed = fullPath.end();

    float minDist = 0.0f;
    auto startP = beg;

    for (auto p = startP; p!=ed;p++)
    {
        if (p->point.getMapId() != startPos.getMapId())
            continue;

        float curDist = p->point.distance(startPos);

        if (curDist <= minDist || startP == beg)
        {
            minDist = curDist;
            startP = p;
        }
    }

    float moveDist = startP->point.distance(startPos);


    for (auto p = startP + 1; p != ed; p++)
    {
        auto prevP = std::prev(p);
        auto nextP = std::next(p);

        if (nextP == ed)
        {
            startP = p;
            break;
        }

        //Teleport with next point to a new map.
        if (p->type == NODE_PORTAL && nextP->type == NODE_PORTAL && p->entry == nextP->entry)
        {
            startP = p; //Move to teleport and activate area trigger.
            break;
        }

        //Transport with entry. 
        if (p->type == NODE_TRANSPORT && p->entry)
        {
            if (nextP->type != NODE_TRANSPORT)
                continue;

            if (startPos.distance(prevP->point) > 5.0f)
            {
                startP = prevP;
                break;
            }
        }

        float nextMove = p->point.distance(nextP->point);

        if (p->point.getMapId() != startPos.getMapId() || moveDist + nextMove > maxDist)
        {
            startP = p;
            break;
        }

        moveDist += nextMove;
    }

    isTeleport = false;
    isTransport = false;

    //We are moving towards a teleport. Move to portal an activate area trigger
    if (startP->type == NODE_PORTAL)
    {
        isTeleport = true;
        entry = startP->entry;
        return startP->point;
    }
  
    //We are moving towards transport. Teleport to next normal point instead.
    if (startP->type == NODE_TRANSPORT)
    {
        for (auto p = startP + 1; p != ed; p++)
        {
            if (p->type != NODE_TRANSPORT)
            {
                isTeleport = true;
                entry = 0;
                return p->point;
            }
        }
    }

    //We have to move far for next point. Try to make a cropped path.
    if (moveDist < sPlayerbotAIConfig.targetPosRecalcDistance && std::next(startP) != ed)
    {
        //vector<WorldPosition> path = startPos.getPathTo(std::next(startP)->point, nullptr);
        //startP->point = startPos.lastInRange(path, -1, maxDist);
        return WorldPosition();
    }

    return startP->point;
}

ostringstream TravelPath::print()
{
    ostringstream out;

    out << sPlayerbotAIConfig.GetTimestampStr();
    out << "+00," << "1,";
    out << std::fixed;

    WorldPosition().printWKT(getPointPath(), out, 1);

    return out;
}

float TravelNodeRoute::getTotalDistance()
{
    float totalLength = 0;
    for (uint32 i = 0; i<nodes.size()-2 ;i++)
    {
        totalLength += nodes[i]->linkDistanceTo(nodes[i + 1]);
    }

    return totalLength;
}

TravelPath TravelNodeRoute::buildPath(vector<WorldPosition> pathToStart, vector<WorldPosition> pathToEnd, Unit* bot)
{
    TravelPath travelPath;

    if (!pathToStart.empty()) //From start position to start of path.
        travelPath.addPath(pathToStart, NODE_PREPATH);

    TravelNode* prevNode = nullptr;
    for (auto& node : nodes)
    {
        if(prevNode)
        {
            TravelNodePath* nodePath = nullptr;
            if(prevNode->hasPathTo(node))  //Get the path to the next node if it exists.
                nodePath = prevNode->getPathTo(node);

            if (!nodePath || !nodePath->getComplete()) //Build the path to the next node if it doesn't exist.
            {
                if(!prevNode->isTransport())
                    nodePath = prevNode->buildPath(node, NULL);
                else //For transports we have no proper path since the node is in air/water. Instead we build a reverse path and follow that.
                {
                    node->buildPath(prevNode, NULL); //Reverse build to get proper path.
                    nodePath = prevNode->getPathTo(node);
                }
            }

            TravelNodePath returnNodePath;

            if (!nodePath || !nodePath->getComplete()) //It looks like we can't properly path to our node. Make a temporary reverse path and see if that works instead.
            {
                returnNodePath = *node->buildPath(prevNode, NULL); //Build reverse path and save it to a temporary variable.
                vector<WorldPosition> path = returnNodePath.getPath();
                reverse(path.begin(), path.end()); //Reverse the path 
                returnNodePath.setPath(path);      
                nodePath = &returnNodePath;
            }

            if (!nodePath || !nodePath->getComplete()) //If we can not build a path just try to move to the node.
            {
                travelPath.addPoint(*prevNode->getPosition(), NODE_NODE);
                prevNode = node;
                continue;
            }

            vector<WorldPosition> path = nodePath->getPath();

            if (node != nodes.back()) //Remove the last point since that will also be the start of the next path.
                path.pop_back();

            if (prevNode->isPortal() && !nodePath->getPortal()) //Do not move to the area trigger if we don't plan to take the portal.
                path.erase(path.begin());

            if (prevNode->isTransport() && !nodePath->getTransport()) //Do not move to the transport if we aren't going to take it.
                path.erase(path.begin());

            if (nodePath->getPortal()) //Teleport to next node.
            {
                travelPath.addPoint(*prevNode->getPosition(), NODE_PORTAL, nodePath->getPortalId()); //Entry point
                travelPath.addPoint(*node->getPosition(), NODE_PORTAL, nodePath->getPortalId());     //Exit point
            }
            else if (nodePath->getTransport()) //Move onto transport
            {
                travelPath.addPoint(*prevNode->getPosition(), NODE_TRANSPORT, node->getTransportId()); //Departure point
                travelPath.addPoint(*node->getPosition(), NODE_TRANSPORT, node->getTransportId());     //Arrival point        
            }
            else
            {
                travelPath.addPath(path, NODE_PATH);
            }
        }
        prevNode = node;
    }

    if (!pathToEnd.empty())
        travelPath.addPath(pathToEnd, NODE_PATH);    

    return travelPath;
}

ostringstream TravelNodeRoute::print()
{
    ostringstream out;

    out << sPlayerbotAIConfig.GetTimestampStr();
    out << "+00" << ",0," << "\"LINESTRING(";

    for (auto& node : nodes)
    {
        out << std::fixed << node->getPosition()->getDisplayX() << " " << node->getPosition()->getDisplayY() << ",";
    }

    out << ")\"";

    return out;
}

TravelNodeMap::TravelNodeMap(TravelNodeMap* baseMap)
{
    TravelNode* newNode;

    baseMap->m_nMapMtx.lock_shared();

    for (auto & node : baseMap->getNodes())
    {
        newNode = new TravelNode(node);

        m_nodes.push_back(newNode);
    }

    for (auto & node : baseMap->getNodes())
    {
        newNode = getNode(node);

        for (auto& path : *node->getPaths())
        {
            TravelNode* endNode = getNode(path.first);

            newNode->setPathTo(endNode, path.second);
        }
    }

    baseMap->m_nMapMtx.unlock_shared();
}

TravelNode* TravelNodeMap::addNode(WorldPosition* pos, string preferedName, bool isImportant, bool checkDuplicate, bool transport,uint32 transportId)
{
    TravelNode* newNode;

    if (checkDuplicate)
    {
        newNode = getNode(pos, nullptr , 5.0f);
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

    if(!transport)
        newNode = new TravelNode(pos, finalName, isImportant);
    else
        newNode = new TravelNode(pos, finalName, isImportant, transport, transportId);

    m_nodes.push_back(newNode);

    return newNode;
}

void TravelNodeMap::removeNode(TravelNode* node)
{
    node->removeLinkTo(NULL, true);

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
    WorldPosition* startPosition = startNode->getPosition();
    vector<TravelNode*> linkNodes = getNodes(startPosition);

    for (auto& endNode : linkNodes)
    {
        if (endNode == startNode)
            continue;

        if (startNode->hasLinkTo(endNode))
            continue;

        startNode->buildPath(endNode, bot);
        endNode->buildPath(startNode, bot);        
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
        if (!bot || pos->canPathTo(*node->getPosition(), bot))
            return node;

        c++;

        if (c > 5) //Max 5 attempts
            break;
    }

    return NULL;
}

TravelNodeRoute TravelNodeMap::getRoute(TravelNode* start, TravelNode* goal, Unit* bot)
{
    if(start == goal)
        return TravelNodeRoute();

    //Basic A* algoritm
    std::unordered_map<TravelNode*, TravelNodeStub> m_stubs;

    for (auto& node : m_nodes)
    {
        m_stubs.insert(make_pair(node, TravelNodeStub(node)));
    }

    for (auto& i : m_stubs)
    {
        TravelNodeStub* stub = &i.second;
        for (auto& j : *i.first->getLinks())
        {
            TravelNode* node = j.first;
            float cost = j.second->getCost(bot);
            if (cost > 0)
            {
                TravelNodeStub* cStub = &m_stubs.find(node)->second;
                if (cStub)
                    stub->addChild(cStub, cost);
            }
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

            return TravelNodeRoute(path);
        }

        for (const auto& children : currentNode->children)// for each successor n' of n
        {
            childNode = static_cast<TravelNodeStub*>(children.first);
            g = currentNode->m_g + children.second; // stance from start + distance between the two nodes
            if ((childNode->open || childNode->close) && childNode->m_g <= g) // n' is already in opend or closed with a lower cost g(n')
                continue; // consider next successor

            h = childNode->dataNode->getDistance(goalStub->dataNode);
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

    return TravelNodeRoute();
}

TravelNodeRoute TravelNodeMap::getRoute(WorldPosition* startPos, WorldPosition* endPos, vector<WorldPosition>& startPath, Unit* bot)
{
    if (m_nodes.empty())
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

        TravelNodeRoute route = getRoute(startNode, endNode, bot);

        if (!route.isEmpty())
        {
            //Check if the bot can actually walk to this start position.
            startPath = startPos->getPathTo(*startNode->getPosition(), NULL);
            if (startNode->getPosition()->isPathTo(startPath, startNode->isTransport() ? 20.0f: sPlayerbotAIConfig.targetPosRecalcDistance))
                return route;
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
    for (auto& path : *startNode->getPaths())
    {

        
        TravelNode* endNode = path.first;

        string zoneName = startNode->getPosition()->getAreaName(true, true);
        for (auto & pos : path.second.getPath())
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
    std::unordered_map<TravelNode*, TravelNodePath> paths = *startNode->getPaths();

    if (paths.empty())
        return NULL;

    for (uint32 i = 0; i < 20; i++)
    {
        auto random_it = std::next(std::begin(paths), urand(0, paths.size() - 1));

        TravelNode* endNode = random_it->first;
        vector<WorldPosition> path = random_it->second.getPath();

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
        for (uint32 i = 0; i < (mapFull ? (uint32)20 : (uint32)1); i++)
        {
            vector<TravelNode*> rnodes = getNodes(&WorldPosition(bot));

            if (!rnodes.empty())
            {
                uint32 j = urand(0, rnodes.size() - 1);

                startNode = rnodes[j];
                newNode = NULL;

                bool nodeDone = false;

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
    if (!sPlayerbotAIConfig.hasLog("travelNodes.csv") && !sPlayerbotAIConfig.hasLog("travelPaths.csv"))
        return;

    printf("\r [Qgis] \r\x3D");
    fflush(stdout);

    sPlayerbotAIConfig.openLog("travelNodes.csv", "w");
    sPlayerbotAIConfig.openLog("travelPaths.csv", "w");

    vector<TravelNode*> anodes = getNodes();

    uint32 nr = 0;

    for (auto& node : anodes)
    {
        node->print(false);
    }
}

void TravelNodeMap::printNodeStore()
{
    string nodeStore = "TravelNodeStore.h";

    if (!sPlayerbotAIConfig.hasLog(nodeStore))
        return;

    printf("\r [Map] \r\x3D");
    fflush(stdout);

    sPlayerbotAIConfig.openLog(nodeStore, "w");

    std::unordered_map<TravelNode*, uint32> saveNodes;

    vector<TravelNode*> anodes = getNodes();

    sPlayerbotAIConfig.log(nodeStore, "#pragma once");
    sPlayerbotAIConfig.log(nodeStore, "#include \"TravelMgr.h\"");
    sPlayerbotAIConfig.log(nodeStore, "namespace ai");
    sPlayerbotAIConfig.log(nodeStore, "    {");
    sPlayerbotAIConfig.log(nodeStore, "    class TravelNodeStore");
    sPlayerbotAIConfig.log(nodeStore, "    {");
    sPlayerbotAIConfig.log(nodeStore, "    public:");
    sPlayerbotAIConfig.log(nodeStore, "    static void loadNodes()");
    sPlayerbotAIConfig.log(nodeStore, "    {");
    sPlayerbotAIConfig.log(nodeStore, "        TravelNode* nodes[%d];", anodes.size());

    for (uint32 i = 0; i < anodes.size(); i++)
    {
        TravelNode* node = anodes[i];

        ostringstream out;

        string name = node->getName();
        name.erase(remove(name.begin(), name.end(), '\"'), name.end());

        out << std::fixed << std::setprecision(2) << "        nodes[" << i << "] = sTravelNodeMap.addNode(&WorldPosition(" << node->getMapId() << "," << node->getX() << "f," << node->getY() << "f," << node->getZ() << "f,"<< node->getO() <<"f), \""
            << name << "\", " << (node->isImportant() ? "true" : "false") << ", true";
        if (node->isTransport())
            out << "," << (node->isTransport() ? "true" : "false") << "," << node->getTransportId();

        out << ");";
        sPlayerbotAIConfig.log(nodeStore, out.str().c_str());

        saveNodes.insert(make_pair(node, i));
    }

    for (uint32 i = 0; i < anodes.size(); i++)
    {
        TravelNode* node = anodes[i];

        for (auto& Link : *node->getLinks())
        {
            ostringstream out;
            out << std::fixed << std::setprecision(1) << "        nodes[" << i << "]->setPathTo(nodes[" << saveNodes.find(Link.first)->second << "],TravelNodePath(";
            out << Link.second->print() << "), true);";
            sPlayerbotAIConfig.log(nodeStore, out.str().c_str());
        }
    }

    sPlayerbotAIConfig.log(nodeStore, "	}");
    sPlayerbotAIConfig.log(nodeStore, "};");
    sPlayerbotAIConfig.log(nodeStore, "}");

    printf("\r [Done] \r\x3D");
    fflush(stdout);
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