#pragma once
#include "../actions/GenericActions.h"

namespace ai
{
	// seals
	BUFF_ACTION(CastSealOfRighteousnessAction, "seal of righteousness");
	BUFF_ACTION(CastSealOfJusticeAction, "seal of justice");
	BUFF_ACTION(CastSealOfLightAction, "seal of light");
	BUFF_ACTION(CastSealOfWisdomAction, "seal of wisdom");
	BUFF_ACTION(CastSealOfCommandAction, "seal of command");
	BUFF_ACTION(CastSealOfVengeanceAction, "seal of vengeance");

	// judgements
	DEBUFF_ACTION_R(CastJudgementAction, "judgement", 10.0f);
	DEBUFF_ACTION_R(CastJudgementOfLightAction, "judgement of light", 10.0f);
	DEBUFF_ACTION_R(CastJudgementOfWisdomAction, "judgement of wisdom", 10.0f);
	DEBUFF_ACTION_R(CastJudgementOfJusticeAction, "judgement of justice", 10.0f);

	// auras
	BUFF_ACTION(CastDevotionAuraAction, "devotion aura");
	BUFF_ACTION(CastRetributionAuraAction, "retribution aura");
	BUFF_ACTION(CastConcentrationAuraAction, "concentration aura");
	BUFF_ACTION(CastShadowResistanceAuraAction, "shadow resistance aura");
	BUFF_ACTION(CastFrostResistanceAuraAction, "frost resistance aura");
	BUFF_ACTION(CastFireResistanceAuraAction, "fire resistance aura");
	BUFF_ACTION(CastCrusaderAuraAction, "crusader aura");
	BUFF_ACTION(CastSanctityAuraAction, "sanctity aura");

	SPELL_ACTION(CastHolyShockAction, "holy shock");
	HEAL_PARTY_ACTION(CastHolyShockOnPartyAction, "holy shock");

	// consecration
	SPELL_ACTION(CastConsecrationAction, "consecration");

	// repentance
	SNARE_ACTION(CastRepentanceSnareAction, "repentance");
	DEBUFF_ACTION(CastRepentanceAction, "repentance");
	ENEMY_HEALER_ACTION(CastRepentanceOnHealerAction, "repentance");

	//hamme of wrath
	SPELL_ACTION(CastHammerOfWrathAction, "hammer of wrath");

	// buffs
	BUFF_ACTION(CastDivineFavorAction, "divine favor");

	// blessings

	// fury
	BUFF_ACTION(CastRighteousFuryAction, "righteous fury");

