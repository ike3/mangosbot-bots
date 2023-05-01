#pragma once
#include "../Value.h"
#include "PartyMemberValue.h"
#include "../../PlayerbotAIConfig.h"

namespace ai
{
    class PartyMemberWithoutAuraValue : public PartyMemberValue, public Qualified
	{
	public:
        PartyMemberWithoutAuraValue(PlayerbotAI* ai, string name = "party member without aura", float range = sPlayerbotAIConfig.sightDistance) :
          PartyMemberValue(ai, name) {}

    protected:
        virtual Unit* Calculate();
	};

    class PartyMemberWithoutMyAuraValue : public PartyMemberValue, public Qualified
    {
    public:
        PartyMemberWithoutMyAuraValue(PlayerbotAI* ai, string name = "party member without my aura", float range = 30.f) :
            PartyMemberValue(ai, name) {}

    protected:
        virtual Unit* Calculate();
    };

    class PartyTankWithoutAuraValue : public PartyMemberValue, public Qualified
    {
    public:
        PartyTankWithoutAuraValue(PlayerbotAI* ai, string name = "party tank without aura", float range = sPlayerbotAIConfig.sightDistance) :
            PartyMemberValue(ai, name) {}

    protected:
        virtual Unit* Calculate();
    };
}
