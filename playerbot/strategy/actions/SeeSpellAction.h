#pragma once

#include "../Action.h"
#include "MovementActions.h"
#include "../values/LastMovementValue.h"

namespace ai
{
    class SeeSpellAction : public MovementAction
    {
    public:
        SeeSpellAction(PlayerbotAI* ai) : MovementAction(ai, "see spell") {}
        virtual bool Execute(Event event);
    private:
        Creature* CreateWps(Player* wpOwner, float x, float y, float z, float o, uint32 entry, Creature* lastWp, bool important = false);
    };
}