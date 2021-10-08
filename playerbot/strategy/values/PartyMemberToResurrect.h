#pragma once
#include "../Value.h"
#include "PartyMemberValue.h"

namespace ai
{
    class PartyMemberToResurrect : public PartyMemberValue
	{
	public:
        PartyMemberToResurrect(PlayerbotAI* ai, string name = "party member to resurrect") :
          PartyMemberValue(ai,name) {}
    
    protected:
        virtual Unit* Calculate();
	};
}
