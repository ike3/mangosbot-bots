#include "botpch.h"
#include "../../playerbot.h"
#include "MoveToTravelTargetAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"
#include "../../LootObjectStack.h"
#include "MotionGenerators/PathFinder.h"

using namespace ai;

bool MoveToTravelTargetAction::Execute(Event event)
{
    TravelTarget* target = AI_VALUE(TravelTarget*, "travel target");

    WorldPosition botLocation(bot);

    float distance = AI_VALUE2(float, "distance", "travel target");
    float angle = 2 * M_PI * urand(0, 100) / 100.0;

    WorldLocation location = target->getLocation();

    if (target->getMaxTravelTime() > target->getTimeLeft()) //The bot is late. Speed it up.
    {
        location = botLocation.getLocation();
        distance = sPlayerbotAIConfig.tooCloseDistance;
        angle = bot->GetAngle(location.coord_x, location.coord_y);
    }

    float x = location.coord_x;
    float y = location.coord_y;
    float z = location.coord_z;
    float mapId = location.mapid;

    float mod = urand(50, 100)/50.0;

    /*
    PathFinder path(bot);
    path.calculate(x, y, z, false);
    PathType type = path.getPathType();
    PointsArray& points = path.getPath();
    
    if (ai->HasStrategy("debug", BOT_STATE_NON_COMBAT))
    for (auto i : points)
    {
        CreateWp(bot, i.x, i.y, i.z, GetAngle(x, y, i.x, i.y), 15631);

        x = i.x;
        y = i.y;
        z = i.z;
    }
    */

    x += cos(angle) * sPlayerbotAIConfig.tooCloseDistance * mod;
    y += sin(angle) * sPlayerbotAIConfig.tooCloseDistance * mod;

    if (bot->IsWithinLOS(x, y, z))
        return MoveNear(mapId, x, y, z, 0);
    else
        return MoveTo(mapId, x, y, z, false, false);

    /*
    if (distance < 80.0f)
    {
        if (bot->IsWithinLOS(x, y, z))  
        {
            if (ai->HasStrategy("debug", BOT_STATE_NON_COMBAT))
                ai->TellMaster("Los Traveling " + to_string(distance));

           return MoveNear(mapId, x , y , z, 0);
        }
    }

    if (type == PATHFIND_NOPATH)
    {
        sTravelMgr.setNullTravelTarget(bot);
        return false;
    }

    MotionMaster& mm = *bot->GetMotionMaster();

    //WaitForReach(distance);
    
    if (bot->IsSitState())
        bot->SetStandState(UNIT_STAND_STATE_STAND);

    if (bot->IsNonMeleeSpellCasted(true))
    {
        bot->CastStop();
        ai->InterruptSpell();
    }

    bool generatePath = !bot->IsFlying() && !sServerFacade.IsUnderwater(bot);

#ifdef MANGOS
    mm.MovePoint(mapId, x, y, z, generatePath);
#endif
#ifdef CMANGOS
    if (ai->HasStrategy("debug", BOT_STATE_NON_COMBAT))
    {
        ai->TellMaster("Point Traveling " + to_string(distance));

        ostringstream out;
        out << "From: " << bot->GetPositionX() << " | " << bot->GetPositionY() << " | " << bot->GetPositionZ();
        out << " to: " << x << " | " << y << " | " << z;
        ai->TellMaster(out);
    }
    bot->StopMoving();
    mm.Clear();
    mm.MovePoint(mapId, x, y, z, FORCED_MOVEMENT_RUN, generatePath);
#endif
    */

    AI_VALUE(LastMovement&, "last movement").Set(x, y, z, bot->GetOrientation());
    return true;
}

bool MoveToTravelTargetAction::isUseful()
{
    return context->GetValue<TravelTarget*>("travel target")->Get()->isTraveling() 
        && !bot->IsTaxiFlying()
        && !bot->IsFlying() 
        && !bot->IsMoving() 
        && !bot->IsInCombat();
}

