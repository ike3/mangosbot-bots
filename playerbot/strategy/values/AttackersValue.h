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

        static bool IsValid(Unit* target, Player* player, bool checkInCombat = true);

	private:
        void AddTargetsOf(Group* group, set<Unit*>& targets, bool getOne = false);
        void AddTargetsOf(Player* player, set<Unit*>& targets, bool getOne = false);
        static float GetRange() { return sPlayerbotAIConfig.sightDistance; }
        bool IsPossibleTarget(Unit* target, Player* player) const;

        static bool IsFriendly(Unit* target, Player* player);
        static bool IsAttackable(Unit* target, Player* player, bool inVehicle = false);
        static bool InCombat(Unit* target, Player* player, bool checkPullTargets = true);
    };
}
