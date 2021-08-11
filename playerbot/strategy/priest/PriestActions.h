#pragma once
#include "../actions/GenericActions.h"

namespace ai
{
    // disc
    BUFF_ACTION(CastPowerWordFortitudeAction, "power word: fortitude");
    BUFF_PARTY_ACTION(CastPowerWordFortitudeOnPartyAction, "power word: fortitude");
    BUFF_PARTY_ACTION(CastPrayerOfFortitudeOnPartyAction, "prayer of fortitude");
    BUFF_ACTION(CastPowerWordShieldAction, "power word: shield");
    BUFF_PARTY_ACTION(CastPowerWordShieldOnPartyAction, "power word: shield");
    BUFF_ACTION(CastInnerFireAction, "inner fire");

    CURE_ACTION(CastDispelMagicAction, "dispel magic");
    CURE_PARTY_ACTION(CastDispelMagicOnPartyAction, "dispel magic", DISPEL_MAGIC);
    SPELL_ACTION(CastDispelMagicOnTargetAction, "dispel magic");
    CC_ACTION(CastShackleUndeadAction, "shackle undead");
    SPELL_ACTION_U(CastManaBurnAction, "mana burn", AI_VALUE2(uint8, "mana", "self target") < 50 && AI_VALUE2(uint8, "mana", "current target") >= 20);
    BUFF_ACTION(CastLevitateAction, "levitate");
    BUFF_ACTION(CastDivineSpiritAction, "divine spirit");
    BUFF_PARTY_ACTION(CastDivineSpiritOnPartyAction, "divine spirit");
    BUFF_PARTY_ACTION(CastPrayerOfSpiritOnPartyAction, "prayer of spirit");

    // disc talents
    BUFF_ACTION(CastPowerInfusionAction, "power infusion");
    BUFF_PARTY_ACTION(CastPowerInfusionOnPartyAction, "power infusion");
    BUFF_ACTION(CastInnerFocusAction, "inner focus");

    // holy
    HEAL_ACTION(CastLesserHealAction, "lesser heal");
    HEAL_PARTY_ACTION(CastLesserHealOnPartyAction, "lesser heal");
    HEAL_ACTION(CastHealAction, "heal");
    HEAL_PARTY_ACTION(CastHealOnPartyAction, "heal");
    HEAL_ACTION(CastGreaterHealAction, "greater heal");
    HEAL_PARTY_ACTION(CastGreaterHealOnPartyAction, "greater heal");
    HEAL_ACTION(CastFlashHealAction, "flash heal");
    HEAL_PARTY_ACTION(CastFlashHealOnPartyAction, "flash heal");
    HEAL_ACTION(CastRenewAction, "renew");
    HEAL_PARTY_ACTION(CastRenewOnPartyAction, "renew");

    AOE_HEAL_ACTION(CastPrayerOfHealingAction, "prayer of healing");
    AOE_HEAL_ACTION(CastLightwellAction, "lightwell");
    AOE_HEAL_ACTION(CastCircleOfHealingAction, "circle of healing");

    SPELL_ACTION(CastSmiteAction, "smite");
    SPELL_ACTION(CastHolyNovaAction, "holy nova");

    RESS_ACTION(CastResurrectionAction, "resurrection");

    CURE_ACTION(CastCureDiseaseAction, "cure disease");
    CURE_PARTY_ACTION(CastCureDiseaseOnPartyAction, "cure disease", DISPEL_DISEASE);
    CURE_ACTION(CastAbolishDiseaseAction, "abolish disease");
    CURE_PARTY_ACTION(CastAbolishDiseaseOnPartyAction, "abolish disease", DISPEL_DISEASE);

    DEBUFF_ACTION(CastHolyFireAction, "holy fire");

    // shadow
    DEBUFF_ACTION(CastPowerWordPainAction, "shadow word: pain");
    DEBUFF_ENEMY_ACTION(CastPowerWordPainOnAttackerAction, "shadow word: pain");
    SPELL_ACTION(CastMindBlastAction, "mind blast");
    SPELL_ACTION(CastPsychicScreamAction, "psychic scream");
    DEBUFF_ACTION(CastMindSootheAction, "mind soothe");
    BUFF_ACTION_U(CastFadeAction, "fade", bot->GetGroup());
    BUFF_ACTION(CastShadowProtectionAction, "shadow protection");
    BUFF_PARTY_ACTION(CastShadowProtectionOnPartyAction, "shadow protection");
    BUFF_PARTY_ACTION(CastPrayerOfShadowProtectionAction, "prayer of shadow protection");

    // shadow talents
    SPELL_ACTION(CastMindFlayAction, "mind flay");
    DEBUFF_ACTION(CastVampiricEmbraceAction, "vampiric embrace");
    BUFF_ACTION(CastShadowformAction, "shadowform");
    SPELL_ACTION(CastSilenceAction, "silence");
    ENEMY_HEALER_ACTION(CastSilenceOnEnemyHealerAction, "silence");

    // racials
    DEBUFF_ACTION_U(CastDevouringPlagueAction, "devouring plague", bot->getRace() == RACE_UNDEAD);
    BUFF_ACTION_U(CastTouchOfWeaknessAction, "touch of weakness", bot->getRace() == RACE_UNDEAD);
    DEBUFF_ACTION_U(CastHexOfWeaknessAction, "hex of weakness", bot->getRace() == RACE_TROLL);
    BUFF_ACTION_U(CastShadowguardAction, "shadowguard", bot->getRace() == RACE_TROLL);
    HEAL_ACTION_U(CastDesperatePrayerAction, "desperate prayer", (bot->getRace() == RACE_DWARF || bot->getRace() == RACE_HUMAN));
    BUFF_ACTION_U(CastFearWardAction, "fear ward", bot->getRace() == RACE_DWARF);
    BUFF_PARTY_ACTION(CastFearWardOnPartyAction, "fear ward");
    SPELL_ACTION_U(CastStarshardsAction, "starshards", (AI_VALUE2(uint8, "mana", "self target") > 50 && bot->getRace() == RACE_NIGHTELF && AI_VALUE(Unit*, "current target") && AI_VALUE2(float, "distance", "current target") > 15.0f));
    BUFF_ACTION_U(CastElunesGraceAction, "elune's grace", bot->getRace() == RACE_NIGHTELF);
    BUFF_ACTION_U(CastFeedbackAction, "feedback", bot->getRace() == RACE_HUMAN);

    class CastRemoveShadowformAction : public Action {
    public:
        CastRemoveShadowformAction(PlayerbotAI* ai) : Action(ai, "remove shadowform") {}
        virtual bool isUseful() { return ai->HasAura("shadowform", AI_VALUE(Unit*, "self target")); }
        virtual bool isPossible() { return true; }
        virtual bool Execute(Event event) {
            ai->RemoveAura("shadowform");
            return true;
        }
    };

    DEBUFF_ACTION(CastVampiricTouchAction, "vampiric touch");
}
