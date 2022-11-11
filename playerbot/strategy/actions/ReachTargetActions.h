#pragma once

#include "../Action.h"
#include "MovementActions.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"

namespace ai
{
    class ReachTargetAction : public MovementAction
    {
    public:
        ReachTargetAction(PlayerbotAI* ai, string name, float distance) : MovementAction(ai, name), distance(distance) {}

        virtual bool Execute(Event& event)
		{
            Unit* target = AI_VALUE(Unit*, GetTargetName());
            if (target)
            {
                UpdateMovementState();

                // Ignore movement if too far
                const float distanceToTarget = sServerFacade.GetDistance2d(bot, target);
                if (distanceToTarget <= sPlayerbotAIConfig.sightDistance)
                {
                    float chaseDist = distance;
                    const bool inLos = bot->IsWithinLOSInMap(target, true);
                    const bool isFriend = sServerFacade.IsFriendlyTo(bot, target);

                    if (distance > 0.0f)
                    {
                        const float coeff = isFriend ? 0.8f : 1.0f;
                        chaseDist = inLos ? distance : (isFriend ? std::min(distanceToTarget * 0.9f, distance) : distance);
                        chaseDist = (chaseDist - sPlayerbotAIConfig.contactDistance) * coeff;
                    }

                    if (inLos && isFriend && (distance <= sPlayerbotAIConfig.followDistance))
                    {
                        return MoveNear(target, chaseDist);
                    }
                    else
                    {
                        return ChaseTo(target, chaseDist, bot->GetAngle(target));
                    }
                }
            }

            return false;
        }

        virtual bool isUseful()
		{
            // Do not move while casting
            if (bot->IsNonMeleeSpellCasted(true))
                return false;

            // Do not move if stay strategy is set
            if(ai->HasStrategy("stay", BotState::BOT_STATE_NON_COMBAT))
                return false;

            return true;

            //Unit* target = AI_VALUE(Unit*, GetTargetName());
            //return target && (!bot->IsWithinDistInMap(target, distance) || (bot->IsWithinDistInMap(target, distance) && !bot->IsWithinLOSInMap(target, true)));
        }

        virtual string GetTargetName() { return "current target"; }

    protected:
        float distance;
    };

    class CastReachTargetSpellAction : public CastSpellAction
    {
    public:
        CastReachTargetSpellAction(PlayerbotAI* ai, string spell, float distance) : CastSpellAction(ai, spell), distance(distance) {}

		virtual bool isUseful()
		{
            // Do not move if stay strategy is set
            if (ai->HasStrategy("stay", BotState::BOT_STATE_NON_COMBAT))
                return false;

			return sServerFacade.IsDistanceGreaterThan(AI_VALUE2(float, "distance", "current target"), (distance + sPlayerbotAIConfig.contactDistance));
		}

    protected:
        float distance;
    };

    class ReachMeleeAction : public ReachTargetAction
	{
    public:
        ReachMeleeAction(PlayerbotAI* ai) : ReachTargetAction(ai, "reach melee", 0.0f) {}
    };

    class ReachSpellAction : public ReachTargetAction
	{
    public:
        ReachSpellAction(PlayerbotAI* ai) : ReachTargetAction(ai, "reach spell", ai->GetRange("spell")) {}
    };

    class ReachPartyMemberToHealAction : public ReachTargetAction
    {
    public:
        ReachPartyMemberToHealAction(PlayerbotAI* ai) : ReachTargetAction(ai, "reach party member to heal", ai->GetRange("heal")) {}
        virtual string GetTargetName() { return "party member to heal"; }
    };
}
