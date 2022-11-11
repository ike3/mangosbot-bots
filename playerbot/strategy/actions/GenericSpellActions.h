#pragma once

#include "../Action.h"
#include "../../PlayerbotAIConfig.h"

namespace ai
{
    class CastSpellAction : public Action
    {
    public:
        CastSpellAction(PlayerbotAI* ai, string spell) 
        : Action(ai, spell)
        , range(ai->GetRange("spell"))
        {
            SetSpellName(spell);
        }

		virtual string GetTargetName() { return "current target"; }
        virtual bool Execute(Event& event);
        virtual bool isPossible();
		virtual bool isUseful();
        virtual ActionThreatType getThreatType() { return ActionThreatType::ACTION_THREAT_SINGLE; }

		/*virtual NextAction** getPrerequisites()
		{
            if (spell == "mount")
                return NULL;
            if (range > ai->GetRange("spell"))
				return NULL;
			else if (range > ATTACK_DISTANCE)
				return NextAction::merge( NextAction::array(0, new NextAction("reach spell"), NULL), Action::getPrerequisites());
			else
				return NextAction::merge( NextAction::array(0, new NextAction("reach melee"), NULL), Action::getPrerequisites());
		}*/

    protected:
        const uint32& GetSpellID() const { return spellId; }
        const string& GetSpellName() const { return spellName; }
        void SetSpellName(const string& name, string spellIDContextName = "spell id");

    protected:
		float range;

    private:
        string spellName;
        uint32 spellId;
    };

	//---------------------------------------------------------------------------------------------------------------------
	class CastAuraSpellAction : public CastSpellAction
	{
	public:
		CastAuraSpellAction(PlayerbotAI* ai, string spell, bool isOwner = false) : CastSpellAction(ai, spell) { this->isOwner = isOwner; }

		virtual bool isUseful();

    protected:
        bool isOwner;
	};

    //---------------------------------------------------------------------------------------------------------------------
    class CastMeleeSpellAction : public CastSpellAction
    {
    public:
        CastMeleeSpellAction(PlayerbotAI* ai, string spell) : CastSpellAction(ai, spell) {
			this->range = ATTACK_DISTANCE;
		}
    };

    //---------------------------------------------------------------------------------------------------------------------
    class CastDebuffSpellAction : public CastAuraSpellAction
    {
    public:
        CastDebuffSpellAction(PlayerbotAI* ai, string spell, bool isOwner = true) : CastAuraSpellAction(ai, spell, isOwner) {}
    };

    class CastOnlyDebuffSpellAction : public CastAuraSpellAction
    {
    public:
        CastOnlyDebuffSpellAction(PlayerbotAI* ai, string spell, bool isOwner = false) : CastAuraSpellAction(ai, spell, isOwner) {}
    };

    class CastDebuffSpellOnAttackerAction : public CastAuraSpellAction
    {
    public:
        CastDebuffSpellOnAttackerAction(PlayerbotAI* ai, string spell, bool isOwner = true) : CastAuraSpellAction(ai, spell, isOwner) {}
        Value<Unit*>* GetTargetValue()
        {
            return context->GetValue<Unit*>("attacker without aura", GetSpellName());
        }
        virtual string getName() { return GetSpellName() + " on attacker"; }
        virtual ActionThreatType getThreatType() { return ActionThreatType::ACTION_THREAT_AOE; }
    };

	class CastBuffSpellAction : public CastAuraSpellAction
	{
	public:
		CastBuffSpellAction(PlayerbotAI* ai, string spell) : CastAuraSpellAction(ai, spell)
		{
            range = ai->GetRange("spell");
		}

        virtual string GetTargetName() { return "self target"; }
	};

	class CastEnchantItemAction : public CastSpellAction
	{
	public:
	    CastEnchantItemAction(PlayerbotAI* ai, string spell) : CastSpellAction(ai, spell)
		{
            range = ai->GetRange("spell");
		}

        virtual bool isPossible();
        virtual string GetTargetName() { return "self target"; }
	};

    //---------------------------------------------------------------------------------------------------------------------

    class CastHealingSpellAction : public CastAuraSpellAction
    {
    public:
        CastHealingSpellAction(PlayerbotAI* ai, string spell, uint8 estAmount = 15.0f) : CastAuraSpellAction(ai, spell, true)
		{
            this->estAmount = estAmount;
            range = ai->GetRange("spell");
        }
		virtual string GetTargetName() { return "self target"; }
        virtual ActionThreatType getThreatType() { return ActionThreatType::ACTION_THREAT_AOE; }

    protected:
        uint8 estAmount;
    };

    class CastAoeHealSpellAction : public CastHealingSpellAction
    {
    public:
    	CastAoeHealSpellAction(PlayerbotAI* ai, string spell, uint8 estAmount = 15.0f) : CastHealingSpellAction(ai, spell, estAmount) {}
		virtual string GetTargetName() { return "party member to heal"; }
        virtual bool isUseful();
    };

	class CastCureSpellAction : public CastSpellAction
	{
	public:
		CastCureSpellAction(PlayerbotAI* ai, string spell) : CastSpellAction(ai, spell)
		{
            range = ai->GetRange("spell");
		}

