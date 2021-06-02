#include "botpch.h"
#include "../../playerbot.h"
#include "PvpTriggers.h"
#include "ServerFacade.h"
#include "BattleGroundWS.h"

using namespace ai;

bool EnemyPlayerNear::IsActive()
{
    return AI_VALUE(Unit*, "enemy player target");
}

bool PlayerHasNoFlag::IsActive()
{
#ifdef MANGOS
    if (ai->GetBot()->InBattleGround())
    {
        if (ai->GetBot()->GetBattleGroundTypeId() == BattleGroundTypeId::BATTLEGROUND_WS)
        {
            BattleGroundWS *bg = (BattleGroundWS*)ai->GetBot()->GetBattleGround();
            if (!(bg->GetFlagState(bg->GetOtherTeam(bot->GetTeam())) == BG_WS_FLAG_STATE_ON_PLAYER))
                return true;
            if (bot->GetObjectGuid() == bg->GetAllianceFlagCarrierGuid() || bot->GetObjectGuid() == bg->GetHordeFlagCarrierGuid())
            {
                return false;
            }
            return true;
        }
        return false;
    }
#endif
    return false;
}

bool PlayerIsInBattleground::IsActive()
{
    return ai->GetBot()->InBattleGround();
}

bool BgWaitingTrigger::IsActive()
{
    if (bot->InBattleGround())
    {
        if (bot->GetBattleGround() && bot->GetBattleGround()->GetStatus() == STATUS_WAIT_JOIN)
            return true;
    }
    return false;
}

bool BgActiveTrigger::IsActive()
{
    if (bot->InBattleGround())
    {
        if (bot->GetBattleGround() && bot->GetBattleGround()->GetStatus() == STATUS_IN_PROGRESS)
            return true;
    }
    return false;
}

bool PlayerIsInBattlegroundWithoutFlag::IsActive()
{
#ifdef MANGOS
    if (ai->GetBot()->InBattleGround())
    {
        if (ai->GetBot()->GetBattleGroundTypeId() == BattleGroundTypeId::BATTLEGROUND_WS)
        {
            BattleGroundWS *bg = (BattleGroundWS*)ai->GetBot()->GetBattleGround();
            if (!(bg->GetFlagState(bg->GetOtherTeam(bot->GetTeam())) == BG_WS_FLAG_STATE_ON_PLAYER))
                return true;
            if (bot->GetGUIDLow() == bg->GetAllianceFlagCarrierGuid() || bot->GetGUIDLow() == bg->GetHordeFlagCarrierGuid())
            {
                return false;
            }
        }
        return true;
    }
#endif
    return false;
}

bool PlayerHasFlag::IsActive()
{
    if (bot->InBattleGround())
    {
        if (bot->GetBattleGroundTypeId() == BattleGroundTypeId::BATTLEGROUND_WS)
        {
            BattleGroundWS *bg = (BattleGroundWS*)ai->GetBot()->GetBattleGround();
            if (bot->GetObjectGuid() == bg->GetFlagCarrierGuid(TEAM_INDEX_ALLIANCE) || bot->GetObjectGuid() == bg->GetFlagCarrierGuid(TEAM_INDEX_HORDE))
            {
                return true;
            }
        }
        return false;
    }
    return false;
}

bool TeamHasFlag::IsActive()
{
#ifdef MANGOS
    if (ai->GetBot()->InBattleGround())
    {
        if (ai->GetBot()->GetBattleGroundTypeId() == BattleGroundTypeId::BATTLEGROUND_WS)
        {
            BattleGroundWS *bg = (BattleGroundWS*)ai->GetBot()->GetBattleGround();

            if (bot->GetObjectGuid() == bg->GetAllianceFlagCarrierGuid() || bot->GetObjectGuid() == bg->GetHordeFlagCarrierGuid())
            {
                return false;
            }

            if (bg->GetFlagState(bg->GetOtherTeam(bot->GetTeam())) == BG_WS_FLAG_STATE_ON_PLAYER)
                return true;
        }
        return false;
    }
#endif
    return false;
}

bool EnemyTeamHasFlag::IsActive()
{
    if (ai->GetBot()->InBattleGround())
    {
        if (ai->GetBot()->GetBattleGroundTypeId() == BattleGroundTypeId::BATTLEGROUND_WS)
        {
            BattleGroundWS *bg = (BattleGroundWS*)ai->GetBot()->GetBattleGround();

            if (bot->GetTeam() == HORDE)
            {
                if (!bg->GetFlagCarrierGuid(TEAM_INDEX_HORDE).IsEmpty())
                    return true;
            }
            else
            {
                if (!bg->GetFlagCarrierGuid(TEAM_INDEX_ALLIANCE).IsEmpty())
                    return true;
            }
        }
        return false;
    }
    return false;
}

bool EnemyFlagCarrierNear::IsActive()
{
    Unit* carrier = AI_VALUE(Unit*, "enemy flag carrier");
    return carrier && sServerFacade.IsDistanceLessOrEqualThan(sServerFacade.GetDistance2d(bot, carrier), VISIBILITY_DISTANCE_LARGE);
}

bool TeamFlagCarrierNear::IsActive()
{
    Unit* carrier = AI_VALUE(Unit*, "team flag carrier");
    return carrier && sServerFacade.IsDistanceLessOrEqualThan(sServerFacade.GetDistance2d(bot, carrier), VISIBILITY_DISTANCE_LARGE);
}

bool PlayerWantsInBattlegroundTrigger::IsActive()
{
    if (bot->InBattleGround())
        return false;

    if (bot->GetBattleGround() && bot->GetBattleGround()->GetStatus() == STATUS_WAIT_JOIN)
        return false;

    if (bot->GetBattleGround() && bot->GetBattleGround()->GetStatus() == STATUS_IN_PROGRESS)
        return false;

    if (!bot->CanJoinToBattleground())
        return false;

    return true;
};
