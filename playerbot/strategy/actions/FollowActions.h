#pragma once

#include "../Action.h"
#include "MovementActions.h"

namespace ai
{
	class FollowAction : public MovementAction {
	public:
		FollowAction(PlayerbotAI* ai) : MovementAction(ai, "follow") {}
		virtual bool Execute(Event event);
        virtual bool isUseful();
	};

    class FleeToMasterAction : public MovementAction {
    public:
        FleeToMasterAction(PlayerbotAI* ai) : MovementAction(ai, "flee to master") {}

        virtual bool Execute(Event event)
        {
            bool canFollow = Follow(AI_VALUE(Unit*, "master target"));
            if (!canFollow)
            {
                //ai->SetNextCheckDelay(5000);
                return false;
            }

            ai->TellMaster("Wait for me!");
            return true;
        }
    };
}
