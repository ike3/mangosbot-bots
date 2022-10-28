#pragma once

#include "../Action.h"
#include "MovementActions.h"
#include "../values/LastMovementValue.h"
#include "../../TravelMgr.h"

namespace ai
{
    class MoveToTravelTargetAction : public MovementAction {
    public:
        MoveToTravelTargetAction(PlayerbotAI* ai) : MovementAction(ai, "move to travel target") {}

        virtual bool Execute(Event& event);
        virtual bool isUseful();
    };

}
