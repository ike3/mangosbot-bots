#pragma once

#include "../triggers/GenericTriggers.h"

namespace ai
{
    DEBUFF_TRIGGER(HolyFireTrigger, "holy fire");
    DEBUFF_TRIGGER(PowerWordPainTrigger, "shadow word: pain");
    DEBUFF_ENEMY_TRIGGER(PowerWordPainOnAttackerTrigger, "shadow word: pain");
    DEBUFF_TRIGGER(VampiricTouchTrigger, "vampiric touch");
    DEBUFF_TRIGGER(VampiricEmbraceTrigger, "vampiric embrace");
    CURE_TRIGGER(DispelMagicTrigger, "dispel magic", DISPEL_MAGIC);
    CURE_PARTY_TRIGGER(DispelMagicPartyMemberTrigger, "dispel magic", DISPEL_MAGIC);
    CURE_TRIGGER(CureDiseaseTrigger, "cure disease", DISPEL_DISEASE);
    CURE_PARTY_TRIGGER(PartyMemberCureDiseaseTrigger, "cure disease", DISPEL_DISEASE);
    BUFF_TRIGGER_A(InnerFireTrigger, "inner fire");
    BUFF_TRIGGER_A(ShadowformTrigger, "shadowform");
    BUFF_TRIGGER(PowerInfusionTrigger, "power infusion");
    BUFF_TRIGGER(InnerFocusTrigger, "inner focus");
    CC_TRIGGER(ShackleUndeadTrigger, "shackle undead");
    INTERRUPT_TRIGGER(SilenceTrigger, "silence");
    INTERRUPT_HEALER_TRIGGER(SilenceEnemyHealerTrigger, "silence");

    // racials
    DEBUFF_TRIGGER(DevouringPlagueTrigger, "devouring plague");
    BUFF_TRIGGER(TouchOfWeaknessTrigger, "touch of weakness");
    DEBUFF_TRIGGER(HexOfWeaknessTrigger, "hex of weakness");
    BUFF_TRIGGER(ShadowguardTrigger, "shadowguard");
    BUFF_TRIGGER(FearWardTrigger, "fear ward");
    DEFLECT_TRIGGER(FeedbackTrigger, "feedback");
    SNARE_TRIGGER(ChastiseTrigger, "chastise");

    BOOST_TRIGGER_A(ShadowfiendTrigger, "shadowfiend");

    class PowerWordFortitudeOnPartyTrigger : public BuffOnPartyTrigger {
    public:
        PowerWordFortitudeOnPartyTrigger(PlayerbotAI* ai) : BuffOnPartyTrigger(ai, "power word: fortitude", 4) {}

        virtual bool IsActive() { return BuffOnPartyTrigger::IsActive() && !ai->HasAura("power word: fortitude", GetTarget()) && !ai->HasAura("prayer of fortitude", GetTarget()); }
    };

    class PowerWordFortitudeTrigger : public BuffTrigger {
    public:
        PowerWordFortitudeTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "power word: fortitude", 4) {}

