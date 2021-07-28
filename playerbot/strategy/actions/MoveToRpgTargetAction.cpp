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
    Unit* unit = ai->GetUnit(AI_VALUE(ObjectGuid, "rpg target"));
    GameObject* go = ai->GetGameObject(AI_VALUE(ObjectGuid, "rpg target"));
    WorldObject* wo;
    if (unit)
        wo = unit;
    else if(go)
        wo = go;
    else
        return false;

    if (ai->HasStrategy("debug rpg", BOT_STATE_NON_COMBAT))
    {
        ostringstream out;
        out << "Heading to: ";
        out << chat->formatWorldobject(wo);
        ai->TellMasterNoFacing(out);
    }

    if ((unit && unit->IsMoving() && !urand(0, 20))
        || !ChooseRpgTargetAction::isFollowValid(bot, wo)
        || !urand(0, 50))
    {
        context->GetValue<set<ObjectGuid>&>("ignore rpg target")->Get().insert(AI_VALUE(ObjectGuid, "rpg target"));

        context->GetValue<ObjectGuid>("rpg target")->Set(ObjectGuid());
        return false;
    }

    float x = wo->GetPositionX();
    float y = wo->GetPositionY();
    float z = wo->GetPositionZ();
    float mapId = wo->GetMapId();
	
	if (sPlayerbotAIConfig.RandombotsWalkingRPG)
	{
        bot->m_movementInfo.AddMovementFlag(MOVEFLAG_WALK_MODE);
	}

    float angle;
    
    if (bot->IsWithinLOS(x, y, z))
    {
        if (!unit || !unit->IsMoving())
            angle = wo->GetAngle(bot) + (M_PI * irand(-25, 25) / 100.0); //Closest 45 degrees towards the target
        else
            angle = wo->GetOrientation() + (M_PI * irand(-25, 25) / 100.0); //45 degrees infront of target (leading it's movement)
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
}

bool MoveToRpgTargetAction::isUseful()
{
    return context->GetValue<ObjectGuid>("rpg target")->Get()
        && !context->GetValue<TravelTarget*>("travel target")->Get()->isTraveling()
        && AI_VALUE2(float, "distance", "rpg target") > sPlayerbotAIConfig.followDistance
        && AI_VALUE2(uint8, "health", "self target") > sPlayerbotAIConfig.almostFullHealth 
        && (!AI_VALUE2(uint8, "mana", "self target") || AI_VALUE2(uint8, "mana", "self target") > sPlayerbotAIConfig.mediumMana)
        && !bot->IsInCombat();
}


