#pragma once
#include "../Value.h"
#include "TargetValue.h"
#include "PossibleTargetsValue.h"

namespace ai
{
    class EnemyPlayerValue : public UnitCalculatedValue
    {
    public:
        EnemyPlayerValue(PlayerbotAI* ai, string name = "enemy player") : UnitCalculatedValue(ai, name) {}
        virtual Unit* Calculate();

        //static float GetMaxAttackDistance(Player* bot);
    };
}
