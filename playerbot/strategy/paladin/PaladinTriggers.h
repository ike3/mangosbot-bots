#pragma once
#include "../triggers/GenericTriggers.h"

namespace ai
{
    // master aura trigger
    BUFF_TRIGGER_A(PaladinAuraTrigger, "paladin aura");
    BUFF_TRIGGER(HolyShieldTrigger, "holy shield");
    BUFF_TRIGGER(RighteousFuryTrigger, "righteous fury");

    BUFF_TRIGGER_A(RetributionAuraTrigger, "retribution aura");
    BUFF_TRIGGER_A(SanctityAuraTrigger, "sanctity aura");

	class CrusaderAuraTrigger : public BuffTrigger
	{
	public:
		CrusaderAuraTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "crusader aura") {}
		virtual bool IsActive();
	};

	class SealTrigger : public BuffTrigger
	{
	public:
		SealTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "seal of justice") {}
		virtual bool IsActive();
	};

    // judgements
    DEBUFF_TRIGGER(JudgementTrigger, "judgement");
    DEBUFF_TRIGGER(JudgementOfLightTrigger, "judgement of light");
    DEBUFF_TRIGGER(JudgementOfWisdomTrigger, "judgement of wisdom");

    DEBUFF_TRIGGER(ConsecrationTrigger, "consecration");

    // repentance triggers
    INTERRUPT_HEALER_TRIGGER(RepentanceOnHealerTrigger, "repentance on enemy healer");
    SNARE_TRIGGER(RepentanceSnareTrigger, "repentance on snare target");
    INTERRUPT_TRIGGER(RepentanceInterruptTrigger, "repentance");

    // sanctuary for tanks
    BUFF_TRIGGER(BlessingOfSanctuaryTrigger, "blessing of sanctuary");

    class BlessingOnPartyTrigger : public BuffOnPartyTrigger
    {
    public:
        BlessingOnPartyTrigger(PlayerbotAI* ai) : BuffOnPartyTrigger(ai, "blessing", 2) {}
        virtual bool IsActive();
    };

    class BlessingTrigger : public BuffTrigger
    {
    public:
        BlessingTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "blessing", 2) {}
        virtual bool IsActive();
    };

    class HammerOfJusticeInterruptSpellTrigger : public InterruptSpellTrigger
    {
    public:
        HammerOfJusticeInterruptSpellTrigger(PlayerbotAI* ai) : InterruptSpellTrigger(ai, "hammer of justice") {}
    };

    class HammerOfJusticeSnareTrigger : public SnareTargetTrigger
    {
    public:
        HammerOfJusticeSnareTrigger(PlayerbotAI* ai) : SnareTargetTrigger(ai, "hammer of justice") {}
    };

    class HammerOfJusticeOnEnemyTrigger : public Trigger
    {
    public:
        HammerOfJusticeOnEnemyTrigger(PlayerbotAI* ai) : Trigger(ai, "hammer of justice on enemy") {}
        virtual bool IsActive();
    };

    class ArtOfWarTrigger : public HasAuraTrigger
    {
    public:
        ArtOfWarTrigger(PlayerbotAI* ai) : HasAuraTrigger(ai, "the art of war") {}
    };

    class ShadowResistanceAuraTrigger : public BuffTrigger
    {
    public:
        ShadowResistanceAuraTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "shadow resistance aura") {}
    };

    class FrostResistanceAuraTrigger : public BuffTrigger
    {
    public:
        FrostResistanceAuraTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "frost resistance aura") {}
    };

    class FireResistanceAuraTrigger : public BuffTrigger
    {
    public:
        FireResistanceAuraTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "fire resistance aura") {}
    };

    class DevotionAuraTrigger : public BuffTrigger
    {
    public:
        DevotionAuraTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "devotion aura") {}
    };

    BUFF_TRIGGER_A(ConcentrationAuraTrigger, "concentration aura");

    class CleanseCureDiseaseTrigger : public NeedCureTrigger
    {
    public:
        CleanseCureDiseaseTrigger(PlayerbotAI* ai) : NeedCureTrigger(ai, "cleanse", DISPEL_DISEASE) {}
    };

    class CleanseCurePartyMemberDiseaseTrigger : public PartyMemberNeedCureTrigger
    {
    public:
        CleanseCurePartyMemberDiseaseTrigger(PlayerbotAI* ai) : PartyMemberNeedCureTrigger(ai, "cleanse", DISPEL_DISEASE) {}
    };

    class CleanseCurePoisonTrigger : public NeedCureTrigger
    {
    public:
        CleanseCurePoisonTrigger(PlayerbotAI* ai) : NeedCureTrigger(ai, "cleanse", DISPEL_POISON) {}
    };

    class CleanseCurePartyMemberPoisonTrigger : public PartyMemberNeedCureTrigger
    {
    public:
        CleanseCurePartyMemberPoisonTrigger(PlayerbotAI* ai) : PartyMemberNeedCureTrigger(ai, "cleanse", DISPEL_POISON) {}
    };

    class CleanseCureMagicTrigger : public NeedCureTrigger
    {
    public:
        CleanseCureMagicTrigger(PlayerbotAI* ai) : NeedCureTrigger(ai, "cleanse", DISPEL_MAGIC) {}
    };

    class CleanseCurePartyMemberMagicTrigger : public PartyMemberNeedCureTrigger
    {
    public:
        CleanseCurePartyMemberMagicTrigger(PlayerbotAI* ai) : PartyMemberNeedCureTrigger(ai, "cleanse", DISPEL_MAGIC) {}
    };

    class HammerOfJusticeEnemyHealerTrigger : public InterruptEnemyHealerTrigger
    {
    public:
        HammerOfJusticeEnemyHealerTrigger(PlayerbotAI* ai) : InterruptEnemyHealerTrigger(ai, "hammer of justice") {}
    };

    class DivineFavorTrigger : public BuffTrigger
    {
    public:
        DivineFavorTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "divine favor") {}
    };

    class TurnUndeadTrigger : public HasCcTargetTrigger
    {
    public:
        TurnUndeadTrigger(PlayerbotAI* ai) : HasCcTargetTrigger(ai, "turn undead") {}
    };

    DEBUFF_TRIGGER(AvengerShieldTrigger, "avenger's shield");
    BOOST_TRIGGER(DivineIlluminationBoostTrigger, "divine illumination");
}
