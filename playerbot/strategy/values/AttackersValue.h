#pragma once
#include "../Value.h"
#include "../../PlayerbotAIConfig.h"

namespace ai
{
    // List of hostile targets that are in combat with the bot (or bot group)
    class AttackersValue : public ObjectGuidListCalculatedValue, public Qualified
	{
        struct AddGuardiansHelper
        {
            explicit AddGuardiansHelper(set<Unit*>& units) : units(units) {}
            void operator()(Unit* target) const
            {
                units.insert(target);
            }

            set<Unit*>& units;
        };

	public:
        AttackersValue(PlayerbotAI* ai) : ObjectGuidListCalculatedValue(ai, "attackers", 2) {}
        list<ObjectGuid> Calculate();

        static bool IsValid(Unit* target, Player* player, Player* owner = nullptr, bool checkInCombat = true, bool validatePossibleTarget = true);

	private:
        void AddTargetsOf(Group* group, set<Unit*>& targets, set<ObjectGuid>& invalidTargets, bool getOne = false);
        void AddTargetsOf(Player* player, set<Unit*>& targets, set<ObjectGuid>& invalidTargets, bool getOne = false);
        static float GetRange() { return sPlayerbotAIConfig.sightDistance; }

        static bool InCombat(Unit* target, Player* player, bool checkPullTargets = true);
    };
}
