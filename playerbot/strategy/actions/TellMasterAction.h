#pragma once

#include "../Action.h"

namespace ai
{
    class TellMasterAction : public Action {
    public:
        TellMasterAction(PlayerbotAI* ai, string text) : Action(ai, "tell master"), text(text) {}

        virtual bool Execute(Event event)
        {
            ai->TellMaster(text);
            return true;
        }

    private:
        string text;
    };

    class OutOfReactRangeAction : public MovementAction {
    public:
        OutOfReactRangeAction(PlayerbotAI* ai) : MovementAction(ai, "tell out of react range") {}

        virtual bool Execute(Event event)
        {
            ai->TellMaster(BOT_TEXT("wait_travel_close"));
            return true;
        }

        virtual bool isUseful() 
        {
            bool canFollow = Follow(AI_VALUE(Unit*, "master target"));
            if (!canFollow)
                return false;

            return true;
        }
    };
}
