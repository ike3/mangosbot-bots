#include "botpch.h"
#include "../../playerbot.h"
#include "MoveToTravelTargetAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"
#include "../../LootObjectStack.h"
#include "MotionGenerators/PathFinder.h"
#include "ChooseRpgTargetAction.h"

using namespace ai;

bool MoveToTravelTargetAction::Execute(Event event)
{

    TravelTarget* target = AI_VALUE(TravelTarget*, "travel target");

    WorldPosition botLocation(bot);

    float maxDistance = target->getDestination()->getRadiusMin();

    //Evenly distribute around the target.
    float angle = 2 * M_PI * urand(0, 100) / 100.0;

    WorldLocation location = target->getLocation();

    if (target->getMaxTravelTime() > target->getTimeLeft()) //The bot is late. Speed it up.
    {
        //distance = sPlayerbotAIConfig.fleeDistance;
        //angle = bot->GetAngle(location.coord_x, location.coord_y);
        //location = botLocation.getLocation();
    }

    float x = location.coord_x;
    float y = location.coord_y;
    float z = location.coord_z;
    float mapId = location.mapid;

    //Move between 0.5 and 1.0 times the maxDistance.
    float mod = urand(50, 100)/100.0;   

    x += cos(angle) * maxDistance * mod;
    y += sin(angle) * maxDistance * mod;

    bool canMove = false;

    if (bot->IsWithinLOS(x, y, z))
        canMove = MoveNear(mapId, x, y, z, 0);
    else
        canMove = MoveTo(mapId, x, y, z, false, false);

    if (!canMove && !target->isForced())
    {
        target->incRetry(true);

        if (target->isMaxRetry(true))
            target->setStatus(TRAVEL_STATUS_COOLDOWN);
    }
    else
        target->setRetry(true);
     
    return canMove;
}

bool MoveToTravelTargetAction::isUseful()
{
    if (!context->GetValue<TravelTarget*>("travel target")->Get()->isTraveling())
        return false;

    if (bot->IsTaxiFlying())
        return false;

    if (bot->IsFlying())
        return false;

    if (bot->IsMoving())
        return false;

    if (bot->IsInCombat())
        return false;

    if (AI_VALUE2(uint8, "health", "self target") <= sPlayerbotAIConfig.almostFullHealth)
        return false;
     
    if (AI_VALUE2(uint8, "mana", "self target") && AI_VALUE2(uint8, "mana", "self target") <= sPlayerbotAIConfig.mediumMana) 
        return false;

    LootObject loot = AI_VALUE(LootObject, "loot target");
    if (loot.IsLootPossible(bot))
        return false;

    if (!ChooseRpgTargetAction::isFollowValid(bot, context->GetValue<TravelTarget*>("travel target")->Get()->getLocation()))
        return false;

    return true;
}

