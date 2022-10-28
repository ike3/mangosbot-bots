#include "botpch.h"
#include "../../playerbot.h"
#include "ChatShortcutActions.h"
#include "../../PlayerbotAIConfig.h"
#include "../values/PositionValue.h"
#include "../values/Formations.h"

using namespace ai;

void ReturnPositionResetAction::ResetReturnPosition()
{
    ai::PositionMap& posMap = context->GetValue<ai::PositionMap&>("position")->Get();
    ai::PositionEntry pos = posMap["return"];
    pos.Reset();
    posMap["return"] = pos;
}

void ReturnPositionResetAction::SetReturnPosition(float x, float y, float z)
{
    ai::PositionMap& posMap = context->GetValue<ai::PositionMap&>("position")->Get();
    ai::PositionEntry pos = posMap["return"];
    pos.Set(x, y, z, ai->GetBot()->GetMapId());
    posMap["return"] = pos;
}

bool FollowChatShortcutAction::Execute(Event& event)
{
    Player* master = GetMaster();
    if (!master)
        return false;

    ai->Reset();
    ai->ChangeStrategy("+follow,-passive", BotState::BOT_STATE_NON_COMBAT);
    ai->ChangeStrategy("-follow,-passive", BotState::BOT_STATE_COMBAT);

    ai::PositionMap& posMap = context->GetValue<ai::PositionMap&>("position")->Get();
    ai::PositionEntry pos = posMap["return"];
    pos.Reset();
    posMap["return"] = pos;

    if (sServerFacade.IsInCombat(bot))
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
        if (moved)
        {
            ai->TellError(BOT_TEXT("following"));
            return true;
        }
    }
    /* Default mechanics takes care of this now.
    if (bot->GetMapId() != master->GetMapId() || (master && bot->GetDistance(master) > sPlayerbotAIConfig.sightDistance))
    {
        if (sServerFacade.UnitIsDead(bot))
        {
            bot->ResurrectPlayer(1.0f, false);
            ai->TellMasterNoFacing("Back from the grave!");
        }
        else
            ai->TellError("I will not follow you - too far away");

        bot->TeleportTo(master->GetMapId(), master->GetPositionX(), master->GetPositionY(), master->GetPositionZ(), master->GetOrientation());
        return true;
    }
    */

    ai->TellError(BOT_TEXT("following"));
    return true;
}

bool StayChatShortcutAction::Execute(Event& event)
{
    Player* master = GetMaster();
    if (!master)
        return false;

    ai->Reset();
    ai->ChangeStrategy("+stay,-passive", BotState::BOT_STATE_NON_COMBAT);
    ai->ChangeStrategy("-follow,-passive", BotState::BOT_STATE_COMBAT);

    SetReturnPosition(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ());

    ai->TellError(BOT_TEXT("staying"));
    return true;
}

bool FleeChatShortcutAction::Execute(Event& event)
{
    Player* master = GetMaster();
    if (!master)
        return false;

    ai->Reset();
    ai->ChangeStrategy("+follow,+passive", BotState::BOT_STATE_NON_COMBAT);
    ai->ChangeStrategy("+follow,+passive", BotState::BOT_STATE_COMBAT);
    ResetReturnPosition();
    if (bot->GetMapId() != master->GetMapId() || sServerFacade.GetDistance2d(bot, master) > sPlayerbotAIConfig.sightDistance)
    {
        ai->TellError(BOT_TEXT("fleeing_far"));
        return true;
    }
    ai->TellError(BOT_TEXT("fleeing"));
    return true;
}

bool GoawayChatShortcutAction::Execute(Event& event)
{
    Player* master = GetMaster();
    if (!master)
        return false;

    ai->Reset();
    ai->ChangeStrategy("+runaway", BotState::BOT_STATE_NON_COMBAT);
    ai->ChangeStrategy("+runaway", BotState::BOT_STATE_COMBAT);
    ResetReturnPosition();
    ai->TellError("Running away");
    return true;
}

bool GrindChatShortcutAction::Execute(Event& event)
{
    Player* master = GetMaster();
    if (!master)
        return false;

    ai->Reset();
    ai->ChangeStrategy("+grind,-passive", BotState::BOT_STATE_NON_COMBAT);
    ResetReturnPosition();
    ai->TellError(BOT_TEXT("grinding"));
    return true;
}

bool TankAttackChatShortcutAction::Execute(Event& event)
{
    Player* master = GetMaster();
    if (!master)
        return false;

    if (!ai->IsTank(bot))
        return false;

    ai->Reset();
    ai->ChangeStrategy("-passive", BotState::BOT_STATE_NON_COMBAT);
    ai->ChangeStrategy("-passive", BotState::BOT_STATE_COMBAT);
    ResetReturnPosition();
    ai->TellError(BOT_TEXT("attacking"));
    return true;
}

bool MaxDpsChatShortcutAction::Execute(Event& event)
{
    Player* master = GetMaster();
    if (!master)
        return false;

    if (!ai->ContainsStrategy(STRATEGY_TYPE_DPS))
        return false;

    ai->Reset();
    ai->ChangeStrategy("-threat,-conserve mana,-cast time,+dps debuff,+boost", BotState::BOT_STATE_COMBAT);
    ai->TellError("Max DPS!");
    return true;
}
