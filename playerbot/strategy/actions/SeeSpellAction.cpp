#include "botpch.h"
#include "../../playerbot.h"
#include "SeeSpellAction.h"
#include "../values/Formations.h"
#include "../../ServerFacade.h"
#include "MovementGenerator.h"
#ifdef MANGOS
#include "luaEngine.h"
#endif

#include "MotionGenerators/PathFinder.h"



using namespace ai;

void CreateWp(Player* wpOwner,float x, float y, float z, float o, uint32 entry)
{
    float dist = wpOwner->GetDistance(x, y, z);
    float delay = 1000.0f * dist / wpOwner->GetSpeed(MOVE_RUN) + sPlayerbotAIConfig.reactDelay;
    Creature* wpCreature = wpOwner->SummonCreature(entry, x, y, z, o, TEMPSPAWN_TIMED_DESPAWN, delay*1.2);
}

float GetAngle(const float x1, const float y1, const float x2, const float y2)
{
    float dx = x1 - x2;
    float dy = y1 - y2;

    float ang = atan2(dy, dx);                              // returns value between -Pi..Pi
    ang = (ang >= 0) ? ang : 2 * M_PI_F + ang;
    return ang;
}

bool SeeSpellAction::Execute(Event event)
{
    WorldPacket p(event.getPacket()); // 
    uint32 spellId;

    p.rpos(0);
    p >> spellId;

    //ai->TellMaster(to_string(spellId));

    if (spellId != 30758)
        return false;

    SpellCastTargets targets;

#ifdef BUILD_PLAYERBOT
    recvPacket >> targets.ReadForCaster(mover);
#else
    p >> targets.ReadForCaster(ai->GetMaster());
#endif

    Position spellPosition = targets.m_destPos;

    if (bot->GetDistance(spellPosition.GetPositionX(), spellPosition.GetPositionY(), spellPosition.GetPositionZ()) <= 10)
    {
        ai->TellMaster("In range!");
    }
    else
    {
        ai->TellMaster("Out of range!");
    }

    Player* master = ai->GetMaster();

    float x = spellPosition.GetPositionX();
    float y = spellPosition.GetPositionY();
    float z = spellPosition.GetPositionZ();

    Formation* formation = AI_VALUE(Formation*, "formation");
    WorldLocation formationLocation = formation->GetLocation();
    if (formationLocation.coord_x != 0 || formationLocation.coord_y != 0)
    {
        x = x - master->GetPositionX() + formationLocation.coord_x;
        y = y - master->GetPositionY() + formationLocation.coord_y;
        z = z - master->GetPositionZ() + formationLocation.coord_z;
    }

    PathFinder path(bot);

    path.calculate(x, y, z, false);

    Vector3 end = path.getEndPosition();
    Vector3 aend = path.getActualEndPosition();

    PointsArray& points = path.getPath();
    PathType type = path.getPathType();

    ostringstream out;

    out << "current path is: ";

    out << type;

    out << " of length ";

    out << points.size();

    out << " with offset ";

    out << (end - aend).length();

    //std::reverse(points.begin(), points.end());

    /*
    for (auto i : points)
    {
        if (bot->IsWithinLOS(i.x, i.y, i.z))
        {
            CreateWp(bot, i.x, i.y, i.z, GetAngle(x, y, i.x, i.y), 1);
        }
        else
            CreateWp(bot, i.x, i.y, i.z, GetAngle(x, y, i.x, i.y), 11144);

        x = i.x;
        y = i.y;
        z = i.z;
    }
    */
  
    ai->TellMaster(out);

    if (bot->IsWithinLOS(x, y, z))
        return MoveNear(bot->GetMapId(), x, y, z, 0);
    else
        return MoveTo(bot->GetMapId(), x, y, z, false, false);
}