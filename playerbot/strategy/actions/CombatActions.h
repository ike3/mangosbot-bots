#pragma once

#include "../Action.h"
#include "ChangeStrategyAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"

namespace ai
{
    class SwitchToMeleeAction : public ChangeCombatStrategyAction
    {
    public:
        SwitchToMeleeAction(PlayerbotAI* ai) : ChangeCombatStrategyAction(ai, "-ranged,+close") {}

        virtual bool Execute(Event event)
        {
            //ai->TellMasterNoFacing("Switching to melee!");
            return ChangeCombatStrategyAction::Execute(event);
        }

        virtual bool isUseful()
        {
            if (bot->getClass() == CLASS_HUNTER)
            {
                Unit* target = AI_VALUE(Unit*, "current target");
                time_t lastFlee = AI_VALUE(LastMovement&, "last movement").lastFlee;
                return ai->HasStrategy("ranged", BOT_STATE_COMBAT) && ((sServerFacade.IsInCombat(bot) && target && (target->GetVictim() == bot && (!bot->GetGroup() || lastFlee) &&
                    sServerFacade.IsDistanceLessOrEqualThan(AI_VALUE2(float, "distance", "current target"), 8.0f))) ||
                    (!sServerFacade.IsInCombat(bot)));
            }
            return ai->HasStrategy("ranged", BOT_STATE_COMBAT);
        }
    };

    class SwitchToRangedAction : public ChangeCombatStrategyAction
    {
    public:
        SwitchToRangedAction(PlayerbotAI* ai) : ChangeCombatStrategyAction(ai, "-close,+ranged") {}

        virtual bool Execute(Event event)
        {
            //ai->TellMasterNoFacing("Switching to ranged!");
            return ChangeCombatStrategyAction::Execute(event);
        }

        virtual bool isUseful()
        {
            if (bot->getClass() == CLASS_HUNTER)
            {
                Unit* target = AI_VALUE(Unit*, "current target");
                bool hasAmmo = AI_VALUE2(uint32, "item count", "ammo");
                return ai->HasStrategy("close", BOT_STATE_COMBAT) && hasAmmo && ((sServerFacade.IsInCombat(bot) && target && ((target->GetVictim() != bot || target->GetTarget() != bot) ||
                    sServerFacade.IsDistanceGreaterThan(AI_VALUE2(float, "distance", "current target"), 8.0f))) ||
                    (!sServerFacade.IsInCombat(bot)));
            }
            return ai->HasStrategy("close", BOT_STATE_COMBAT);
        }
    };
}