		virtual string GetTargetName() { return "self target"; }
	};

	class PartyMemberActionNameSupport {
	public:
		PartyMemberActionNameSupport(string spell)
		{
			name = string(spell) + " on party";
		}

		virtual string getName() { return name; }

	private:
		string name;
	};

    class HealPartyMemberAction : public CastHealingSpellAction, public PartyMemberActionNameSupport
    {
    public:
        HealPartyMemberAction(PlayerbotAI* ai, string spell, uint8 estAmount = 15.0f) :
			CastHealingSpellAction(ai, spell, estAmount), PartyMemberActionNameSupport(spell) {}

		virtual string GetTargetName() { return "party member to heal"; }
		virtual string getName() { return PartyMemberActionNameSupport::getName(); }
    };

    class HealHotPartyMemberAction : public HealPartyMemberAction
    {
    public:
        HealHotPartyMemberAction(PlayerbotAI* ai, string spell) : HealPartyMemberAction(ai, spell) {}
        virtual bool isUseful()
        {
            return HealPartyMemberAction::isUseful() && GetTarget() && !ai->HasAura(GetSpellName(), GetTarget());
        }
    };

	class ResurrectPartyMemberAction : public CastSpellAction
	{
	public:
		ResurrectPartyMemberAction(PlayerbotAI* ai, string spell) : CastSpellAction(ai, spell) {}

		virtual string GetTargetName() { return "party member to resurrect"; }
	};
    //---------------------------------------------------------------------------------------------------------------------

    class CurePartyMemberAction : public CastSpellAction, public PartyMemberActionNameSupport
    {
    public:
        CurePartyMemberAction(PlayerbotAI* ai, string spell, uint32 dispelType) :
			CastSpellAction(ai, spell), PartyMemberActionNameSupport(spell)
        {
            this->dispelType = dispelType;
        }

		virtual Value<Unit*>* GetTargetValue();
		virtual string getName() { return PartyMemberActionNameSupport::getName(); }

    protected:
        uint32 dispelType;
    };

    //---------------------------------------------------------------------------------------------------------------------

    class BuffOnPartyAction : public CastBuffSpellAction, public PartyMemberActionNameSupport
    {
    public:
        BuffOnPartyAction(PlayerbotAI* ai, string spell) :
			CastBuffSpellAction(ai, spell), PartyMemberActionNameSupport(spell) {}
    public:
		virtual Value<Unit*>* GetTargetValue();
		virtual string getName() { return PartyMemberActionNameSupport::getName(); }
    };

    //---------------------------------------------------------------------------------------------------------------------

    class CastShootAction : public CastSpellAction
    {
    public:
        CastShootAction(PlayerbotAI* ai) : CastSpellAction(ai, "shoot"), rangedWeapon(nullptr), weaponDelay(0) {}
        ActionThreatType getThreatType() override { return ActionThreatType::ACTION_THREAT_NONE; }
        bool Execute(Event& event) override;
        bool isPossible() override;

    private:
        void UpdateWeaponInfo();

    private:
        const Item* rangedWeapon;
        uint32 weaponDelay;
    };

    class RemoveBuffAction : public Action
    {
    public:
        RemoveBuffAction(PlayerbotAI* ai, string spell) : Action(ai, "remove aura")
        {
            name = string(spell);
        }
    public:
        virtual string getName() { return "remove " + name; }
        virtual bool isUseful() { return ai->HasAura(name, AI_VALUE(Unit*, "self target")); }
        virtual bool isPossible() { return true; }
        virtual bool Execute(Event& event)
        {
            ai->RemoveAura(name);
            return !ai->HasAura(name, bot);
        }
    private:
        string name;
    };

    // racials

    // heal
#ifndef MANGOSBOT_ZERO
    HEAL_ACTION(CastGiftOfTheNaaruAction, "gift of the naaru");
#endif
    HEAL_ACTION(CastCannibalizeAction, "cannibalize");

    // buff

    BUFF_ACTION(CastShadowmeldAction, "shadowmeld");
    BUFF_ACTION(CastBerserkingAction, "berserking");
    BUFF_ACTION(CastBloodFuryAction, "blood fury");
    BUFF_ACTION(CastStoneformAction, "stoneform");
    BUFF_ACTION(CastPerceptionAction, "perception");

    // spells

#ifndef MANGOSBOT_ZERO
    SPELL_ACTION(CastManaTapAction, "mana tap");
    SPELL_ACTION(CastArcaneTorrentAction, "arcane torrent");
#endif

    class CastWarStompAction : public CastSpellAction
    {
    public:
        CastWarStompAction(PlayerbotAI* ai) : CastSpellAction(ai, "war stomp") {}
    };

    //cc breakers

    BUFF_ACTION(CastWillOfTheForsakenAction, "will of the forsaken");
    BUFF_ACTION_U(CastEscapeArtistAction, "escape artist", !ai->HasAura("stealth", AI_VALUE(Unit*, "self target")));
#ifdef MANGOSBOT_TWO
    SPELL_ACTION(CastEveryManforHimselfAction, "every man for himself");
#endif

