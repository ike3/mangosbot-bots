#pragma once
#include "../Value.h"
#include "NearestUnitsValue.h"
#include "../../PlayerbotAIConfig.h"

namespace ai
{
    class NearestNpcsValue : public NearestUnitsValue
	{
	public:
        NearestNpcsValue(PlayerbotAI* ai, float range = sPlayerbotAIConfig.sightDistance) :
          NearestUnitsValue(ai, "nearest npcs", range) {}

    protected:
        void FindUnits(list<Unit*> &targets);
        bool AcceptUnit(Unit* unit);
	};

    class NearestVehiclesValue : public NearestUnitsValue
    {
    public:
        NearestVehiclesValue(PlayerbotAI* ai, float range = sPlayerbotAIConfig.sightDistance) :
            NearestUnitsValue(ai, "nearest vehicles", range) {}

    protected:
        void FindUnits(list<Unit*>& targets);
        bool AcceptUnit(Unit* unit);
    };
}
