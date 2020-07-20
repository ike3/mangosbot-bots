#include "botpch.h"
#include "../../playerbot.h"
#include "InvalidTargetValue.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"

using namespace ai;

bool InvalidTargetValue::Calculate()
{
    Unit* target = AI_VALUE(Unit*, qualifier);
    if (qualifier == "current target")
    {
        return !target ||
                target->GetMapId() != bot->GetMapId() ||
                sServerFacade.UnitIsDead(target) ||
                target->IsPolymorphed() ||
                sServerFacade.IsCharmed(target) ||
                sServerFacade.IsFeared(target) ||
#ifdef CMANGOS
				target->GetCombatManager().IsInEvadeMode() ||
#endif
                target->hasUnitState(UNIT_STAT_ISOLATED) ||
                sServerFacade.IsFriendlyTo(target, bot) ||
                !bot->IsWithinDistInMap(target, sPlayerbotAIConfig.sightDistance) ||
				!sServerFacade.IsWithinLOSInMap(bot, target) ||
				target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE) ||
				sPlayerbotAIConfig.IsInPvpProhibitedZone(target->GetAreaId()) || sPlayerbotAIConfig.IsInPvpProhibitedZone(bot->GetAreaId());
    }

    return !target;
}