    class CastSpellOnEnemyHealerAction : public CastSpellAction
    {
    public:
        CastSpellOnEnemyHealerAction(PlayerbotAI* ai, string spell) : CastSpellAction(ai, spell) {}
        Value<Unit*>* GetTargetValue()
        {
            return context->GetValue<Unit*>("enemy healer target", GetSpellName());
        }
        virtual string getName() { return GetSpellName() + " on enemy healer"; }
    };

    class CastSnareSpellAction : public CastDebuffSpellAction
    {
    public:
        CastSnareSpellAction(PlayerbotAI* ai, string spell) : CastDebuffSpellAction(ai, spell) {}
        Value<Unit*>* GetTargetValue()
        {
            return context->GetValue<Unit*>("snare target", GetSpellName());
        }
        virtual string getName() { return GetSpellName() + " on snare target"; }
        virtual ActionThreatType getThreatType() { return ActionThreatType::ACTION_THREAT_NONE; }
    };

    class CastCrowdControlSpellAction : public CastBuffSpellAction
    {
    public:
        CastCrowdControlSpellAction(PlayerbotAI* ai, string spell) : CastBuffSpellAction(ai, spell) {}
        Value<Unit*>* GetTargetValue()
        {
            return context->GetValue<Unit*>("cc target", getName());
        }
        virtual bool Execute(Event& event) { return ai->CastSpell(getName(), GetTarget()); }
        virtual bool isPossible() { return ai->CanCastSpell(getName(), GetTarget(), true); }
        virtual bool isUseful() { return true; }
        virtual ActionThreatType getThreatType() { return ActionThreatType::ACTION_THREAT_NONE; }
    };

    class CastProtectSpellAction : public CastSpellAction
    {
    public:
        CastProtectSpellAction(PlayerbotAI* ai, string spell) : CastSpellAction(ai, spell) {}
        virtual string GetTargetName() { return "party member to protect"; }
        virtual bool isUseful()
        {
            return GetTarget() && !ai->HasAura(GetSpellName(), GetTarget());
        }
        virtual ActionThreatType getThreatType() { return ActionThreatType::ACTION_THREAT_NONE; }
    };

    //--------------------//
    //   Vehicle Actions  //
    //--------------------//

    class CastVehicleSpellAction : public CastSpellAction
    {
    public:
        CastVehicleSpellAction(PlayerbotAI* ai, string spell) : CastSpellAction(ai, spell)
        {
            range = 120.0f;
            SetSpellName(spell, "vehicle spell id");
        }

        virtual string GetTargetName() { return "current target"; }
        virtual bool Execute(Event& event);
        virtual bool isUseful();
        virtual bool isPossible();
        virtual ActionThreatType getThreatType() { return ActionThreatType::ACTION_THREAT_NONE; }

    protected:
        WorldObject* spellTarget;
    };

    class CastHurlBoulderAction : public CastVehicleSpellAction
    {
    public:
        CastHurlBoulderAction(PlayerbotAI* ai) : CastVehicleSpellAction(ai, "hurl boulder") {}
    };

    class CastSteamRushAction : public CastVehicleSpellAction
    {
    public:
        CastSteamRushAction(PlayerbotAI* ai) : CastVehicleSpellAction(ai, "steam rush") {}
    };

    class CastRamAction : public CastVehicleSpellAction
    {
    public:
        CastRamAction(PlayerbotAI* ai) : CastVehicleSpellAction(ai, "ram") {}
    };

    class CastNapalmAction : public CastVehicleSpellAction
    {
    public:
        CastNapalmAction(PlayerbotAI* ai) : CastVehicleSpellAction(ai, "napalm") {}
    };

    class CastFireCannonAction : public CastVehicleSpellAction
    {
    public:
        CastFireCannonAction(PlayerbotAI* ai) : CastVehicleSpellAction(ai, "fire cannon") {}
    };

    class CastSteamBlastAction : public CastVehicleSpellAction
    {
    public:
        CastSteamBlastAction(PlayerbotAI* ai) : CastVehicleSpellAction(ai, "steam blast") {}
    };

    class CastIncendiaryRocketAction : public CastVehicleSpellAction
    {
    public:
        CastIncendiaryRocketAction(PlayerbotAI* ai) : CastVehicleSpellAction(ai, "incendiary rocket") {}
    };

    class CastRocketBlastAction : public CastVehicleSpellAction
    {
    public:
        CastRocketBlastAction(PlayerbotAI* ai) : CastVehicleSpellAction(ai, "rocket blast") {}
    };

    class CastGlaiveThrowAction : public CastVehicleSpellAction
    {
    public:
        CastGlaiveThrowAction(PlayerbotAI* ai) : CastVehicleSpellAction(ai, "glaive throw") {}
    };

    class CastBladeSalvoAction : public CastVehicleSpellAction
    {
    public:
        CastBladeSalvoAction(PlayerbotAI* ai) : CastVehicleSpellAction(ai, "blade salvo") {}
    };
}
