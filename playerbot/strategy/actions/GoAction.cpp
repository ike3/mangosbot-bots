#include "botpch.h"
#include "../../playerbot.h"
#include "GoAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"
#include "../values/Formations.h"
#include "../values/PositionValue.h"
#include "travelMgr.h"
#include "MotionGenerators/PathFinder.h"

using namespace ai;

vector<string> split(const string &s, char delim);
char *strstri(const char *haystack, const char *needle);

bool GoAction::Execute(Event event)
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

    if (param.find("travel") != string::npos)
    {
        WorldPosition* botPos = &WorldPosition(bot);

        vector<TravelDestination*> dests;

        for (auto& d : sTravelMgr.getExploreTravelDestinations(bot, true, true))
        {
            if (strstri(d->getTitle().c_str(), param.substr(7).c_str()))
                dests.push_back(d);
        }

        for (auto& d : sTravelMgr.getRpgTravelDestinations(bot, true, true))
        {
            if (strstri(d->getTitle().c_str(), param.substr(7).c_str()))
                dests.push_back(d);
        }

        TravelTarget* target = context->GetValue<TravelTarget*>("travel target")->Get();

        if (!dests.empty())
        {
            TravelDestination* dest = *std::min_element(dests.begin(), dests.end(), [botPos](TravelDestination* i, TravelDestination* j) {return i->distanceTo(botPos) < j->distanceTo(botPos); });

            target->setTarget(dest, dest->nextPoint(botPos, true).front());
            target->setForced(true);

            ostringstream out; out << "Traveling to " << dest->getTitle();
            ai->TellMasterNoFacing(out.str());

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

        if (ai->HasStrategy("debug move", BOT_STATE_NON_COMBAT))
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
                CreateWp(bot, i.x, i.y, i.z, GetAngle(x, y, i.x, i.y), 11144);
            }

            ai->TellMaster(out);            
        }

        if (bot->IsWithinLOS(x, y, z))
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
            ai->TellMaster("It is too far away");
            return false;
        }

        const TerrainInfo* terrain = map->GetTerrain();
        if (terrain->IsUnderWater(x, y, z) || terrain->IsInWater(x, y, z))
        {
            ai->TellError("It is under water");
            return false;
        }

#ifdef MANGOSBOT_TWO
        float ground = map->GetHeight(bot->GetPhaseMask(), x, y, z + 0.5f);
#else
        float ground = map->GetHeight(x, y, z + 0.5f);
#endif
        if (ground <= INVALID_HEIGHT)
        {
            ai->TellError("I can't go there");
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
            ai->TellError("It is too far away");
            return false;
        }

        ostringstream out; out << "Moving to position " << param;
        ai->TellMasterNoFacing(out.str());
        return MoveNear(bot->GetMapId(), pos.x, pos.y, pos.z + 0.5f, sPlayerbotAIConfig.followDistance);
    }

    ai->TellMaster("Whisper 'go x,y', 'go [game object]', 'go unit' or 'go position' and I will go there");
    return false;
}
