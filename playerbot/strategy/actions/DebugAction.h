#pragma once
#include "../../TravelMgr.h"
#include "GenericActions.h"

namespace ai
{
    class DebugAction : public ChatCommandAction
    {
    public:
        DebugAction(PlayerbotAI* ai) : ChatCommandAction(ai, "Debug") {}
        virtual bool Execute(Event& event) override;
        void FakeSpell(uint32 spellId, Unit* truecaster, Unit* caster, ObjectGuid target = ObjectGuid(), list<ObjectGuid> otherTargets = {}, list<ObjectGuid> missTargets = {}, WorldPosition source = WorldPosition(), WorldPosition dest = WorldPosition(), bool forceDest = false);
        void addAura(uint32 spellId, Unit* target);
    };
}
