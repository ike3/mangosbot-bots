#include "botpch.h"
#include "../../playerbot.h"
#include "InvalidTargetValue.h"
#include "AttackersValue.h"
#include "EnemyPlayerValue.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"

using namespace ai;

bool InvalidTargetValue::Calculate()
{
    Unit* target = AI_VALUE(Unit*, qualifier);
    Unit* duel = AI_VALUE(Unit*, "duel target");
    if (duel && duel == target)
        return false;

    if (!target && !bot->GetSelectionGuid())
        return true;

    bool validTarget = AttackersValue::IsValidTarget(target, bot);
    return !validTarget;
}
