#pragma once
#include "../Trigger.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"
#include <MotionGenerators/MoveMap.h>

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
                if (target->GetTarget() == bot && !bot->GetGroup() && !target->IsRooted() && target->GetSpeedInMotion() > bot->GetSpeedInMotion() * 0.65)
                    return false;

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
            if (!target)
                return false;

            if (target->GetTarget() == bot && !bot->GetGroup() && !target->IsRooted() && target->GetSpeedInMotion() > bot->GetSpeedInMotion() * 0.65)
                return false;

            return sServerFacade.IsDistanceLessOrEqualThan(AI_VALUE2(float, "distance", "current target"), (ai->GetRange("shoot") / 2));
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
            return target && (targetDistance > max(5.0f, bot->GetCombinedCombatReach(target, true)) || (!bot->IsWithinLOSInMap(target, true) && targetDistance > 5.0f));
        }
    };

    class EnemyOutOfSpellRangeTrigger : public OutOfRangeTrigger
	{
    public:
        EnemyOutOfSpellRangeTrigger(PlayerbotAI* ai) : OutOfRangeTrigger(ai, "enemy out of spell range", ai->GetRange("spell")) {}
        virtual bool IsActive()
        {
            Unit* target = AI_VALUE(Unit*, GetTargetName());
            return target && (sServerFacade.GetDistance2d(bot, target) > distance || !bot->IsWithinLOSInMap(target, true));
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
            return target && (sServerFacade.GetDistance2d(bot, target) > distance || !bot->IsWithinLOSInMap(target, true));
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

    class MovementStuckTrigger : public Trigger
    {
    public:
        MovementStuckTrigger(PlayerbotAI* ai) : Trigger(ai, "movement stuck", 5) {}

        virtual bool IsActive()
        {
            if (ai->HasActivePlayerMaster())
                return false;

            if (!ai->AllowActive(ALL_ACTIVITY))
                return false;

            WorldPosition botPos(bot);            

            LogCalculatedValue<WorldPosition>* posVal = dynamic_cast<LogCalculatedValue<WorldPosition>*>(context->GetUntypedValue("current position"));

            if (posVal->LastChangeDelay() > 5 * MINUTE)
            {
                sLog.outBasic("Bot #%d %s:%d <%s> was in the same position for %d seconds", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), posVal->LastChangeDelay());

                return true;
            }

            bool longLog = false;

            for (auto tPos : posVal->ValueLog())
            {
                uint32 timePassed = time(0) - tPos.second;

                if (timePassed > 10 * MINUTE)
                {
                    if (botPos.fDist(tPos.first) > 50.0f)
                        return false;

                    longLog = true;
                }
            }

            if (longLog)
            {
                sLog.outBasic("Bot #%d %s:%d <%s> was in the same position for 10mins", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), posVal->LastChangeDelay());
            }

            return longLog;
        }
    };

    class LocationStuckTrigger : public Trigger
    {
    public:
        LocationStuckTrigger(PlayerbotAI* ai) : Trigger(ai, "location stuck", 5) {}

        virtual bool IsActive()
        {
            if (ai->HasActivePlayerMaster())
                return false;

            if (!ai->AllowActive(ALL_ACTIVITY))
                return false;

            WorldPosition botPos(bot);

            Cell const& cell = bot->GetCurrentCell();

            GridPair grid = botPos.getGridPair();

            if (grid.x_coord < 0 || grid.x_coord >= MAX_NUMBER_OF_GRIDS)
            {
                sLog.outBasic("Bot #%d %s:%d <%s> was in grid %d,%d on map %d", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), grid.x_coord, grid.y_coord, botPos.getMapId());

                return true;
            }

            if (grid.y_coord < 0 || grid.y_coord >= MAX_NUMBER_OF_GRIDS)
            {
                sLog.outBasic("Bot #%d %s:%d <%s> was in grid %d,%d on map %d", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), grid.x_coord, grid.y_coord, botPos.getMapId());

                return true;
            }

            if (cell.GridX() > 0 && cell.GridY() > 0 && !MMAP::MMapFactory::createOrGetMMapManager()->IsMMapIsLoaded(botPos.getMapId(), cell.GridX(), cell.GridY()) && !MMAP::MMapFactory::createOrGetMMapManager()->loadMap(botPos.getMapId(), cell.GridX(), cell.GridY()))
            {
                sLog.outBasic("Bot #%d %s:%d <%s> was in unloaded grid %d,%d on map %d", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), grid.x_coord, grid.y_coord, botPos.getMapId());

                return true;
            }

            LogCalculatedValue<WorldPosition>* posVal = dynamic_cast<LogCalculatedValue<WorldPosition>*>(context->GetUntypedValue("current position"));

            if (posVal->LastChangeDelay() > 10 * MINUTE)
            {
                sLog.outBasic("Bot #%d %s:%d <%s> was in the same position for %d seconds", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), posVal->LastChangeDelay());

                posVal->Reset();
                return true;
            }

            MemoryCalculatedValue<uint32>* expVal = dynamic_cast<MemoryCalculatedValue<uint32>*>(context->GetUntypedValue("experience"));

            if (expVal->LastChangeDelay() < 15 * MINUTE)
                return false;            

            bool longLog = false;

            for (auto tPos : posVal->ValueLog())
            {
                uint32 timePassed = time(0) - tPos.second;

                if (timePassed > 15 * MINUTE)
                {
                    if (botPos.fDist(tPos.first) > 50.0f)
                        return false;

                    longLog = true;
                }
            }

            if (longLog)
            {
                posVal->Reset();
                sLog.outBasic("Bot #%d %s:%d <%s> was in the same position for 15mins", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), posVal->LastChangeDelay());
            }

            return longLog;
        }
    };
}
