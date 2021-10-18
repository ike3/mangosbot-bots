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
}
