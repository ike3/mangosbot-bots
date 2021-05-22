#pragma once
#include "../Value.h"
#include "TargetValue.h"
#include "PossibleTargetsValue.h"

namespace ai
{
    /*class EnemyPlayerValue : public TargetValue
	{
	public:
        EnemyPlayerValue(PlayerbotAI* ai) : TargetValue(ai) {}

    public:
        Unit* Calculate();
    };*/

    class NearestEnemyPlayersValue : public PossibleTargetsValue
    {
    public:
        NearestEnemyPlayersValue(PlayerbotAI* ai, float range = sPlayerbotAIConfig.sightDistance) :
            PossibleTargetsValue(ai, "nearest enemy players", range) {}

    public:
        virtual bool AcceptUnit(Unit* unit);
    };

    class EnemyPlayerValue : public UnitCalculatedValue
    {
    public:
        EnemyPlayerValue(PlayerbotAI* ai) : UnitCalculatedValue(ai) {}

        virtual Unit* Calculate();
    private:
        float GetMaxAttackDistance()
        {
            if (!bot->GetBattleGround())
                return 60.0f;

            return 40.0f;
        }
    };
}
