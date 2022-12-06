#pragma once
#include "../Value.h"
#include "TargetValue.h"
#include "NearestUnitsValue.h"

namespace ai
{
    // List of hostile targets that are in combat with the bot (or bot group) and can be attacked (is reachable, not cc'ed, etc...)
    class PossibleAttackTargetsValue : public ObjectGuidListCalculatedValue, public Qualified
	{
	public:
        PossibleAttackTargetsValue(PlayerbotAI* ai) : ObjectGuidListCalculatedValue(ai, "possible attack targets", 2) {}
        list<ObjectGuid> Calculate();

        static bool IsValid(Unit* target, Player* player, float range = sPlayerbotAIConfig.sightDistance, bool ignoreCC = false, bool checkAttackerValid = true);

        static bool HasBreakableCC(Unit* target, Player* player);
        static bool HasUnBreakableCC(Unit* target, Player* player);

	private:
		void RemoveNonThreating(list<ObjectGuid>& targets, bool getOne);

        static bool IsPossibleTarget(Unit* target, Player *player, float range, bool ignoreCC);
        static bool HasIgnoreCCRti(Unit* target, Player* player);
        static bool IsTapped(Unit* target, Player* player);
    };

    class PossibleAddsValue : public BoolCalculatedValue
    {
    public:
        PossibleAddsValue(PlayerbotAI* const ai, string name = "possible adds") : BoolCalculatedValue(ai, name) {}
        virtual bool Calculate();
    };
}
