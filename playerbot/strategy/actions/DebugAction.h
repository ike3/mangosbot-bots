#pragma once

#include "../Action.h"
#include "InventoryAction.h"

namespace ai
{
    class DebugAction : public Action
    {
    public:
        DebugAction(PlayerbotAI* ai) : Action(ai, "Debug") {}

        virtual bool Execute(Event event);

        void FakeSpell(uint32 spellId, Unit* truecaster, Unit* caster, ObjectGuid target = ObjectGuid(), list<ObjectGuid> otherTargets = {}, list<ObjectGuid> missTargets = {}, WorldPosition source = WorldPosition(), WorldPosition dest = WorldPosition(), bool forceDest = false);
        void addAura(uint32 spellId, Unit* target);
    };

}
