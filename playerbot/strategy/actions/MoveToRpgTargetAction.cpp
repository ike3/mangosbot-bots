#include "botpch.h"
#include "../../playerbot.h"
#include "MoveToRpgTargetAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"
#include "../values/PossibleRpgTargetsValue.h"
#include "../../Travelmgr.h"

using namespace ai;

bool MoveToRpgTargetAction::Execute(Event event)
{
    Unit* target = ai->GetUnit(AI_VALUE(ObjectGuid, "rpg target"));
    if (!target) return false;

    float distance = AI_VALUE2(float, "distance", "rpg target");
    if (distance > 180.0f)
    {
        context->GetValue<ObjectGuid>("rpg target")->Set(ObjectGuid());
        return false;
    }

    float x = target->GetPositionX();
    float y = target->GetPositionY();
    float z = target->GetPositionZ();
    float mapId = target->GetMapId();
	
	if (sPlayerbotAIConfig.RandombotsWalkingRPG)
	{
        bot->m_movementInfo.AddMovementFlag(MOVEFLAG_WALK_MODE);
	}

    float angle = 2 * M_PI * urand(0,100) / 100.0;

    if (bot->IsWithinLOS(x, y, z)) return MoveNear(mapId, x + cos(angle) * sPlayerbotAIConfig.followDistance, y + sin(angle) * sPlayerbotAIConfig.followDistance, z, 0);

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
#ifdef MANGOS
    mm.MovePoint(mapId, x, y, z, generatePath);
#endif
#ifdef CMANGOS
    mm.MovePoint(mapId, x, y, z, FORCED_MOVEMENT_RUN, generatePath);
#endif

    AI_VALUE(LastMovement&, "last movement").Set(x, y, z, bot->GetOrientation());
    return true;
}

bool MoveToRpgTargetAction::isUseful()
{
    return context->GetValue<ObjectGuid>("rpg target")->Get() && !context->GetValue<TravelTarget *>("travel target")->Get()->isTraveling()  && AI_VALUE2(float, "distance", "rpg target") > sPlayerbotAIConfig.followDistance;
}
