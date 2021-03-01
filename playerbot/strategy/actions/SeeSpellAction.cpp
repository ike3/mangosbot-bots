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
        x = x - master->GetPositionX() + formationLocation.coord_x ;
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

    ai->TellMaster(out);    

    if (type == PATHFIND_NOPATH)
        return false;

    MotionMaster& mm = *bot->GetMotionMaster();
    bot->StopMoving();
    mm.Clear();

    //if (bot->IsWithinLOS(x, y, z)) return MoveNear(bot->GetMapId(), x, y, z);

    if (bot->IsSitState())
        bot->SetStandState(UNIT_STAND_STATE_STAND);

    if (bot->IsNonMeleeSpellCasted(true))
    {
        bot->CastStop();
        ai->InterruptSpell();
    }

    bool generatePath = !bot->IsFlying() && !sServerFacade.IsUnderwater(bot);

#ifdef MANGOS
    mm.MovePoint(bot->GetMapId(), x, y, z, generatePath);
#endif
#ifdef CMANGOS
    mm.MovePoint(bot->GetMapId(), x,y, z, FORCED_MOVEMENT_RUN, generatePath);
#endif

    AI_VALUE(LastMovement&, "last movement").Set(x, y, z, bot->GetOrientation());
    return true;
}