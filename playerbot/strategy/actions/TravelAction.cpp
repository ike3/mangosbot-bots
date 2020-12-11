#include "botpch.h"
#include "../../playerbot.h"
#include "TravelAction.h"
#include "../../PlayerbotAIConfig.h"
#include <playerbot\ServerFacade.h>


using namespace ai;

bool TravelAction::Execute(Event event)
{    

    ObjectGuid guid = AI_VALUE(ObjectGuid, "travel target");
    Creature* creature = ai->GetCreature(guid);
    Unit* target = ai->GetUnit(guid);
    if (!target || !target->IsAlive())
    {
        context->GetValue<ObjectGuid>("travel target")->Set(ObjectGuid());
        return false;
    }

    if (sServerFacade.isMoving(bot))
        return false;

    if (bot->GetMapId() != target->GetMapId())
    {
        context->GetValue<ObjectGuid>("travel target")->Set(ObjectGuid());
        return false;
    }

    if (!sServerFacade.IsInFront(bot, target, sPlayerbotAIConfig.tooCloseDistance, CAST_ANGLE_IN_FRONT) && !bot->IsTaxiFlying())
    {
        sServerFacade.SetFacingTo(bot, target, true);
        ai->SetNextCheckDelay(sPlayerbotAIConfig.globalCoolDown);
        return false;
    }

    //ostringstream os; os << "Arrived at: " << target->GetName();

    //ai->TellMaster(os);

    if(target->IsFriendlyTo(bot))
        context->GetValue<ObjectGuid>("rpg target")->Set(target->GetObjectGuid());
    else
        context->GetValue<ObjectGuid>("pull target")->Set(target->GetObjectGuid());

    context->GetValue<ObjectGuid>("travel target")->Set(ObjectGuid());
    return true;
}

bool TravelAction::isUseful()
{
    return context->GetValue<ObjectGuid>("travel target")->Get();
}
