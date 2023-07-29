#include "botpch.h"
#include "../../playerbot.h"
#include "GenericSpellActions.h"
#include "ReachTargetActions.h"

using namespace std;
using namespace ai;

bool ReachTargetAction::Execute(Event event)
{
    Unit* target = AI_VALUE(Unit*, GetTargetName());
    if (!target) return false;

    if (sServerFacade.isMoving(target) &&
            sServerFacade.IsInFront(target, bot, sPlayerbotAIConfig.sightDistance, CAST_ANGLE_IN_FRONT) &&
            sServerFacade.IsDistanceGreaterThan(sServerFacade.GetDistance2d(bot, target), sPlayerbotAIConfig.tooCloseDistance))
        return true;

    return MoveTo(target, distance);
}
