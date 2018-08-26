#pragma once
#include "../Value.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"

namespace ai
{
    class NearestUnitsValue : public ObjectGuidListCalculatedValue
	{
	public:
        NearestUnitsValue(PlayerbotAI* ai, string name, float range = sPlayerbotAIConfig.sightDistance) :
            ObjectGuidListCalculatedValue(ai, name, 2), range(range) {}

	public:
        list<ObjectGuid> Calculate()
        {
            list<Unit*> targets;
            FindUnits(targets);

            list<ObjectGuid> results;
            for(list<Unit *>::iterator i = targets.begin(); i!= targets.end(); ++i)
            {
                Unit* unit = *i;
                if(sServerFacade.IsWithinLOSInMap(bot, unit) && AcceptUnit(unit))
                    results.push_back(unit->GetObjectGuid());
            }
            return results;
        }

    protected:
        virtual void FindUnits(list<Unit*> &targets) = 0;
        virtual bool AcceptUnit(Unit* unit) = 0;

    protected:
        float range;
	};
}
