#pragma once

#include "../triggers/GenericTriggers.h"

namespace ai
{
    HAS_AURA_TRIGGER_TIME(FeignDeathTrigger, "feign death", 2);

    BEGIN_TRIGGER(HunterNoStingsActiveTrigger, Trigger)
    END_TRIGGER()

    class HunterAspectOfTheHawkTrigger : public BuffTrigger
    {
    public:
        HunterAspectOfTheHawkTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "aspect of the hawk") 
        {
            checkInterval = 1;
        }

        virtual bool IsActive()
        {
            if (!BuffTrigger::IsActive())
                return false;

            Unit* target = AI_VALUE(Unit*, "current target");
            return target && !sServerFacade.IsDistanceLessOrEqualThan(AI_VALUE2(float, "distance", "current target"), 5.0);
        }
    };

    class HunterAspectOfTheWildTrigger : public BuffTrigger
    {
    public:
        HunterAspectOfTheWildTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "aspect of the wild") 
        {
            checkInterval = 1;
        }
    };

    class HunterAspectOfTheViperTrigger : public BuffTrigger
    {
    public:
        HunterAspectOfTheViperTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "aspect of the viper") {}

        virtual bool IsActive()
        {
            return SpellTrigger::IsActive() && !ai->HasAura(spell, GetTarget());
        }
    };

    class HunterAspectOfThePackTrigger : public BuffTrigger
    {
    public:
        HunterAspectOfThePackTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "aspect of the pack") {}

        virtual bool IsActive()
        {
            return BuffTrigger::IsActive() && !ai->HasAura("aspect of the cheetah", GetTarget());
        };
    };

    BEGIN_TRIGGER(HuntersPetDeadTrigger, Trigger)
    END_TRIGGER()

    BEGIN_TRIGGER(HuntersPetLowHealthTrigger, Trigger)
    END_TRIGGER()

    class BlackArrowTrigger : public DebuffTrigger
    {
    public:
        BlackArrowTrigger(PlayerbotAI* ai) : DebuffTrigger(ai, "black arrow") {}
    };

    SNARE_TRIGGER(BlackArrowSnareTrigger, "black arrow");

    class HuntersMarkTrigger : public DebuffTrigger
    {
    public:
        HuntersMarkTrigger(PlayerbotAI* ai) : DebuffTrigger(ai, "hunter's mark") {}
    };

    class FreezingTrapTrigger : public HasCcTargetTrigger
    {
    public:
        FreezingTrapTrigger(PlayerbotAI* ai) : HasCcTargetTrigger(ai, "freezing trap") {}

#ifdef MANGOSBOT_ZERO
        bool IsActive() override
        {
            // Check if feign death not on cooldown
            if (sServerFacade.IsSpellReady(bot, 5384))
            {
                return HasCcTargetTrigger::IsActive();
            }

            return false;
        }
#endif
    };

    class FrostTrapTrigger : public MeleeLightAoeTrigger
    {
    public:
        FrostTrapTrigger(PlayerbotAI* ai, string spell = "frost trap") : MeleeLightAoeTrigger(ai)
        {
            spellId = AI_VALUE2(uint32, "spell id", spell);
        }

        bool IsActive() override
        {
#ifdef MANGOSBOT_ZERO
            // Check if feign death not on cooldown
            if (!sServerFacade.IsSpellReady(bot, 5384))
            {
                return false;
            }
#endif

            return sServerFacade.IsSpellReady(bot, spellId) && MeleeLightAoeTrigger::IsActive();
        }

    private:
        uint32 spellId;
    };

    class ExplosiveTrapTrigger : public RangedMediumAoeTrigger
    {
    public:
        ExplosiveTrapTrigger(PlayerbotAI* ai, string spell = "explosive trap") : RangedMediumAoeTrigger(ai)
        {
            spellId = AI_VALUE2(uint32, "spell id", spell);
        }

        bool IsActive() override
        {
#ifdef MANGOSBOT_ZERO
            // Check if feign death not on cooldown
            if (!sServerFacade.IsSpellReady(bot, 5384))
            {
                return false;
            }
#endif

            return sServerFacade.IsSpellReady(bot, spellId) && RangedMediumAoeTrigger::IsActive();
        }

    private:
        uint32 spellId;
    };

    class RapidFireTrigger : public BuffTrigger
    {
    public:
        RapidFireTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "rapid fire") {}
    };

    class TrueshotAuraTrigger : public BuffTrigger
    {
    public:
        TrueshotAuraTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "trueshot aura") {}
    };

    class SerpentStingOnAttackerTrigger : public DebuffOnAttackerTrigger
    {
    public:
        SerpentStingOnAttackerTrigger(PlayerbotAI* ai) : DebuffOnAttackerTrigger(ai, "serpent sting") {}
        virtual bool IsActive();
    };

    class ViperStingOnAttackerTrigger : public DebuffOnAttackerTrigger
    {
    public:
        ViperStingOnAttackerTrigger(PlayerbotAI* ai) : DebuffOnAttackerTrigger(ai, "viper sting") {}
        virtual bool IsActive();
    };

    BEGIN_TRIGGER(HunterPetNotHappy, Trigger)
    END_TRIGGER()

    class ConsussiveShotSnareTrigger : public SnareTargetTrigger
    {
    public:
        ConsussiveShotSnareTrigger(PlayerbotAI* ai) : SnareTargetTrigger(ai, "concussive shot") {}
    };

    SNARE_TRIGGER(ScatterShotSnareTrigger, "scatter shot");

    class ScareBeastTrigger : public HasCcTargetTrigger
    {
    public:
        ScareBeastTrigger(PlayerbotAI* ai) : HasCcTargetTrigger(ai, "scare beast") {}
    };

    class HunterLowAmmoTrigger : public AmmoCountTrigger
    {
    public:
        HunterLowAmmoTrigger(PlayerbotAI* ai) : AmmoCountTrigger(ai, "ammo", 1, 30) {}
        virtual bool IsActive() { return bot->GetGroup() && (AI_VALUE2(uint32, "item count", "ammo") < 100) && (AI_VALUE2(uint32, "item count", "ammo") > 0); }
    };

    class HunterNoAmmoTrigger : public AmmoCountTrigger
    {
    public:
        HunterNoAmmoTrigger(PlayerbotAI* ai) : AmmoCountTrigger(ai, "ammo", 1, 10) {}
    };

    class HunterHasAmmoTrigger : public AmmoCountTrigger
    {
    public:
        HunterHasAmmoTrigger(PlayerbotAI* ai) : AmmoCountTrigger(ai, "ammo", 1, 10) {}
        virtual bool IsActive() { return !AmmoCountTrigger::IsActive(); }
    };

    class SwitchToRangedTrigger : public Trigger
    {
    public:
        SwitchToRangedTrigger(PlayerbotAI* ai) : Trigger(ai, "switch to ranged", 1) {}

        virtual bool IsActive()
        {
            Unit* target = AI_VALUE(Unit*, "current target");
            return ai->HasStrategy("close", BotState::BOT_STATE_COMBAT) && target && (target->GetVictim() != bot ||
                sServerFacade.IsDistanceGreaterThan(AI_VALUE2(float, "distance", "current target"), 8.0f));
        }
    };

    class SwitchToMeleeTrigger : public Trigger
    {
    public:
        SwitchToMeleeTrigger(PlayerbotAI* ai) : Trigger(ai, "switch to melee", 1) {}

        virtual bool IsActive()
        {
            Unit* target = AI_VALUE(Unit*, "current target");
            return ai->HasStrategy("ranged", BotState::BOT_STATE_COMBAT) && target && (target->GetVictim() == bot ||
                sServerFacade.IsDistanceLessOrEqualThan(AI_VALUE2(float, "distance", "current target"), 8.0f));
        }
    };

    CAN_CAST_TRIGGER(ChimeraShotCanCastTrigger, "chimera shot");
    CAN_CAST_TRIGGER(ExplosiveShotCanCastTrigger, "explosive shot");
    CAN_CAST_TRIGGER(MultishotCanCastTrigger, "multi-shot");
    SNARE_TRIGGER(IntimidationSnareTrigger, "intimidation");
    CAN_CAST_TRIGGER(CounterattackCanCastTrigger, "counterattack");
    SNARE_TRIGGER(WybernStingSnareTrigger, "wyvern sting");
    CAN_CAST_TRIGGER(MongooseBiteCastTrigger, "mongoose bite");
    BOOST_TRIGGER(BestialWrathBoostTrigger, "bestial wrath");

    INTERRUPT_TRIGGER(SilencingShotInterruptTrigger, "silencing shot");
    INTERRUPT_HEALER_TRIGGER(SilencingShotInterruptHealerTrigger, "silencing shot");

    class ViperStingTrigger : public DebuffTrigger
    {
    public:
        ViperStingTrigger(PlayerbotAI* ai) : DebuffTrigger(ai, "viper sting") {}

        virtual bool IsActive()
        {
            return DebuffTrigger::IsActive() && AI_VALUE2(uint8, "mana", "current target") >= 10;
        }
    };

    class AimedShotTrigger : public Trigger
    {
    public:
        AimedShotTrigger(PlayerbotAI* ai) : Trigger(ai, "aimed shot", 2) {}
        virtual string GetTargetName() { return "current target"; }

        virtual bool IsActive()
        {
            if (!bot->HasSpell(19434) || !bot->IsSpellReady(19434))
                return false;

            Unit* target = GetTarget();
            if (!target)
                return false;

            float distanceTo = AI_VALUE2(float, "distance", GetTargetName());
            if (target->GetSelectionGuid() != bot->GetObjectGuid() && sServerFacade.IsDistanceGreaterOrEqualThan(distanceTo, 8.0f))
                return true;

            // victim
            if (target->GetSelectionGuid() == bot->GetObjectGuid())
            {
                if (sServerFacade.IsDistanceGreaterOrEqualThan(distanceTo, 15.0f))
                    return true;
            }
            return false;
        }
    };

    class HunterNoPet : public Trigger 
    {
    public:
        HunterNoPet(PlayerbotAI* ai) : Trigger(ai, "no beast", 1) {}
        virtual bool IsActive()
        {
            Unit* target = AI_VALUE(Unit*, "current target");
            if (target && target->GetCreatureType() == CREATURE_TYPE_BEAST && !bot->GetPetGuid() && target->GetLevel() <= bot->GetLevel()) {
                return true;
            }
            return false;
        }
    };

    class StealthedNearbyTrigger : public Trigger 
    {
    public:
        StealthedNearbyTrigger(PlayerbotAI* ai) : Trigger(ai, "stealthed nearby", 5) {}
        virtual bool IsActive()
        {
            if (!bot->HasSpell(1543))
                return false;

            Unit* target = AI_VALUE(Unit*, "nearest stealthed unit");
            return target;
        }
    };
}
