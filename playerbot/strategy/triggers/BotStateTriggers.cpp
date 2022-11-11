#include "botpch.h"
#include "../../playerbot.h"
#include "../../ServerFacade.h"
#include "BotStateTriggers.h"

bool CombatStartTrigger::IsActive()
{
    if (!ai->IsStateActive(BotState::BOT_STATE_COMBAT) && !ai->IsStateActive(BotState::BOT_STATE_DEAD))
    {
        // Check if any member of the group (near this bot) is getting attacked
        // NOTE: The validity of the attackers and the group members get checked in AttackersValue::Calculate()
        return !ai->GetAiObjectContext()->GetValue<list<ObjectGuid>>("combat targets")->Get().empty();
    }

    return false;
}

bool CombatEndTrigger::IsActive()
{
    // Check if the bot is currently in combat
    if (ai->IsStateActive(BotState::BOT_STATE_COMBAT))
    {
        // Check if any member of the group (near this bot) is getting attacked
        // NOTE: The validity of the attackers and the group members get checked in AttackersValue::Calculate()
        return ai->GetAiObjectContext()->GetValue<list<ObjectGuid>>("combat targets")->Get().empty();
    }

    return false;
}

bool DeathTrigger::IsActive()
{
    return !ai->IsStateActive(BotState::BOT_STATE_DEAD) && !sServerFacade.IsAlive(bot);
}

bool ai::ResurrectTrigger::IsActive()
{
    return ai->IsStateActive(BotState::BOT_STATE_DEAD) && sServerFacade.IsAlive(bot);
}