	class CastDivineStormAction : public CastBuffSpellAction
	{
	public:
		CastDivineStormAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "divine storm") {}
	};

	class CastCrusaderStrikeAction : public CastMeleeSpellAction
	{
	public:
		CastCrusaderStrikeAction(PlayerbotAI* ai) : CastMeleeSpellAction(ai, "crusader strike") {}
	};

    class CastSealSpellAction : public CastBuffSpellAction
    {
    public:
        CastSealSpellAction(PlayerbotAI* ai, string name) : CastBuffSpellAction(ai, name) {}
        virtual bool isUseful() { return AI_VALUE2(bool, "combat", "self target"); }
    };

    
	class CastBlessingOfMightAction : public CastBuffSpellAction
	{
	public:
		CastBlessingOfMightAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "blessing of might") {}
        virtual bool Execute(Event event);
    };

	class CastGreaterBlessingOfMightAction : public CastBuffSpellAction
	{
	public:
		CastGreaterBlessingOfMightAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "greater blessing of might") {}
		virtual bool Execute(Event event);
	};

    class CastBlessingOnPartyAction : public BuffOnPartyAction
    {
    public:
        CastBlessingOnPartyAction(PlayerbotAI* ai, string name) : BuffOnPartyAction(ai, name) {}
        virtual Value<Unit*>* GetTargetValue();
    };

    class CastBlessingOfMightOnPartyAction : public CastBlessingOnPartyAction
	{
	public:
        CastBlessingOfMightOnPartyAction(PlayerbotAI* ai) : CastBlessingOnPartyAction(ai, "blessing of might") {}
        virtual string getName() { return "blessing of might on party";}
        virtual bool Execute(Event event);
	};

	class CastGreaterBlessingOfMightOnPartyAction : public CastBlessingOnPartyAction
	{
	public:
		CastGreaterBlessingOfMightOnPartyAction(PlayerbotAI* ai) : CastBlessingOnPartyAction(ai, "greater blessing of might") {}
		virtual string getName() { return "greater blessing of might on party"; }
		virtual bool Execute(Event event);
	};

	class CastBlessingOfWisdomAction : public CastBuffSpellAction
	{
	public:
		CastBlessingOfWisdomAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "blessing of wisdom") {}
        virtual bool Execute(Event event);
    };

	class CastGreaterBlessingOfWisdomAction : public CastBuffSpellAction
	{
	public:
		CastGreaterBlessingOfWisdomAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "greater blessing of wisdom") {}
		virtual bool Execute(Event event);
	};

    class CastBlessingOfWisdomOnPartyAction : public CastBlessingOnPartyAction
	{
	public:
        CastBlessingOfWisdomOnPartyAction(PlayerbotAI* ai) : CastBlessingOnPartyAction(ai, "blessing of wisdom") {}
        virtual string getName() { return "blessing of wisdom on party";}
        virtual bool Execute(Event event);
	};

	class CastGreaterBlessingOfWisdomOnPartyAction : public CastBlessingOnPartyAction
	{
	public:
		CastGreaterBlessingOfWisdomOnPartyAction(PlayerbotAI* ai) : CastBlessingOnPartyAction(ai, "greater blessing of wisdom") {}
		virtual string getName() { return "greater blessing of wisdom on party"; }
		virtual bool Execute(Event event);
	};

	class CastBlessingOfKingsAction : public CastBuffSpellAction
	{
	public:
		CastBlessingOfKingsAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "blessing of kings") {}
	};

	class CastGreaterBlessingOfKingsAction : public CastBuffSpellAction
	{
	public:
		CastGreaterBlessingOfKingsAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "greater blessing of kings") {}
	};

    class CastBlessingOfKingsOnPartyAction : public CastBlessingOnPartyAction
	{
	public:
        CastBlessingOfKingsOnPartyAction(PlayerbotAI* ai) : CastBlessingOnPartyAction(ai, "blessing of kings") {}
        virtual string getName() { return "blessing of kings on party";}
	};

	class CastGreaterBlessingOfKingsOnPartyAction : public CastBlessingOnPartyAction
	{
	public:
		CastGreaterBlessingOfKingsOnPartyAction(PlayerbotAI* ai) : CastBlessingOnPartyAction(ai, "greater blessing of kings") {}
		virtual string getName() { return "greater blessing of kings on party"; }
	};

	class CastBlessingOfSanctuaryAction : public CastBuffSpellAction
	{
	public:
		CastBlessingOfSanctuaryAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "blessing of sanctuary") {}
	};

	class CastGreaterBlessingOfSanctuaryAction : public CastBuffSpellAction
	{
	public:
		CastGreaterBlessingOfSanctuaryAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "greater blessing of sanctuary") {}
	};

    class CastBlessingOfSanctuaryOnPartyAction : public CastBlessingOnPartyAction
	{
	public:
        CastBlessingOfSanctuaryOnPartyAction(PlayerbotAI* ai) : CastBlessingOnPartyAction(ai, "blessing of sanctuary") {}
        virtual string getName() { return "blessing of sanctuary on party";}
	};

	class CastGreaterBlessingOfSanctuaryOnPartyAction : public CastBlessingOnPartyAction
	{
	public:
		CastGreaterBlessingOfSanctuaryOnPartyAction(PlayerbotAI* ai) : CastBlessingOnPartyAction(ai, "greater blessing of sanctuary") {}
		virtual string getName() { return "greater blessing of sanctuary on party"; }
	};

    class CastHolyLightAction : public CastHealingSpellAction
    {
    public:
        CastHolyLightAction(PlayerbotAI* ai) : CastHealingSpellAction(ai, "holy light") {}
    };

    class CastHolyLightOnPartyAction : public HealPartyMemberAction
    {
    public:
        CastHolyLightOnPartyAction(PlayerbotAI* ai) : HealPartyMemberAction(ai, "holy light") {}
    };

    class CastFlashOfLightAction : public CastHealingSpellAction
    {
    public:
        CastFlashOfLightAction(PlayerbotAI* ai) : CastHealingSpellAction(ai, "flash of light") {}
    };

    class CastFlashOfLightOnPartyAction : public HealPartyMemberAction
    {
    public:
        CastFlashOfLightOnPartyAction(PlayerbotAI* ai) : HealPartyMemberAction(ai, "flash of light") {}
    };

    class CastLayOnHandsAction : public CastHealingSpellAction
    {
    public:
        CastLayOnHandsAction(PlayerbotAI* ai) : CastHealingSpellAction(ai, "lay on hands") {}
    };

    class CastLayOnHandsOnPartyAction : public HealPartyMemberAction
    {
    public:
        CastLayOnHandsOnPartyAction(PlayerbotAI* ai) : HealPartyMemberAction(ai, "lay on hands") {}
    };

	class CastDivineProtectionAction : public CastBuffSpellAction
	{
	public:
		CastDivineProtectionAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "divine protection") {}
	};

    class CastDivineProtectionOnPartyAction : public HealPartyMemberAction
    {
    public:
        CastDivineProtectionOnPartyAction(PlayerbotAI* ai) : HealPartyMemberAction(ai, "divine protection") {}

        virtual string getName() { return "divine protection on party"; }
    };

	class CastDivineShieldAction: public CastBuffSpellAction
	{
	public:
		CastDivineShieldAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "divine shield") {}
	};

    class CastHolyWrathAction : public CastMeleeSpellAction
    {
    public:
        CastHolyWrathAction(PlayerbotAI* ai) : CastMeleeSpellAction(ai, "holy wrath") {}
    };

    class CastHammerOfJusticeAction : public CastMeleeSpellAction
    {
    public:
        CastHammerOfJusticeAction(PlayerbotAI* ai) : CastMeleeSpellAction(ai, "hammer of justice") {}
    };

	class CastHammerOfTheRighteousAction : public CastMeleeSpellAction
	{
	public:
		CastHammerOfTheRighteousAction(PlayerbotAI* ai) : CastMeleeSpellAction(ai, "hammer of the righteous") {}
	};

	class CastPurifyPoisonAction : public CastCureSpellAction
	{
	public:
		CastPurifyPoisonAction(PlayerbotAI* ai) : CastCureSpellAction(ai, "purify") {}
	};

	class CastPurifyDiseaseAction : public CastCureSpellAction
	{
	public:
		CastPurifyDiseaseAction(PlayerbotAI* ai) : CastCureSpellAction(ai, "purify") {}
	};

    class CastPurifyPoisonOnPartyAction : public CurePartyMemberAction
    {
    public:
        CastPurifyPoisonOnPartyAction(PlayerbotAI* ai) : CurePartyMemberAction(ai, "purify", DISPEL_POISON) {}

        virtual string getName() { return "purify poison on party"; }
    };

	class CastPurifyDiseaseOnPartyAction : public CurePartyMemberAction
	{
	public:
		CastPurifyDiseaseOnPartyAction(PlayerbotAI* ai) : CurePartyMemberAction(ai, "purify", DISPEL_DISEASE) {}

		virtual string getName() { return "purify disease on party"; }
	};

	class CastHandOfReckoningAction : public CastSpellAction
	{
	public:
		CastHandOfReckoningAction(PlayerbotAI* ai) : CastSpellAction(ai, "hand of reckoning") {}
	};

	class CastRighteousDefenseAction : public CastSpellAction
	{
	public:
		CastRighteousDefenseAction(PlayerbotAI* ai) : CastSpellAction(ai, "righteous defense") {}
	};

	class CastCleansePoisonAction : public CastCureSpellAction
	{
	public:
		CastCleansePoisonAction(PlayerbotAI* ai) : CastCureSpellAction(ai, "cleanse") {}
	};

	class CastCleanseDiseaseAction : public CastCureSpellAction
	{
	public:
		CastCleanseDiseaseAction(PlayerbotAI* ai) : CastCureSpellAction(ai, "cleanse") {}
	};

	class CastCleanseMagicAction : public CastCureSpellAction
	{
	public:
		CastCleanseMagicAction(PlayerbotAI* ai) : CastCureSpellAction(ai, "cleanse") {}
	};

    class CastCleansePoisonOnPartyAction : public CurePartyMemberAction
    {
    public:
        CastCleansePoisonOnPartyAction(PlayerbotAI* ai) : CurePartyMemberAction(ai, "cleanse", DISPEL_POISON) {}

        virtual string getName() { return "cleanse poison on party"; }
    };

	class CastCleanseDiseaseOnPartyAction : public CurePartyMemberAction
	{
	public:
		CastCleanseDiseaseOnPartyAction(PlayerbotAI* ai) : CurePartyMemberAction(ai, "cleanse", DISPEL_DISEASE) {}

		virtual string getName() { return "cleanse disease on party"; }
	};

	class CastCleanseMagicOnPartyAction : public CurePartyMemberAction
	{
	public:
		CastCleanseMagicOnPartyAction(PlayerbotAI* ai) : CurePartyMemberAction(ai, "cleanse", DISPEL_MAGIC) {}

		virtual string getName() { return "cleanse magic on party"; }
	};

    BEGIN_SPELL_ACTION(CastAvengersShieldAction, "avenger's shield")
    END_SPELL_ACTION()

	BEGIN_SPELL_ACTION(CastExorcismAction, "exorcism")
	END_SPELL_ACTION()

	class CastHolyShieldAction : public CastBuffSpellAction
	{
	public:
		CastHolyShieldAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "holy shield") {}
	};

	class CastRedemptionAction : public ResurrectPartyMemberAction
	{
	public:
		CastRedemptionAction(PlayerbotAI* ai) : ResurrectPartyMemberAction(ai, "redemption") {}
	};

    class CastHammerOfJusticeOnEnemyHealerAction : public CastSpellOnEnemyHealerAction
    {
    public:
        CastHammerOfJusticeOnEnemyHealerAction(PlayerbotAI* ai) : CastSpellOnEnemyHealerAction(ai, "hammer of justice") {}
    };

    class CastHammerOfJusticeSnareAction : public CastSnareSpellAction
    {
    public:
        CastHammerOfJusticeSnareAction(PlayerbotAI* ai) : CastSnareSpellAction(ai, "hammer of justice") {}
    };

    class CastTurnUndeadAction : public CastBuffSpellAction
    {
    public:
        CastTurnUndeadAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "turn undead") {}
        virtual Value<Unit*>* GetTargetValue() { return context->GetValue<Unit*>("cc target", getName()); }
    };

    PROTECT_ACTION(CastBlessingOfProtectionProtectAction, "blessing of protection");
}
