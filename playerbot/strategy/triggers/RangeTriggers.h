#pragma once
#include "../Trigger.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"

namespace ai
{
    class EnemyTooCloseForSpellTrigger : public Trigger {
    public:
        EnemyTooCloseForSpellTrigger(PlayerbotAI* ai) : Trigger(ai, "enemy too close for spell") {}
        virtual bool IsActive()
		{
			Unit* target = AI_VALUE(Unit*, "current target");
            if (target)
            {
                float targetDistance = sServerFacade.GetDistance2d(bot, target);
                return sServerFacade.IsDistanceLessOrEqualThan(targetDistance, (ai->GetRange("spell") / 2));
            }
            return false;
        }
    };

    class EnemyTooCloseForShootTrigger : public Trigger {
    public:
        EnemyTooCloseForShootTrigger(PlayerbotAI* ai) : Trigger(ai, "enemy too close for shoot") {}
        virtual bool IsActive()
		{
			Unit* target = AI_VALUE(Unit*, "current target");
            return target &&
                sServerFacade.IsDistanceLessOrEqualThan(AI_VALUE2(float, "distance", "current target"), (ai->GetRange("shoot") / 2));
        }
    };

    class EnemyTooCloseForMeleeTrigger : public Trigger {
    public:
        EnemyTooCloseForMeleeTrigger(PlayerbotAI* ai) : Trigger(ai, "enemy too close for melee", 5) {}
        virtual bool IsActive()
		{
			Unit* target = AI_VALUE(Unit*, "current target");
            if (target && target->IsPlayer())
                return false;

            return target && AI_VALUE2(bool, "inside target", "current target");
        }
    };

    class EnemyIsCloseTrigger : public Trigger {
    public:
        EnemyIsCloseTrigger(PlayerbotAI* ai) : Trigger(ai, "enemy is close") {}
        virtual bool IsActive()
		{
			Unit* target = AI_VALUE(Unit*, "current target");
            return target &&
                    sServerFacade.IsDistanceLessOrEqualThan(AI_VALUE2(float, "distance", "current target"), sPlayerbotAIConfig.tooCloseDistance);
        }
    };

    class OutOfRangeTrigger : public Trigger {
    public:
        OutOfRangeTrigger(PlayerbotAI* ai, string name, float distance) : Trigger(ai, name)
		{
            this->distance = distance;
        }
        virtual bool IsActive()
		{
			Unit* target = AI_VALUE(Unit*, GetTargetName());
			return target &&
			        sServerFacade.IsDistanceGreaterThan(AI_VALUE2(float, "distance", GetTargetName()), distance);
        }
        virtual string GetTargetName() { return "current target"; }

    protected:
        float distance;
    };

    class EnemyOutOfMeleeTrigger : public OutOfRangeTrigger
	{
    public:
        EnemyOutOfMeleeTrigger(PlayerbotAI* ai) : OutOfRangeTrigger(ai, "enemy out of melee range", sPlayerbotAIConfig.meleeDistance) {}
        virtual bool IsActive()
        {
            Unit* target = AI_VALUE(Unit*, GetTargetName());
            if (!target)
                return false;

            float targetDistance = sServerFacade.GetDistance2d(bot, target);
            return target && (targetDistance > max(5.0f, bot->GetCombinedCombatReach(target, true)) || (!bot->IsWithinLOSInMap(target) && targetDistance > 5.0f));
        }
    };

    class EnemyOutOfSpellRangeTrigger : public OutOfRangeTrigger
	{
    public:
        EnemyOutOfSpellRangeTrigger(PlayerbotAI* ai) : OutOfRangeTrigger(ai, "enemy out of spell range", ai->GetRange("spell")) {}
        virtual bool IsActive()
        {
            Unit* target = AI_VALUE(Unit*, GetTargetName());
            return target && (sServerFacade.GetDistance2d(bot, target) > distance || !bot->IsWithinLOSInMap(target));
        }
    };

    class PartyMemberToHealOutOfSpellRangeTrigger : public OutOfRangeTrigger
	{
    public:
        PartyMemberToHealOutOfSpellRangeTrigger(PlayerbotAI* ai) : OutOfRangeTrigger(ai, "party member to heal out of spell range", ai->GetRange("heal")) {}
        virtual string GetTargetName() { return "party member to heal"; }
        virtual bool IsActive()
        {
            Unit* target = AI_VALUE(Unit*, GetTargetName());
            return target && (sServerFacade.GetDistance2d(bot, target) > distance || !bot->IsWithinLOSInMap(target));
        }
    };

    class FarFromMasterTrigger : public Trigger {
    public:
        FarFromMasterTrigger(PlayerbotAI* ai, string name = "far from master", float distance = 12.0f, int checkInterval = 50) : Trigger(ai, name, checkInterval), distance(distance) {}

        virtual bool IsActive()
        {
            return sServerFacade.IsDistanceGreaterThan(AI_VALUE2(float, "distance", "master target"), distance);
        }

    private:
        float distance;
    };

    class OutOfReactRangeTrigger : public FarFromMasterTrigger
    {
    public:
        OutOfReactRangeTrigger(PlayerbotAI* ai) : FarFromMasterTrigger(ai, "out of react range", 50.0f, 5) {}
    };

    class HearthIsFasterTrigger : public Trigger
    {
    public:
        HearthIsFasterTrigger(PlayerbotAI* ai) : Trigger(ai, "hearth is faster", 5) {}

        virtual bool IsActive()
        {
            if (!bot->HasItemCount(6948, 1, false))
                return false;

            if (!sServerFacade.IsSpellReady(bot, 8690))
                return false;

            WorldPosition longMove = AI_VALUE(WorldPosition, "last long move");

            if (!longMove)
                return false;

            WorldPosition hearthBind = AI_VALUE(WorldPosition, "home bind");

            return AI_VALUE2(float, "distance", "last long move") > hearthBind.distance(longMove) + 200;
        }
    };
}
