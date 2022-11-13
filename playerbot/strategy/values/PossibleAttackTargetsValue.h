#pragma once
#include "../Value.h"
#include "TargetValue.h"
#include "NearestUnitsValue.h"

namespace ai
{
    // List of hostile targets that are in combat with the bot (or bot group) and can be attacked (is reachable, not cc'ed, etc...)
    class PossibleAttackTargetsValue : public ObjectGuidListCalculatedValue
	{
	public:
        PossibleAttackTargetsValue(PlayerbotAI* ai) : ObjectGuidListCalculatedValue(ai, "possible attack targets") {}
        list<ObjectGuid> Calculate();

	private:
		void RemoveNonThreating(list<ObjectGuid>& targets);

    public:
        static bool HasIgnoreCCRti(Unit* attacker, Player* player);
        static bool HasBreakableCC(Unit* attacker, Player* player);
        static bool HasUnBreakableCC(Unit* attacker, Player* player);
        static bool IsPossibleTarget(Unit* attacker, Player *player, float range = sPlayerbotAIConfig.sightDistance, bool ignoreCC = false);
        static bool IsValidTarget(Unit* attacker, Player* player, bool ignoreCC = false);
    };

    class PossibleAddsValue : public BoolCalculatedValue
    {
    public:
        PossibleAddsValue(PlayerbotAI* const ai, string name = "possible adds") : BoolCalculatedValue(ai, name) {}
        virtual bool Calculate();
    };
}
