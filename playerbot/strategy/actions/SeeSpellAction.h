#pragma once

#include "../Action.h"
#include "MovementActions.h"
#include "../values/LastMovementValue.h"
#include "../values/RTSCValues.h"

namespace ai
{
    class SeeSpellAction : public MovementAction
    {
    public:
        SeeSpellAction(PlayerbotAI* ai, string name = "see spell") : MovementAction(ai, name) {}
        virtual bool Execute(Event event);

        bool SelectSpell(WorldPosition& spellPosition);
        bool MoveSpell(WorldPosition& spellPosition);

        virtual bool MoveToSpell(WorldPosition& spellPosition, bool inFormation = true);
        void SetFormationOffset(WorldPosition& spellPosition);
    private:
        Creature* CreateWps(Player* wpOwner, float x, float y, float z, float o, uint32 entry, Creature* lastWp, bool important = false);
    };
}