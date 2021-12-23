#pragma once

#include "../Action.h"
#include "MovementActions.h"
#include "AiFactory.h"
#include "PlayerbotAIConfig.h"

namespace ai
{
    class EnterVehicleAction : public MovementAction
    {
    public:
        EnterVehicleAction(PlayerbotAI* ai, string name = "enter vehicle") : MovementAction(ai, name) {}
        virtual bool Execute(Event event);
        //virtual bool isUseful();
    };

    class LeaveVehicleAction : public MovementAction
    {
    public:
        LeaveVehicleAction(PlayerbotAI* ai, string name = "leave vehicle") : MovementAction(ai, name) {}
        virtual bool Execute(Event event);
        //virtual bool isUseful();
    };
}
