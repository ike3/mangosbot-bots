#include "botpch.h"
#include "../../playerbot.h"
#include "TravelAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"


using namespace ai;
using namespace MaNGOS;

bool TravelAction::Execute(Event event)
{    
    TravelTarget * target = AI_VALUE(TravelTarget *, "travel target");
    
    if (sServerFacade.isMoving(bot))
        return false;

    target->setStatus(TRAVEL_STATUS_WORK);

     Unit* newTarget;
    list<Unit*> targets;
    AnyUnitInObjectRangeCheck u_check(bot, sPlayerbotAIConfig.sightDistance * 2);
    UnitListSearcher<AnyUnitInObjectRangeCheck> searcher(targets, u_check);
    Cell::VisitAllObjects(bot, searcher, sPlayerbotAIConfig.sightDistance * 2);

    for (auto& i : targets)
    {
        newTarget = i;

        if (!newTarget)
            continue;

        if (newTarget->GetMapId() != bot->GetMapId())
            continue;

        if (!newTarget->IsAlive())
            continue;

        if (!newTarget->GetEntry() != target->getDestination()->getEntry())
            continue;

        if (newTarget->IsInCombat())
            continue;

        if (sServerFacade.IsHostileTo(bot, newTarget))
            context->GetValue<ObjectGuid>("pull target")->Set(newTarget->GetObjectGuid());
        else
            context->GetValue<ObjectGuid>("rpg target")->Set(newTarget->GetObjectGuid());

        break;
    }

    return true;
}

bool TravelAction::isUseful()
{
    return false && context->GetValue<TravelTarget *>("travel target")->Get()->isActive() && (!context->GetValue<ObjectGuid>("rpg target")->Get() || !context->GetValue<ObjectGuid>("pull target")->Get());
}
