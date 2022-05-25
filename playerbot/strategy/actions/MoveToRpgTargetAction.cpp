#include "botpch.h"
#include "../../playerbot.h"
#include "MoveToRpgTargetAction.h"
#include "ChooseRpgTargetAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"
#include "../values/PossibleRpgTargetsValue.h"
#include "../../Travelmgr.h"
#include "ChooseRpgTargetAction.h"

using namespace ai;

bool MoveToRpgTargetAction::Execute(Event event)
{
    GuidPosition guidP = AI_VALUE(GuidPosition, "rpg target");
    Unit* unit = ai->GetUnit(guidP);
    GameObject* go = ai->GetGameObject(guidP);
    Player* player = guidP.GetPlayer();

    WorldObject* wo;
    if (unit)
        wo = unit;
    else if(go)
        wo = go;
    else
        return false;

    if (ai->HasStrategy("debug rpg", BOT_STATE_NON_COMBAT) && guidP.GetWorldObject())
    {
        ostringstream out;
        out << "Heading to: ";
        out << chat->formatWorldobject(guidP.GetWorldObject());
        ai->TellMasterNoFacing(out);
    }

    if (guidP.IsPlayer())
    {
        Player* player = guidP.GetPlayer();

        if (player && player->GetPlayerbotAI())
        {
            GuidPosition guidPP = PAI_VALUE(GuidPosition, "rpg target");

            if (guidPP.IsPlayer())
            {
                AI_VALUE(set<ObjectGuid>&,"ignore rpg target").insert(AI_VALUE(GuidPosition, "rpg target"));

                RESET_AI_VALUE(GuidPosition, "rpg target");
                return false;
            }
        }
    }

    if ((unit && unit->IsMoving() && !urand(0, 20))
        || !ChooseRpgTargetAction::isFollowValid(bot, wo)
        || guidP.distance(bot) > sPlayerbotAIConfig.reactDistance * 2
        || !urand(0, 50))
    {
        AI_VALUE(set<ObjectGuid>&,"ignore rpg target").insert(AI_VALUE(GuidPosition, "rpg target"));

        RESET_AI_VALUE(GuidPosition,"rpg target");
        return false;
    }

    float x = wo->GetPositionX();
    float y = wo->GetPositionY();
    float z = wo->GetPositionZ();
    float mapId = wo->GetMapId();
	
	if (sPlayerbotAIConfig.RandombotsWalkingRPG)
        if (!bot->GetTerrain()->IsOutdoors(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ()))
            bot->m_movementInfo.AddMovementFlag(MOVEFLAG_WALK_MODE);

    float angle;
    float distance = 1.0f;
    
    if (bot->IsWithinLOS(x, y, z, true))
    {
        if (!unit || !unit->IsMoving())
            angle = wo->GetAngle(bot) + (M_PI * irand(-25, 25) / 100.0); //Closest 45 degrees towards the target
        else
            angle = wo->GetOrientation() + (M_PI * irand(-25, 25) / 100.0); //45 degrees infront of target (leading it's movement)

        distance = frand(0.5, 1);
    }
    else
        angle = 2 * M_PI * urand(0, 100) / 100.0; //A circle around the target.

    x += cos(angle) * INTERACTION_DISTANCE * distance;
    y += sin(angle) * INTERACTION_DISTANCE * distance;
    
    //WaitForReach(distance);

    bool couldMove;
    
    if (bot->IsWithinLOS(x, y, z, true))
        couldMove = MoveNear(mapId, x , y, z, 0);
    else
        couldMove = MoveTo(mapId, x, y, z, false, false);

    if (!couldMove && WorldPosition(mapId,x,y,z).distance(bot) > INTERACTION_DISTANCE)
    {
        AI_VALUE(set<ObjectGuid>&,"ignore rpg target").insert(AI_VALUE(GuidPosition, "rpg target"));

        RESET_AI_VALUE(GuidPosition, "rpg target");
    }

    return couldMove;
}

bool MoveToRpgTargetAction::isUseful()
{
    GuidPosition guidP = AI_VALUE(GuidPosition, "rpg target");

    if (!guidP)
        return false;

    WorldObject* wo = guidP.GetWorldObject();

    if (!wo)
        return false;

    TravelTarget* travelTarget = AI_VALUE(TravelTarget*, "travel target");

    if (travelTarget->isTraveling() && ChooseRpgTargetAction::isFollowValid(bot,*travelTarget->getPosition()))
        return false;

    if (guidP.distance(bot) < INTERACTION_DISTANCE)
        return false;

    if (!ChooseRpgTargetAction::isFollowValid(bot, wo))
        return false;

    if (!AI_VALUE(bool, "can move around"))
        return false;

    return true;
}


