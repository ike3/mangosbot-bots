#pragma once
#include "../Value.h"
#include "../../PlayerbotAIConfig.h"

namespace ai
{
    // List of hostile targets that are in combat with the bot (or bot group)
    class AttackersValue : public ObjectGuidListCalculatedValue
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
        AttackersValue(PlayerbotAI* ai) : ObjectGuidListCalculatedValue(ai, "attackers") {}
        list<ObjectGuid> Calculate();

	private:
        void AddTargetsOf(Group* group, set<Unit*>& targets);
        void AddTargetsOf(Player* player, set<Unit*>& targets);
        float GetRange() const { return sPlayerbotAIConfig.sightDistance; }
        bool IsValid(Unit* target, Player* player) const;
    };
}
