#include "botpch.h"
#include "../../playerbot.h"
#include "MoveToTravelTargetAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"
#include "../../LootObjectStack.h"

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

    float mod = urand(1, 100)/50.0;

    if (distance < 80.0f)
        if (bot->IsWithinLOS(x, y, z)) return MoveNear(mapId, x + cos(angle) * sPlayerbotAIConfig.tooCloseDistance * mod, y + sin(angle) * sPlayerbotAIConfig.tooCloseDistance * mod, z, 0);

    WaitForReach(distance);

    if (bot->IsSitState())
        bot->SetStandState(UNIT_STAND_STATE_STAND);

    if (bot->IsNonMeleeSpellCasted(true))
    {
        bot->CastStop();
        ai->InterruptSpell();
    }

    bool generatePath = !bot->IsFlying() && !sServerFacade.IsUnderwater(bot);
    MotionMaster &mm = *bot->GetMotionMaster();
#ifdef MANGOS
    mm.MovePoint(mapId, x, y, z, generatePath);
#endif
#ifdef CMANGOS
    mm.MovePoint(mapId, x, y, z, FORCED_MOVEMENT_RUN, generatePath);
#endif

    AI_VALUE(LastMovement&, "last movement").Set(x, y, z, bot->GetOrientation());
    return true;
}

bool MoveToTravelTargetAction::isUseful()
{
    return context->GetValue<TravelTarget*>("travel target")->Get()->isTraveling() && !bot->IsTaxiFlying() && !bot->IsFlying();
}

