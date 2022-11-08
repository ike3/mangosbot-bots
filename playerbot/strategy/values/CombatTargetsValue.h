#pragma once
#include "../Value.h"
#include "../../PlayerbotAIConfig.h"

namespace ai
{
    class CombatTargetsValue : public ObjectGuidListCalculatedValue
	{
        struct AddGuardiansHelper
        {
            explicit AddGuardiansHelper(list<Unit*>& units) : units(units) {}
            void operator()(Unit* target) const
            {
                units.push_back(target);
            }

            list<Unit*>& units;
        };

	public:
        CombatTargetsValue(PlayerbotAI* ai) : ObjectGuidListCalculatedValue(ai, "combat targets") {}
        list<ObjectGuid> Calculate();

	private:
        void AddTargetsOf(Group* group, set<Unit*>& targets);
        void AddTargetsOf(Player* player, set<Unit*>& targets);
        float GetRange() const { return sPlayerbotAIConfig.sightDistance; }
        bool IsValid(Unit* target, Player* player) const;
    };
}
