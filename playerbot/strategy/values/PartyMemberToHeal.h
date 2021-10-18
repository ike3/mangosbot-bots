#pragma once
#include "../Value.h"
#include "PartyMemberValue.h"

namespace ai
{
    class PartyMemberToHeal : public PartyMemberValue
	{
	public:
        PartyMemberToHeal(PlayerbotAI* ai, string name = "party member to heal") :
          PartyMemberValue(ai, name) {}
    
    protected:
        virtual Unit* Calculate();
        bool CanHealPet(Pet* pet);
        virtual bool Check(Unit* player);
	};

    class PartyMemberToProtect : public PartyMemberValue
    {
    public:
        PartyMemberToProtect(PlayerbotAI* ai, string name = "party member to protect") :
            PartyMemberValue(ai, name) {}

    protected:
        virtual Unit* Calculate();
    };
}
