#include "botpch.h"
#include "../../playerbot.h"
#include "MoveToRpgTargetAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../values/PossibleRpgTargetsValue.h"

using namespace ai;

bool MoveToRpgTargetAction::Execute(Event event)
{
    Unit* target = AI_VALUE(Unit*, "rpg target");
    if (!target) return false;

    float distance = AI_VALUE2(float, "distance", "rpg target");
    if (distance > 180.0f)
    {
        context->GetValue<Unit*>("rpg target")->Set(NULL);
        return false;
    }

    float x = target->GetPositionX();
    float y = target->GetPositionY();
    float z = target->GetPositionZ();
    float mapId = target->GetMapId();
    if (bot->IsWithinLOS(x, y, z)) return MoveNear(target, sPlayerbotAIConfig.followDistance);

    WaitForReach(distance);

    if (bot->IsSitState())
        bot->SetStandState(UNIT_STAND_STATE_STAND);

    if (bot->IsNonMeleeSpellCasted(true))
    {
        bot->CastStop();
        ai->InterruptSpell();
    }

    bool generatePath = !bot->IsFlying() && !bot->IsUnderWater();
    MotionMaster &mm = *bot->GetMotionMaster();
#ifdef CMANGOS
    mm.Clear();
#endif
    mm.MovePoint(mapId, x, y, z, generatePath);

    AI_VALUE(LastMovement&, "last movement").Set(x, y, z, bot->GetOrientation());
    return true;
}

bool MoveToRpgTargetAction::isUseful()
{
    return context->GetValue<Unit*>("rpg target")->Get() && AI_VALUE2(float, "distance", "rpg target") > sPlayerbotAIConfig.followDistance;
}
