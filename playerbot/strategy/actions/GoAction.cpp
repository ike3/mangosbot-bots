#include "botpch.h"
#include "../../playerbot.h"
#include "GoAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"
#include "../values/Formations.h"
#include "../values/PositionValue.h"
#include "TravelMgr.h"
#include "MotionGenerators/PathFinder.h"
#include "ChooseTravelTargetAction.h"

using namespace ai;

vector<string> split(const string& s, char delim);
char* strstri(const char* haystack, const char* needle);

bool GoAction::Execute(Event& event)
{
    Player* master = GetMaster();
    if (!master)
        return false;

    string param = event.getParam();
    if (param == "?")
    {
        float x = bot->GetPositionX();
        float y = bot->GetPositionY();
        Map2ZoneCoordinates(x, y, bot->GetZoneId());
        ostringstream out;
        out << "I am at " << x << "," << y;
        ai->TellMaster(out.str());
        return true;
    }

    if (param.find("where") != string::npos)
    {
        ChooseTravelTargetAction* travelAction = new ChooseTravelTargetAction(ai);

        TravelTarget* target = context->GetValue<TravelTarget*>("travel target")->Get();

        travelAction->getNewTarget(target, target);

        if (!target->getDestination() || target->getDestination()->getTitle().empty())
        {
            ai->TellMasterNoFacing("I have no place I want to go to");
            return false;
        }

        string title = target->getDestination()->getTitle();

        if (title.find('[') != string::npos)
            title = title.substr(title.find("[") + 1, title.find("]") - title.find("[")-1);


        TravelDestination* dest = ChooseTravelTargetAction::FindDestination(bot,title);

        if (!dest)
            dest = target->getDestination();

        if (!dest)
        {
            ai->TellMasterNoFacing("I have no place I want to go to");
            return false;
        }

        string link = ChatHelper::formatValue("command", "go to " + title,title, "FFFFFFFF");

        ostringstream out; out << "I would like to travel to " << link;
        ai->TellMasterNoFacing(out.str());

        delete travelAction;
        return true;
    }
    if (param.find("to") != string::npos && param.size() > 3)
    {
        WorldPosition botPos = WorldPosition(bot);

        string destination = param.substr(3);

        TravelTarget* target = context->GetValue<TravelTarget*>("travel target")->Get();

        TravelDestination* dest = ChooseTravelTargetAction::FindDestination(bot, destination);
        if (dest)
        {
            WorldPosition* point = dest->nearestPoint(botPos);

            if (!point)
                return false;

            if (ai->HasStrategy("follow", BotState::BOT_STATE_NON_COMBAT))
            {
                vector<WorldPosition> beginPath, endPath;
                TravelNodeRoute route = sTravelNodeMap.getRoute(botPos, *point, beginPath, bot);

                WorldPosition poi = *point;
                float pointAngle = botPos.getAngleTo(poi);

                if (botPos.distance(poi) > 60)
                {
                    TravelNode* nearNode = nullptr;
                    TravelNode* nextNode = nullptr;

                    if (route.getNodes().size() > 1)
                    {
                        nearNode = route.getNodes().front();

                        for (auto node : route.getNodes())
                        {
                            if (node->getPosition()->getAreaName(true, true) != botPos.getAreaName(true,true) && node != nearNode)
                            {
                                nextNode = node;
                                break;
                            }
                        }
                        if(nearNode == nextNode)
                            for (auto node : route.getNodes())
                            {
                                if (node->getPosition()->distance(botPos) > 150 && node != nearNode)
                                {
                                    nextNode = node;
                                    break;
                                }
                            }
                    }
                    else
                        nextNode = route.getNodes().front();

                    if (nearNode)
                        ai->TellMasterNoFacing("We are now near " + nearNode->getName() + ".");
                    
                    ai->TellMasterNoFacing("if we want to travel to " + dest->getTitle());
                    if (nextNode->getPosition()->getAreaName(true, true) != botPos.getAreaName(true, true))
                        ai->TellMasterNoFacing("we should head to " + nextNode->getName() + " in " + nextNode->getPosition()->getAreaName(true, true));
                    else
                        ai->TellMasterNoFacing("we should head to " + nextNode->getName());

                    poi = *nextNode->getPosition();
                    pointAngle = botPos.getAngleTo(poi);

                }
                else
                    ai->TellMasterNoFacing("We are near " + dest->getTitle());

                ai->TellMaster("it is " + to_string(uint32(round(poi.distance(botPos)))) + " yards to the " + ChatHelper::formatAngle(pointAngle));
                sServerFacade.SetFacingTo(bot, pointAngle, true);
                bot->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                ai->Poi(poi.getX(), poi.getY(), "this way", master);

                return true;
            }
            else
            {
                target->setTarget(dest, point);
                target->setForced(true);

                ostringstream out; out << "Traveling to " << dest->getTitle();
                ai->TellMasterNoFacing(out.str());

                if (!ai->HasStrategy("travel", BotState::BOT_STATE_NON_COMBAT))
                    ai->ChangeStrategy("+travel once", BotState::BOT_STATE_NON_COMBAT);
            }
            return true;
        }
        else
        {
            ai->TellMasterNoFacing("I don't know how to travel there to " + destination);
            return true;
        }

        return true;
    }
    if (param.find("travel") != string::npos && param.size()> 7)
    {
        WorldPosition pos = WorldPosition(bot);
        WorldPosition* botPos = &pos;

        string destination = param.substr(7);

        TravelTarget* target = context->GetValue<TravelTarget*>("travel target")->Get();

        TravelDestination* dest = ChooseTravelTargetAction::FindDestination(bot, destination);
        if(dest)
        {
            vector <WorldPosition*> points = dest->nextPoint(botPos, true);

            if (points.empty())
                return false;

            target->setTarget(dest, points.front());
            target->setForced(true);

            ostringstream out; out << "Traveling to " << dest->getTitle();
            ai->TellMasterNoFacing(out.str());

            if(!ai->HasStrategy("travel", BotState::BOT_STATE_NON_COMBAT))
                ai->ChangeStrategy("+travel once", BotState::BOT_STATE_NON_COMBAT);

            return true;
        }
        else
        {
            ai->TellMasterNoFacing("Clearing travel target");
            target->setTarget(sTravelMgr.nullTravelDestination, sTravelMgr.nullWorldPosition);
            target->setForced(false);
            return true;
        }
    }

    list<ObjectGuid> gos = ChatHelper::parseGameobjects(param);
    if (!gos.empty())
    {
        for (list<ObjectGuid>::iterator i = gos.begin(); i != gos.end(); ++i)
        {
            GameObject* go = ai->GetGameObject(*i);
            if (go && sServerFacade.isSpawned(go))
            {
                if (sServerFacade.IsDistanceGreaterThan(sServerFacade.GetDistance2d(bot, go), sPlayerbotAIConfig.reactDistance))
                {
                    ai->TellError("It is too far away");
                    return false;
                }

                ostringstream out; out << "Moving to " << ChatHelper::formatGameobject(go);
                ai->TellMasterNoFacing(out.str());
                return MoveNear(bot->GetMapId(), go->GetPositionX(), go->GetPositionY(), go->GetPositionZ() + 0.5f, sPlayerbotAIConfig.followDistance);
            }
        }
        return false;
    }

    list<ObjectGuid> units;
    list<ObjectGuid> npcs = AI_VALUE(list<ObjectGuid>, "nearest npcs");
    units.insert(units.end(), npcs.begin(), npcs.end());
    list<ObjectGuid> players = AI_VALUE(list<ObjectGuid>, "nearest friendly players");
    units.insert(units.end(), players.begin(), players.end());
    for (list<ObjectGuid>::iterator i = units.begin(); i != units.end(); i++)
    {
        Unit* unit = ai->GetUnit(*i);
        if (unit && strstri(unit->GetName(), param.c_str()))
        {
            ostringstream out; out << "Moving to " << unit->GetName();
            ai->TellMasterNoFacing(out.str());
            return MoveNear(bot->GetMapId(), unit->GetPositionX(), unit->GetPositionY(), unit->GetPositionZ() + 0.5f, sPlayerbotAIConfig.followDistance);
        }
    }

    if (param.find(";") != string::npos)
    {
        vector<string> coords = split(param, ';');
        float x = atof(coords[0].c_str());
        float y = atof(coords[1].c_str());
        float z;
        if (coords.size() > 2)
            z = atof(coords[2].c_str());
        else
            z = bot->GetPositionZ();

        if (ai->HasStrategy("debug move", BotState::BOT_STATE_NON_COMBAT))
        {
            
            PathFinder path(bot);

            path.calculate(x, y, z, false);

            Vector3 end = path.getEndPosition();
            Vector3 aend = path.getActualEndPosition();

            PointsArray& points = path.getPath();
            PathType type = path.getPathType();

            ostringstream out;

            out << x << ";" << y << ";" << z << " =";

            out << "path is: ";

            out << type;

            out << " of length ";

            out << points.size();

            out << " with offset ";

            out << (end - aend).length();


            for (auto i : points)
            {
                CreateWp(bot, i.x, i.y, i.z, 0.0, 11144);
            }

            ai->TellMaster(out);            
        }

        if (bot->IsWithinLOS(x, y, z, true))
            return MoveNear(bot->GetMapId(), x, y, z, 0);
        else
            return MoveTo(bot->GetMapId(), x, y, z, false, false);    

        return true;
    }


    if (param.find(",") != string::npos)
    {
        vector<string> coords = split(param, ',');
        float x = atof(coords[0].c_str());
        float y = atof(coords[1].c_str());
        Zone2MapCoordinates(x, y, bot->GetZoneId());

        Map* map = bot->GetMap();
        float z = bot->GetPositionZ();
        bot->UpdateAllowedPositionZ(x, y, z);

        if (sServerFacade.IsDistanceGreaterThan(sServerFacade.GetDistance2d(bot, x, y), sPlayerbotAIConfig.reactDistance))
        {
            ai->TellMaster(BOT_TEXT("error_far"));
            return false;
        }

        const TerrainInfo* terrain = map->GetTerrain();
        if (terrain->IsUnderWater(x, y, z) || terrain->IsInWater(x, y, z))
        {
            ai->TellError(BOT_TEXT("error_water"));
            return false;
        }

#ifdef MANGOSBOT_TWO
        float ground = map->GetHeight(bot->GetPhaseMask(), x, y, z + 0.5f);
#else
        float ground = map->GetHeight(x, y, z + 0.5f);
#endif
        if (ground <= INVALID_HEIGHT)
        {
            ai->TellError(BOT_TEXT("error_cant_go"));
            return false;
        }

        float x1 = x, y1 = y;
        Map2ZoneCoordinates(x1, y1, bot->GetZoneId());
        ostringstream out; out << "Moving to " << x1 << "," << y1;
        ai->TellMasterNoFacing(out.str());
        return MoveNear(bot->GetMapId(), x, y, z + 0.5f, sPlayerbotAIConfig.followDistance);
    }

    ai::PositionEntry pos = context->GetValue<ai::PositionMap&>("position")->Get()[param];
    if (pos.isSet())
    {
        if (sServerFacade.IsDistanceGreaterThan(sServerFacade.GetDistance2d(bot, pos.x, pos.y), sPlayerbotAIConfig.reactDistance))
        {
            ai->TellError(BOT_TEXT("error_far"));
            return false;
        }

        ostringstream out; out << "Moving to position " << param;
        ai->TellMasterNoFacing(out.str());
        return MoveNear(bot->GetMapId(), pos.x, pos.y, pos.z + 0.5f, sPlayerbotAIConfig.followDistance);
    }

    ai->TellMaster("Whisper 'go x,y', 'go [game object]', 'go unit' or 'go position' and I will go there");
    return false;
}
