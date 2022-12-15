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

bool ChatCommandAction::Execute(Event& event)
{
    if (ExecuteCommand(event))
    {
        SetDuration(getDuration());
        return true;
    }

    SetDuration(0U);
    return false;
}