        virtual bool IsActive() { return BuffTrigger::IsActive() && !ai->HasAura("power word: fortitude", GetTarget()) && !ai->HasAura("prayer of fortitude", GetTarget()); }
    };

    class DivineSpiritOnPartyTrigger : public BuffOnPartyTrigger {
    public:
        DivineSpiritOnPartyTrigger(PlayerbotAI* ai) : BuffOnPartyTrigger(ai, "divine spirit", 4) {}

        virtual bool IsActive() { return BuffOnPartyTrigger::IsActive() && !ai->HasAura("divine spirit", GetTarget()) && !ai->HasAura("prayer of spirit", GetTarget()); }
    };

    class DivineSpiritTrigger : public BuffTrigger {
    public:
        DivineSpiritTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "divine spirit", 4) {}

        virtual bool IsActive() { return BuffTrigger::IsActive() && !ai->HasAura("divine spirit", GetTarget()) && !ai->HasAura("prayer of shadow protection", GetTarget()); }
    };

    class ShadowProtectionOnPartyTrigger : public BuffOnPartyTrigger {
    public:
        ShadowProtectionOnPartyTrigger(PlayerbotAI* ai) : BuffOnPartyTrigger(ai, "shadow protection", 4) {}

        virtual bool IsActive() { return BuffOnPartyTrigger::IsActive() && !ai->HasAura("shadow protection", GetTarget()) && !ai->HasAura("prayer of shadow protection", GetTarget()); }
    };

    class ShadowProtectionTrigger : public BuffTrigger {
    public:
        ShadowProtectionTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "shadow protection", 4) {}

        virtual bool IsActive() { return BuffTrigger::IsActive() && !ai->HasAura("shadow protection", GetTarget()) && !ai->HasAura("prayer of spirit", GetTarget()); }
    };

    class PrayerOfFortitudeTrigger : public BuffOnPartyTrigger {
    public:
        PrayerOfFortitudeTrigger(PlayerbotAI* ai) : BuffOnPartyTrigger(ai, "prayer of fortitude", 3) {}

        virtual bool IsActive() { return bot->GetGroup() && BuffOnPartyTrigger::IsActive() &&
            !ai->HasAura("prayer of fortitude", GetTarget()) &&
#ifdef MANGOS
            (ai->GetBot()->IsInSameGroupWith((Player*)GetTarget()) || ai->GetBot()->IsInSameRaidWith((Player*)GetTarget())) &&
#endif
#ifdef CMANGOS
            bot->IsInGroup((Player*)GetTarget()) &&
#endif
            (ai->GetBuffedCount((Player*)GetTarget(), "prayer of fortitude") + ai->GetBuffedCount((Player*)GetTarget(), "power word: fortitude")) < 4;
            ; }
    };

    class PrayerOfSpiritTrigger : public BuffOnPartyTrigger {
    public:
        PrayerOfSpiritTrigger(PlayerbotAI* ai) : BuffOnPartyTrigger(ai, "prayer of spirit", 3) {}

        virtual bool IsActive() { return bot->GetGroup() && BuffOnPartyTrigger::IsActive() &&
            !ai->HasAura("prayer of spirit", GetTarget()) &&
#ifdef MANGOS
            (ai->GetBot()->IsInSameGroupWith((Player*)GetTarget()) || ai->GetBot()->IsInSameRaidWith((Player*)GetTarget())) &&
#endif
#ifdef CMANGOS
            bot->IsInGroup((Player*)GetTarget()) &&
#endif
            //ai->GetManaPercent() > 50 &&
            (ai->GetBuffedCount((Player*)GetTarget(), "prayer of spirit") + ai->GetBuffedCount((Player*)GetTarget(), "divine spirit")) < 4;
            ; }
    };

    class PrayerOfShadowProtectionTrigger : public BuffOnPartyTrigger {
    public:
        PrayerOfShadowProtectionTrigger(PlayerbotAI* ai) : BuffOnPartyTrigger(ai, "prayer of shadow protection", 3) {}

        virtual bool IsActive() {
            return bot->GetGroup() && BuffOnPartyTrigger::IsActive() &&
                !ai->HasAura("prayer of shadow protection", GetTarget()) &&
#ifdef MANGOS
                (ai->GetBot()->IsInSameGroupWith((Player*)GetTarget()) || ai->GetBot()->IsInSameRaidWith((Player*)GetTarget())) &&
#endif
#ifdef CMANGOS
                bot->IsInGroup((Player*)GetTarget()) &&
#endif
                (ai->GetBuffedCount((Player*)GetTarget(), "prayer of shadow protection") + ai->GetBuffedCount((Player*)GetTarget(), "shadow protection")) < 4;
            ;
        }
    };

    class BindingHealTrigger : public PartyMemberLowHealthTrigger {
    public:
        BindingHealTrigger(PlayerbotAI* ai) : PartyMemberLowHealthTrigger(ai, "binding heal", sPlayerbotAIConfig.lowHealth, 0) {}
        virtual bool IsActive()
        {
            return PartyMemberLowHealthTrigger::IsActive() && AI_VALUE2(uint8, "health", "self target") < sPlayerbotAIConfig.mediumHealth;
        }
    };
}
