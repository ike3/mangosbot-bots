#pragma once

#include "botpch.h"
#include "../../playerbot.h"
#include "FollowActions.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"
#include "../values/Formations.h"
#include "ChooseRpgTargetAction.h"
#include "../../TravelMgr.h"
#include "../../LootObjectStack.h"

using namespace ai;


bool FollowAction::Execute(Event& event)
{
    Formation* formation = AI_VALUE(Formation*, "formation");
    string target = formation->GetTargetName();
    bool moved = false;
    if (!target.empty())
    {
        moved = Follow(AI_VALUE(Unit*, target));
    }
    else
    {
        WorldLocation loc = formation->GetLocation();
        if (Formation::IsNullLocation(loc) || loc.mapid == -1)
            return false;

        moved = MoveTo(loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z);
    }

    //if (moved) SetDuration(sPlayerbotAIConfig.reactDelay);
    return moved;
}

bool FollowAction::isUseful()
{
    Formation* formation = AI_VALUE(Formation*, "formation");
    float distance = 0;
    string target = formation->GetTargetName();

    Unit* fTarget = NULL;
    if (!target.empty())
        fTarget = AI_VALUE(Unit*, target);
    else
        fTarget = AI_VALUE(Unit*, "master target");

    if (fTarget && fTarget->IsPlayer())
    {
        if (AI_VALUE(GuidPosition, "rpg target") && ChooseRpgTargetAction::isFollowValid(bot, AI_VALUE(GuidPosition, "rpg target")))
            return false;
    }

    if (fTarget)
    {
        if (fTarget->IsTaxiFlying())
            return false;

        if (!CanDeadFollow(fTarget))
            return false;

        if (fTarget->GetGUIDLow() == bot->GetGUIDLow())
            return false;        
    }

    if (!target.empty())
    {
        distance = AI_VALUE2(float, "distance", target);
    }
    else
    {
        WorldLocation loc = formation->GetLocation();
        if (Formation::IsNullLocation(loc) || bot->GetMapId() != loc.mapid)
            return false;

        distance = sServerFacade.GetDistance2d(bot, loc.coord_x, loc.coord_y);
    }

    return sServerFacade.IsDistanceGreaterThan(distance, formation->GetMaxDistance());
}

bool FollowAction::CanDeadFollow(Unit* target)
{
    //Move to corpse when dead and player is alive or not a ghost.
    if (!sServerFacade.IsAlive(bot) && (sServerFacade.IsAlive(target) || !target->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST)))
        return false;
    
    return true;
}

bool FleeToMasterAction::Execute(Event& event)
{
    Unit* fTarget = AI_VALUE(Unit*, "master target");
    bool canFollow = Follow(fTarget);
    if (!canFollow)
    {
        //SetDuration(5000);
        return false;
    }

    WorldPosition targetPos(fTarget);
    WorldPosition bosPos(bot);
    float distance = bosPos.fDist(targetPos);

    if (distance > sPlayerbotAIConfig.reactDistance && bot->IsInCombat())
    {
        if (!urand(0, 5))
            ai->TellMaster("I'm heading to your location but I'm in combat", PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
            //ai->TellMaster(BOT_TEXT("wait_travel_combat"), PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
    }
    else if (distance < sPlayerbotAIConfig.reactDistance * 3)
    {
        if (!urand(0, 5))
            ai->TellMaster(BOT_TEXT("wait_travel_close"), PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
    }
    else if (distance < 1000)
    {
        if (!urand(0, 20))
            ai->TellMaster(BOT_TEXT("wait_travel_medium"), PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
    }
    else
        if (!urand(0, 30))
            ai->TellMaster(BOT_TEXT("wait_travel_medium"), PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
           
    SetDuration(3000U);
    return true;
}

bool FleeToMasterAction::isUseful()
{
    if (!ai->GetGroupMaster())
        return false;

    if (ai->GetGroupMaster() == bot)
        return false;

    Unit* target = AI_VALUE(Unit*, "current target");

    if (target && ai->GetGroupMaster()->HasTarget(target->GetObjectGuid()))
        return false;

    if (!ai->HasStrategy("follow", BotState::BOT_STATE_NON_COMBAT))
        return false;

    Unit* fTarget = AI_VALUE(Unit*, "master target");
    
    if (!CanDeadFollow(fTarget))
        return false;

    if (fTarget && fTarget->IsPlayer())
    {
        if (AI_VALUE(GuidPosition, "rpg target") && ChooseRpgTargetAction::isFollowValid(bot, AI_VALUE(GuidPosition, "rpg target")))
            return false;
    }

    LootObject loot = AI_VALUE(LootObject, "loot target");
    if (loot.IsLootPossible(bot))
        return false;

    return true;
}

