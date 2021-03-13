#pragma once
#include "../triggers/GenericTriggers.h"

namespace ai
{

    class KickInterruptSpellTrigger : public InterruptSpellTrigger
    {
    public:
        KickInterruptSpellTrigger(PlayerbotAI* ai) : InterruptSpellTrigger(ai, "kick") {}
    };

    class SliceAndDiceTrigger : public BuffTrigger
    {
    public:
        SliceAndDiceTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "slice and dice") {}
    };

    class AdrenalineRushTrigger : public BuffTrigger
    {
    public:
        AdrenalineRushTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "adrenaline rush") {}
        virtual bool IsPossible()
        {
            return !ai->HasAura("stealth", bot);
        }
    };

    class RuptureTrigger : public DebuffTrigger
    {
    public:
        RuptureTrigger(PlayerbotAI* ai) : DebuffTrigger(ai, "rupture") {}
    };

    class ExposeArmorTrigger : public DebuffTrigger
    {
    public:
        ExposeArmorTrigger(PlayerbotAI* ai) : DebuffTrigger(ai, "expose armor") {}
    };

    class KickInterruptEnemyHealerSpellTrigger : public InterruptEnemyHealerTrigger
    {
    public:
        KickInterruptEnemyHealerSpellTrigger(PlayerbotAI* ai) : InterruptEnemyHealerTrigger(ai, "kick") {}
    };

    class InStealthTrigger : public HasAuraTrigger
    {
    public:
        InStealthTrigger(PlayerbotAI* ai) : HasAuraTrigger(ai, "stealth") {}
    };

    class NoStealthTrigger : public HasNoAuraTrigger
    {
    public:
        NoStealthTrigger(PlayerbotAI* ai) : HasNoAuraTrigger(ai, "stealth") {}
    };

    class UnstealthTrigger : public BuffTrigger
    {
    public:
        UnstealthTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "stealth", 3) {}
        virtual bool IsActive()
        {
            if (!ai->HasAura("stealth", bot))
                return false;

            return ai->HasAura("stealth", bot) &&
                !AI_VALUE(uint8, "attacker count") &&
                (AI_VALUE2(bool, "moving", "self target") &&
                ((ai->GetMaster() &&
                    sServerFacade.IsDistanceGreaterThan(AI_VALUE2(float, "distance", "master target"), 10.0f) &&
                    AI_VALUE2(bool, "moving", "master target")) ||
                    !AI_VALUE(uint8, "attacker count")));
        }
    };

    class StealthTrigger : public Trigger {
    public:
        StealthTrigger(PlayerbotAI* ai) : Trigger(ai, "stealth") {}
        virtual bool IsActive()
        {
            if (ai->HasAura("stealth", bot) || sServerFacade.IsInCombat(bot) || !sServerFacade.IsSpellReady(bot, 1784))
                return false;

            float distance = 30.0f;

            Unit* target = AI_VALUE(Unit*, "enemy player target");
            if (!target)
                target = AI_VALUE(Unit*, "grind target");
            if (!target)
                target = AI_VALUE(Unit*, "dps target");
            if (!target)
                return false;

#ifdef MANGOS
            if (target && target->getVictim())
                distance -= 10;

            if (sServerFacade.isMoving(target) && target->getVictim())
                distance -= 10;
#endif
#ifdef CMANGOS
            if (target && target->GetVictim())
                distance -= 10;

            if (sServerFacade.isMoving(target) && target->GetVictim())
                distance -= 10;
#endif

            if (bot->InBattleGround())
                distance += 15;

#ifndef MANGOSBOT_ZERO
            if (bot->InArena())
                distance += 15;
#endif

            return (target &&
                sServerFacade.GetDistance2d(bot, target) < distance);
        }
    };

    class SapTrigger : public HasCcTargetTrigger
    {
    public:
        SapTrigger(PlayerbotAI* ai) : HasCcTargetTrigger(ai, "sap") {}
        virtual bool IsPossible()
        {
            return bot->getLevel() > 10 && bot->HasSpell(6770) && !sServerFacade.IsInCombat(bot);
        }
    };

    class SprintTrigger : public BuffTrigger
    {
    public:
        SprintTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "sprint", 3) {}
        virtual bool IsPossible()
        {
            return bot->HasSpell(2983);
        }
        virtual bool IsActive()
        {
            if (!sServerFacade.IsSpellReady(bot, 2983))
                return false;

            float distance = ai->GetMaster() ? 45.0f : 35.0f;
            if (ai->HasAura("stealth", bot))
                distance -= 10;

            bool targeted = false;

            Unit* dps = AI_VALUE(Unit*, "dps target");
            Unit* enemyPlayer = AI_VALUE(Unit*, "enemy player target");
            if (dps)
                targeted = (dps == AI_VALUE(Unit*, "current target"));
            if (enemyPlayer && !targeted)
                targeted = (enemyPlayer == AI_VALUE(Unit*, "current target"));

            if (!targeted)
                return false;

            if ((dps && sServerFacade.IsInCombat(dps)) || (enemyPlayer))
                distance -= 10;

            return  AI_VALUE2(bool, "moving", "self target") &&
                    (AI_VALUE2(bool, "moving", "dps target") ||
                    AI_VALUE2(bool, "moving", "enemy player target")) &&
                    targeted &&
                    (sServerFacade.IsDistanceGreaterThan(AI_VALUE2(float, "distance", "dps target"), distance) ||
                     sServerFacade.IsDistanceGreaterThan(AI_VALUE2(float, "distance", "enemy player target"), distance));
        }
    };
}
