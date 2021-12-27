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
        EnemyPlayerValue(PlayerbotAI* ai, string name = "enemy player") : UnitCalculatedValue(ai, name) {}

        virtual Unit* Calculate();
    private:
        float GetMaxAttackDistance()
        {
            if (!bot->GetBattleGround())
                return 60.0f;

#ifdef MANGOSBOT_TWO
            if (bot->InBattleGround())
            {
                BattleGround* bg = bot->GetBattleGround();
                if (!bg)
                    return 40.0f;

                BattleGroundTypeId bgType = bg->GetTypeId();
                if (bgType == BATTLEGROUND_RB)
                    bgType = bg->GetTypeId(true);

                if (bgType == BATTLEGROUND_IC)
                {
                    if (ai->IsInVehicle(false, true))
                        return 120.0f;
                }
            }
#endif

            return 40.0f;
        }
    };
}
