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

    x += cos(angle) * sPlayerbotAIConfig.tooCloseDistance * mod;
    y += sin(angle) * sPlayerbotAIConfig.tooCloseDistance * mod;

    if (bot->IsWithinLOS(x, y, z))
        return MoveNear(mapId, x, y, z, 0);
    else
        return MoveTo(mapId, x, y, z, false, false);
 
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

