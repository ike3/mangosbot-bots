#pragma once

#include "../Action.h"
#include "MovementActions.h"

namespace ai
{
    class SeeSpellAction : public MovementAction
    {
    public:
        SeeSpellAction(PlayerbotAI* ai) : MovementAction(ai, "see spell") {}
        virtual bool Execute(Event event);
    private:
    };
}