#pragma once
#include "../Value.h"
#include "NearestUnitsValue.h"
#include "../../PlayerbotAIConfig.h"

namespace ai
{
    // List of hostile and friendly targets in a range around the bot
    class PossibleTargetsValue : public NearestUnitsValue
	{
	public:
        PossibleTargetsValue(PlayerbotAI* ai, string name = "possible targets", float range = sPlayerbotAIConfig.sightDistance, bool ignoreLos = false) :
          NearestUnitsValue(ai, name, range, ignoreLos) {}

    public:
        static void FindPossibleTargets(Player* player, list<Unit*>& targets, float range);
        static bool IsValid(Player* player, Unit* target, float range);

    protected:
        virtual void FindUnits(list<Unit*> &targets);
        virtual bool AcceptUnit(Unit* unit);
	};

    class AllTargetsValue : public PossibleTargetsValue
	{
	public:
        AllTargetsValue(PlayerbotAI* ai, float range = sPlayerbotAIConfig.sightDistance) :
        PossibleTargetsValue(ai, "all targets", range, true) {}
	};
}
