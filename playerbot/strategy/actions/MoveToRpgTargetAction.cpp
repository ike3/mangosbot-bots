#include "botpch.h"
#include "../../playerbot.h"
#include "MoveToRpgTargetAction.h"
#include "ChooseRpgTargetAction.h"
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
    if (distance > 180.0f || (target->IsMoving() && urand(1,100) < 5) || !ChooseRpgTargetAction::isFollowValid(bot, target))
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

    float angle;
    
    if (bot->IsWithinLOS(x, y, z))
    {
        if (!target->IsMoving())
            angle = target->GetAngle(bot) + (M_PI * irand(-25, 25) / 100.0); //Closest 45 degrees towards the target
        else
            angle = target->GetOrientation() + (M_PI * irand(-25, 25) / 100.0); //45 degrees infront of target (leading it's movement)
    }
    else
        angle = 2 * M_PI * urand(0, 100) / 100.0; //A circle around the target.

    x += cos(angle) * sPlayerbotAIConfig.followDistance;
    y += sin(angle) * sPlayerbotAIConfig.followDistance;
    
    //WaitForReach(distance);
    
    if (bot->IsWithinLOS(x, y, z))
        return MoveNear(mapId, x , y, z, 0);
    else
        return MoveTo(mapId, x, y, z, false, false);

    /*
    if (bot->IsSitState())
        bot->SetStandState(UNIT_STAND_STATE_STAND);

    if (bot->IsNonMeleeSpellCasted(true))
    {
        bot->CastStop();
        ai->InterruptSpell();
    }


    angle = 2 * M_PI * urand(0, 100) / 100.0; //A circle around the target.
    bool generatePath = !bot->IsFlying() && !sServerFacade.IsUnderwater(bot);
    MotionMaster &mm = *bot->GetMotionMaster();
#ifdef MANGOS
    mm.MovePoint(mapId, x + cos(angle) * sPlayerbotAIConfig.followDistance, y + sin(angle) * sPlayerbotAIConfig.followDistance, z, generatePath);
#endif
#ifdef CMANGOS
    bot->StopMoving();
    mm.Clear();
    mm.MovePoint(mapId, x + cos(angle) * sPlayerbotAIConfig.followDistance, y + sin(angle) * sPlayerbotAIConfig.followDistance, z, FORCED_MOVEMENT_RUN, generatePath);
#endif

    AI_VALUE(LastMovement&, "last movement").Set(x + cos(angle) * sPlayerbotAIConfig.followDistance, y + sin(angle) * sPlayerbotAIConfig.followDistance, z, bot->GetOrientation());
    return true;
    */
}

bool MoveToRpgTargetAction::isUseful()
{
    return context->GetValue<ObjectGuid>("rpg target")->Get() 
        && !context->GetValue<TravelTarget *>("travel target")->Get()->isTraveling()  
        && AI_VALUE2(float, "distance", "rpg target") > sPlayerbotAIConfig.followDistance
        && !bot->IsInCombat();
}


