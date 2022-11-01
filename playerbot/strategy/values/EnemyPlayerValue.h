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
        NearestEnemyPlayersValue(PlayerbotAI* ai, float range = 60.0f) :
            PossibleTargetsValue(ai, "nearest enemy players", range, true) {}

    public:
        virtual bool AcceptUnit(Unit* unit);
    };

    class EnemyPlayerValue : public UnitCalculatedValue
    {
    public:
        EnemyPlayerValue(PlayerbotAI* ai, string name = "enemy player") : UnitCalculatedValue(ai, name) {}

        virtual Unit* Calculate();
        static float GetMaxAttackDistance(Player* bot)
        {
            if (!bot->GetBattleGround())
                return 60.0f;

            if (bot->InBattleGround())
            {
                BattleGround* bg = bot->GetBattleGround();
                if (!bg)
                    return 40.0f;

                BattleGroundTypeId bgType = bg->GetTypeId();

#ifdef MANGOSBOT_TWO
                if (bgType == BATTLEGROUND_RB)
                    bgType = bg->GetTypeId(true);

                if (bgType == BATTLEGROUND_IC)
                {
                    if (bot->GetPlayerbotAI()->IsInVehicle(false, true))
                        return 120.0f;
                }
#endif
                if (bgType == BATTLEGROUND_AV)
                {
                    bool strifeTime = bg->GetStartTime() < (uint32)(20 * MINUTE * IN_MILLISECONDS);
                    return strifeTime ? 40.0f : 10.0f;
                }
            }

            return 40.0f;
        }
    };
}
