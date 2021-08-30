#pragma once

#include "../Action.h"

namespace ai
{
    class ResetInstancesAction : public Action {
    public:
        ResetInstancesAction(PlayerbotAI* ai) : Action(ai, "reset instances") {}
        virtual bool Execute(Event event);
        virtual bool isUseful() { return ai->GetGroupMaster() == bot; };
    };

}
