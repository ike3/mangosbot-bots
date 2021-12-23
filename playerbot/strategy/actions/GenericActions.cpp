#include "botpch.h"
#include "../../playerbot.h"
#include "GenericActions.h"

using namespace ai;

bool MeleeAction::isUseful()
{
    // do not allow if can't attack from vehicle
    if (ai->IsInVehicle() && !ai->IsInVehicle(false, false, true))
        return false;

    return true;
}
