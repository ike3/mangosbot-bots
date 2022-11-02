#include "botpch.h"
#include "../../playerbot.h"
#include "../../ServerFacade.h"
#include "../../PlayerbotAIConfig.h"
#include "../values/LastMovementValue.h"
#include "CombatActions.h"

using namespace ai;

bool SwitchToMeleeAction::isUseful()
{
    if (bot->getClass() == CLASS_HUNTER)
    {
        Unit* target = AI_VALUE(Unit*, "current target");
        time_t lastFlee = AI_VALUE(LastMovement&, "last movement").lastFlee;
        return ai->HasStrategy("ranged", BotState::BOT_STATE_COMBAT) && ((sServerFacade.IsInCombat(bot) && target && (target->GetVictim() == bot && (!bot->GetGroup() || lastFlee) &&
            sServerFacade.IsDistanceLessOrEqualThan(AI_VALUE2(float, "distance", "current target"), 8.0f))) ||
            (!sServerFacade.IsInCombat(bot)));
    }

    return ai->HasStrategy("ranged", BotState::BOT_STATE_COMBAT);
}

bool SwitchToRangedAction::isUseful()
{
    if (bot->getClass() == CLASS_HUNTER)
    {
        Unit* target = AI_VALUE(Unit*, "current target");
        bool hasAmmo = AI_VALUE2(uint32, "item count", "ammo");
        return ai->HasStrategy("close", BotState::BOT_STATE_COMBAT) && hasAmmo && ((sServerFacade.IsInCombat(bot) && target && ((target->GetVictim() != bot || target->GetTarget() != bot) ||
            sServerFacade.IsDistanceGreaterThan(AI_VALUE2(float, "distance", "current target"), 8.0f))) ||
            (!sServerFacade.IsInCombat(bot)));
    }

    return ai->HasStrategy("close", BotState::BOT_STATE_COMBAT);
}