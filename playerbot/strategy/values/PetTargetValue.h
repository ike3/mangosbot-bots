#pragma once
#include "../Value.h"

namespace ai
{
    class PetTargetValue : public UnitCalculatedValue
	{
	public:
        PetTargetValue(PlayerbotAI* ai, string name = "pet target") : UnitCalculatedValue(ai, name) {}

        virtual Unit* Calculate() { return (Unit*)(ai->GetBot()->GetPet()); }
    };
}
