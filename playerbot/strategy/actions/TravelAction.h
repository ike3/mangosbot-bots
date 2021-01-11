#pragma once

#include "../Action.h"
#include "MovementActions.h"
#include "../values/LastMovementValue.h"
#include "../../TravelMgr.h"

namespace ai
{
    class TravelAction : public MovementAction {
    public:
        TravelAction(PlayerbotAI* ai) : MovementAction(ai, "travel") {}

        virtual bool Execute(Event event);
        virtual bool isUseful();
    };

}
