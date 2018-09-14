#include "botpch.h"
#include "../../playerbot.h"
#include "StayActions.h"

#include "../../ServerFacade.h"
#include "../values/LastMovementValue.h"

using namespace ai;

bool StayActionBase::Stay()
{
    AI_VALUE(LastMovement&, "last movement").Set(NULL);

    if (!urand(0, 5000)) ai->PlaySound(TEXTEMOTE_YAWN);

    MotionMaster &mm = *bot->GetMotionMaster();
    if (mm.GetCurrentMovementGeneratorType() == FLIGHT_MOTION_TYPE || bot->IsFlying())
        return false;

    if (!sServerFacade.isMoving(bot))
        return false;

    mm.Clear();
    bot->InterruptMoving();
	bot->clearUnitState(UNIT_STAT_CHASE);
	bot->clearUnitState(UNIT_STAT_FOLLOW);

    if (!bot->IsStandState())
        bot->SetStandState(UNIT_STAND_STATE_STAND);
    return true;
}

bool StayAction::Execute(Event event)
{
    return Stay();
}

bool StayAction::isUseful()
{
    return !AI_VALUE2(bool, "moving", "self target");
}
