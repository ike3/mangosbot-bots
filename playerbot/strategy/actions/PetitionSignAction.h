#pragma once

#include "../Action.h"

namespace ai
{
    class PetitionSignAction : public Action {
    public:
        PetitionSignAction(PlayerbotAI* ai) : Action(ai, "petition sign") {}
        virtual bool Execute(Event event);
    };    
}
