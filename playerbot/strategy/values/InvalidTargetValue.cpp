#include "botpch.h"
#include "../../playerbot.h"
#include "InvalidTargetValue.h"
#include "PossibleAttackTargetsValue.h"
#include "EnemyPlayerValue.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"

using namespace ai;

bool InvalidTargetValue::Calculate()
{
    Unit* target = AI_VALUE(Unit*, qualifier);
    if (!target || !target->IsInWorld() || target->GetMapId() != bot->GetMapId())
        return true;

    Unit* duel = AI_VALUE(Unit*, "duel target");
    if (duel && duel == target)
        return false;

    if (qualifier == "current target")
    {
        if (target->GetObjectGuid() != bot->GetSelectionGuid())
            return true;
    }

    const bool validTarget = PossibleAttackTargetsValue::IsValidTarget(target, bot);
    if (!validTarget)
    {
        list<ObjectGuid> attackers = AI_VALUE(list<ObjectGuid>, "possible attack targets");
        if (std::find(attackers.begin(), attackers.end(), target->GetObjectGuid()) != attackers.end())
            return false;
    }

    return !validTarget;
}
