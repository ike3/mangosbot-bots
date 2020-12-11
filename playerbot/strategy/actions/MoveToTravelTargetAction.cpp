#include "botpch.h"
#include "../../playerbot.h"
#include "MoveToTravelTargetAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"
#include "../../LootObjectStack.h"

using namespace ai;

bool MoveToTravelTargetAction::Execute(Event event)
{
    Unit* target = ai->GetUnit(AI_VALUE(ObjectGuid, "travel target"));
    if (!target || target->IsInCombat())
    {
        context->GetValue<ObjectGuid>("travel target")->Set(ObjectGuid());
        return false;
    }        

    float distance = AI_VALUE2(float, "distance", "travel target");

    //ostringstream os; os << "Travel to: " << target->GetName() << " at distance " << distance;

    //ai->TellMaster(os);

    float x = target->GetPositionX();
    float y = target->GetPositionY();
    float z = target->GetPositionZ();
    float mapId = target->GetMapId();

    if (distance < 80.0f)
        if (bot->IsWithinLOS(x, y, z)) return MoveNear(target, sPlayerbotAIConfig.tooCloseDistance);

    WaitForReach(distance);

    if (bot->IsSitState())
        bot->SetStandState(UNIT_STAND_STATE_STAND);

    if (bot->IsNonMeleeSpellCasted(true))
    {
        bot->CastStop();
        ai->InterruptSpell();
    }

    bool generatePath = !bot->IsFlying() && !sServerFacade.IsUnderwater(bot);
    MotionMaster &mm = *bot->GetMotionMaster();
    mm.MovePoint(mapId, x, y, z, generatePath);

    AI_VALUE(LastMovement&, "last movement").Set(x, y, z, bot->GetOrientation());
    return true;
}

bool MoveToTravelTargetAction::isUseful()
{
    return context->GetValue<ObjectGuid>("travel target")->Get() && context->GetValue<LootObject>("loot target")->Get().IsEmpty() && /*!context->GetValue<ObjectGuid>("pull target")->Get() &&*/ AI_VALUE2(float, "distance", "travel target") > sPlayerbotAIConfig.tooCloseDistance;
}

