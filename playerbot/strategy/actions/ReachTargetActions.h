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
        ReachTargetAction(PlayerbotAI* ai, string name, float distance) : MovementAction(ai, name)
		{
            this->distance = distance;
        }
        virtual bool Execute(Event event)
		{
            Unit* target = AI_VALUE(Unit*, GetTargetName());
            if (!target)
                return false;

            UpdateMovementState();

            float combatReach = bot->GetCombinedCombatReach(target, true);

            if (distance == ATTACK_DISTANCE)
            {
                return ChaseTo(target, 0.0f, GetFollowAngle());
            }
            else
            {
                combatReach = bot->GetCombinedCombatReach(target, false);
                bool inLos = bot->IsWithinLOSInMap(target, true);
                bool isFriend = sServerFacade.IsFriendlyTo(bot, target);
                float chaseDist = inLos ? distance : isFriend ? distance / 2 : distance;
                return ChaseTo(target, (chaseDist - sPlayerbotAIConfig.contactDistance) * 0.8, bot->GetAngle(target));
            }
        }
        virtual bool isUseful()
		{
            // do not move while casting
            if (bot->IsNonMeleeSpellCasted(true))
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
        CastReachTargetSpellAction(PlayerbotAI* ai, string spell, float distance) : CastSpellAction(ai, spell)
		{
            this->distance = distance;
        }
		virtual bool isUseful()
		{
			return sServerFacade.IsDistanceGreaterThan(AI_VALUE2(float, "distance", "current target"), (distance + sPlayerbotAIConfig.contactDistance));
		}

    protected:
        float distance;
    };

    class ReachMeleeAction : public ReachTargetAction
	{
    public:
        ReachMeleeAction(PlayerbotAI* ai) : ReachTargetAction(ai, "reach melee", ATTACK_DISTANCE) {}
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
