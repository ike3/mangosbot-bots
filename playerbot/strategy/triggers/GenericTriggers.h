#pragma once
#include "../Trigger.h"
#include "../../PlayerbotAIConfig.h"
#include "ServerFacade.h"

namespace ai
{
	class StatAvailable : public Trigger
	{
	public:
		StatAvailable(PlayerbotAI* ai, int amount, string name = "stat available") : Trigger(ai, name)
		{
			this->amount = amount;
		}

	protected:
		int amount;
	};

	class RageAvailable : public StatAvailable
    {
    public:
        RageAvailable(PlayerbotAI* ai, int amount) : StatAvailable(ai, amount, "rage available") {}
        virtual bool IsActive();
    };

    class LightRageAvailableTrigger : public RageAvailable
    {
    public:
        LightRageAvailableTrigger(PlayerbotAI* ai) : RageAvailable(ai, 20) {}
    };

    class MediumRageAvailableTrigger : public RageAvailable
    {
    public:
        MediumRageAvailableTrigger(PlayerbotAI* ai) : RageAvailable(ai, 40) {}
    };

    class HighRageAvailableTrigger : public RageAvailable
    {
    public:
        HighRageAvailableTrigger(PlayerbotAI* ai) : RageAvailable(ai, 60) {}
    };

	class EnergyAvailable : public StatAvailable
	{
	public:
		EnergyAvailable(PlayerbotAI* ai, int amount) : StatAvailable(ai, amount, "energy available") {}
		virtual bool IsActive();
	};

    class LightEnergyAvailableTrigger : public EnergyAvailable
    {
    public:
        LightEnergyAvailableTrigger(PlayerbotAI* ai) : EnergyAvailable(ai, 20) {}
    };

    class MediumEnergyAvailableTrigger : public EnergyAvailable
    {
    public:
        MediumEnergyAvailableTrigger(PlayerbotAI* ai) : EnergyAvailable(ai, 40) {}
    };

    class HighEnergyAvailableTrigger : public EnergyAvailable
    {
    public:
        HighEnergyAvailableTrigger(PlayerbotAI* ai) : EnergyAvailable(ai, 60) {}
    };

	class ComboPointsAvailableTrigger : public StatAvailable
	{
	public:
	    ComboPointsAvailableTrigger(PlayerbotAI* ai, int amount = 5) : StatAvailable(ai, amount, "combo points available") {}
		virtual bool IsActive();
	};

	class LoseAggroTrigger : public Trigger {
	public:
		LoseAggroTrigger(PlayerbotAI* ai) : Trigger(ai, "lose aggro") {}
		virtual bool IsActive();
	};

	class HasAggroTrigger : public Trigger {
	public:
	    HasAggroTrigger(PlayerbotAI* ai) : Trigger(ai, "have aggro") {}
		virtual bool IsActive();
	};

	class SpellTrigger : public Trigger
	{
	public:
		SpellTrigger(PlayerbotAI* ai, string spell, int checkInterval = 1) : Trigger(ai, spell, checkInterval)
		{
			this->spell = spell;
		}

		virtual string GetTargetName() { return "current target"; }
		virtual string getName() { return spell; }
		virtual bool IsActive();

	protected:
		string spell;
	};

	class SpellCanBeCastTrigger : public SpellTrigger
	{
	public:
		SpellCanBeCastTrigger(PlayerbotAI* ai, string spell) : SpellTrigger(ai, spell) {}
		virtual bool IsActive();
	};

    class SpellNoCooldownTrigger : public SpellTrigger
    {
    public:
        SpellNoCooldownTrigger(PlayerbotAI* ai, string spell) : SpellTrigger(ai, spell) {}
        virtual bool IsActive();
    };

	// TODO: check other targets
    class InterruptSpellTrigger : public SpellTrigger
	{
    public:
        InterruptSpellTrigger(PlayerbotAI* ai, string spell) : SpellTrigger(ai, spell) {}
        virtual bool IsActive();
    };

    class DeflectSpellTrigger : public SpellTrigger
    {
    public:
        DeflectSpellTrigger(PlayerbotAI* ai, string spell) : SpellTrigger(ai, spell) {}
        virtual bool IsActive();
    };


    class AttackerCountTrigger : public Trigger
    {
    public:
        AttackerCountTrigger(PlayerbotAI* ai, int amount, float distance = sPlayerbotAIConfig.sightDistance) : Trigger(ai, "attacker count", 2)
        {
            this->amount = amount;
            this->distance = distance;
        }
    public:
        virtual bool IsActive()
		{
            return AI_VALUE(uint8, "attacker count") >= amount;
        }
        virtual string getName() { return "attacker count"; }

    protected:
        int amount;
        float distance;
    };

    class HasAttackersTrigger : public AttackerCountTrigger
    {
    public:
        HasAttackersTrigger(PlayerbotAI* ai) : AttackerCountTrigger(ai, 1) {}
    };

    class MyAttackerCountTrigger : public AttackerCountTrigger
    {
    public:
        MyAttackerCountTrigger(PlayerbotAI* ai, int amount) : AttackerCountTrigger(ai, amount) {}
    public:
        virtual bool IsActive();
        virtual string getName() { return "my attacker count"; }
    };

    class MediumThreatTrigger : public MyAttackerCountTrigger
    {
    public:
        MediumThreatTrigger(PlayerbotAI* ai) : MyAttackerCountTrigger(ai, 2) {}
    };

    class AoeTrigger : public AttackerCountTrigger
    {
    public:
        AoeTrigger(PlayerbotAI* ai, int amount = 3, float range = 15.0f) : AttackerCountTrigger(ai, amount)
        {
            this->range = range;
        }
    public:
        virtual bool IsActive();
        virtual string getName() { return "aoe"; }

    private:
        float range;
    };

    class NoFoodTrigger : public Trigger {
    public:
        NoFoodTrigger(PlayerbotAI* ai) : Trigger(ai, "no food trigger") {}
        virtual bool IsActive()
        {
            return AI_VALUE2(list<Item*>, "inventory items", "conjured food").empty();
        }
    };

    class NoDrinkTrigger : public Trigger {
    public:
        NoDrinkTrigger(PlayerbotAI* ai) : Trigger(ai, "no drink trigger") {}
        virtual bool IsActive()
        {
            return AI_VALUE2(list<Item*>, "inventory items", "conjured water").empty();
        }
    };

    class LightAoeTrigger : public AoeTrigger
    {
    public:
        LightAoeTrigger(PlayerbotAI* ai) : AoeTrigger(ai, 2, 15.0f) {}
    };

    class MediumAoeTrigger : public AoeTrigger
    {
    public:
        MediumAoeTrigger(PlayerbotAI* ai) : AoeTrigger(ai, 3, 17.0f) {}
    };

    class HighAoeTrigger : public AoeTrigger
    {
    public:
        HighAoeTrigger(PlayerbotAI* ai) : AoeTrigger(ai, 4, 20.0f) {}
    };

    class BuffTrigger : public SpellTrigger
    {
    public:
        BuffTrigger(PlayerbotAI* ai, string spell, int checkInterval = 1, bool checkIsOwner = false) : SpellTrigger(ai, spell, checkInterval) { this->checkIsOwner = checkIsOwner; }
    public:
		virtual string GetTargetName() { return "self target"; }
        virtual bool IsActive();

    protected:
        bool checkIsOwner;
    };

    class MyBuffTrigger : public BuffTrigger
    {
    public:
        MyBuffTrigger(PlayerbotAI* ai, string spell, int checkInterval = 1) : BuffTrigger(ai, spell, checkInterval) {}
    public:
        virtual bool IsActive();
    };

    class BuffOnPartyTrigger : public BuffTrigger
    {
    public:
        BuffOnPartyTrigger(PlayerbotAI* ai, string spell, int checkInterval = 2) : BuffTrigger(ai, spell, checkInterval) {}
    public:
		virtual Value<Unit*>* GetTargetValue();
		virtual string getName() { return spell + " on party"; }
    };

    class MyBuffOnPartyTrigger : public BuffOnPartyTrigger
    {
    public:
        MyBuffOnPartyTrigger(PlayerbotAI* ai, string spell, int checkInterval = 2) : BuffOnPartyTrigger(ai, spell, checkInterval) {}
    public:
        virtual Value<Unit*>* GetTargetValue();
        virtual string getName() { return spell + " on party"; }
    };

    class ProtectPartyMemberTrigger : public Trigger
    {
    public:ProtectPartyMemberTrigger(PlayerbotAI* ai) : Trigger(ai, "protect party member") {}
    public:
        virtual string GetTargetName() { return "party member to protect"; }
        virtual bool IsActive()
        {
            return AI_VALUE(Unit*, "party member to protect");
        }
    };

    class NoAttackersTrigger : public Trigger
    {
    public:
        NoAttackersTrigger(PlayerbotAI* ai) : Trigger(ai, "no attackers") {}
        virtual bool IsActive();
    };

    class NoTargetTrigger : public Trigger
    {
    public:
        NoTargetTrigger(PlayerbotAI* ai) : Trigger(ai, "no target") {}
        virtual bool IsActive();
    };

    class InvalidTargetTrigger : public Trigger
    {
    public:
        InvalidTargetTrigger(PlayerbotAI* ai) : Trigger(ai, "invalid target") {}
        virtual bool IsActive();
    };

    class TargetInSightTrigger : public Trigger {
    public:
        TargetInSightTrigger(PlayerbotAI* ai) : Trigger(ai, "target in sight") {}
        virtual bool IsActive() { return AI_VALUE(Unit*, "grind target"); }
    };

    class DebuffTrigger : public BuffTrigger
    {
    public:
        DebuffTrigger(PlayerbotAI* ai, string spell, int checkInterval = 2, bool checkIsOwner = false) : BuffTrigger(ai, spell, checkInterval, checkIsOwner) {
			checkInterval = 1;
		}
    public:
		virtual string GetTargetName() { return "current target"; }
        virtual bool IsActive();
    };

    class DebuffOnAttackerTrigger : public DebuffTrigger
    {
    public:
        DebuffOnAttackerTrigger(PlayerbotAI* ai, string spell) : DebuffTrigger(ai, spell) {}
    public:
        virtual Value<Unit*>* GetTargetValue();
        virtual string getName() { return spell + " on attacker"; }
    };

	class BoostTrigger : public BuffTrigger
	{
	public:
		BoostTrigger(PlayerbotAI* ai, string spell, float balance = 50) : BuffTrigger(ai, spell, 3)
		{
			this->balance = balance;
		}
	public:
		virtual bool IsActive();

	protected:
		float balance;
	};

    class RandomTrigger : public Trigger
    {
    public:
        RandomTrigger(PlayerbotAI* ai, string name, int probability = 7) : Trigger(ai, name)
        {
            this->probability = probability;
            lastCheck = time(0);
        }
    public:
        virtual bool IsActive();

    protected:
        int probability;
        time_t lastCheck;
    };

    class TimeTrigger : public Trigger
    {
    public:
        TimeTrigger(PlayerbotAI* ai, string name, int interval = 2) : Trigger(ai, name, interval) {}
    public:
        virtual bool IsActive()
        {
            return true;
        }

    protected:
        int interval;
    };

    class AndTrigger : public Trigger
    {
    public:
        AndTrigger(PlayerbotAI* ai, Trigger* ls, Trigger* rs) : Trigger(ai)
        {
            this->ls = ls;
            this->rs = rs;
        }
        virtual ~AndTrigger()
        {
            delete ls;
            delete rs;
        }
    public:
        virtual bool IsActive();
        virtual string getName();

    protected:
        Trigger* ls;
        Trigger* rs;
    };

    class TwoTriggers : public Trigger
    {
    public:
        TwoTriggers(PlayerbotAI* ai, std::string name1 = "", std::string name2 = "") : Trigger(ai)
        {
            this->name1 = name1;
            this->name2 = name2;
        }
        virtual bool IsActive();
        virtual string getName();
    protected:
        std::string name1;
        std::string name2;
    };

    class SnareTargetTrigger : public DebuffTrigger
    {
    public:
        SnareTargetTrigger(PlayerbotAI* ai, string spell, int interval = 1) : DebuffTrigger(ai, spell, interval) {}
    public:
        virtual Value<Unit*>* GetTargetValue();
        virtual string getName() { return spell + " on snare target"; }
    };

	class LowManaTrigger : public Trigger
	{
	public:
		LowManaTrigger(PlayerbotAI* ai) : Trigger(ai, "low mana") {}

		virtual bool IsActive();
	};

	class MediumManaTrigger : public Trigger
	{
	public:
		MediumManaTrigger(PlayerbotAI* ai) : Trigger(ai, "medium mana") {}

		virtual bool IsActive();
	};

    class HighManaTrigger : public Trigger
    {
    public:
        HighManaTrigger(PlayerbotAI* ai) : Trigger(ai, "high mana") {}

        virtual bool IsActive();
    };

    class AlmostFullManaTrigger : public Trigger
    {
    public:
        AlmostFullManaTrigger(PlayerbotAI* ai) : Trigger(ai, "almost full mana") {}

        virtual bool IsActive();
    };

    BEGIN_TRIGGER(PanicTrigger, Trigger)
        virtual string getName() { return "panic"; }
    END_TRIGGER()

    BEGIN_TRIGGER(OutNumberedTrigger, Trigger)
        virtual string getName() { return "outnumbered"; }
    END_TRIGGER()

	class NoPetTrigger : public Trigger
	{
	public:
		NoPetTrigger(PlayerbotAI* ai) : Trigger(ai, "no pet", 30) {}

		virtual bool IsActive() {
			return !AI_VALUE(Unit*, "pet target") && !AI_VALUE2(bool, "mounted", "self target");
		}
	};

	class ItemCountTrigger : public Trigger {
	public:
		ItemCountTrigger(PlayerbotAI* ai, string item, int count, int interval = 30) : Trigger(ai, item, interval) {
			this->item = item;
			this->count = count;
		}
	public:
		virtual bool IsActive();
		virtual string getName() { return "item count"; }

	protected:
		string item;
		int count;
	};

    class AmmoCountTrigger : public ItemCountTrigger
    {
    public:
        AmmoCountTrigger(PlayerbotAI* ai, string item, uint32 count = 1, int interval = 30) : ItemCountTrigger(ai, item, count, interval) {}
    };

	class HasAuraTrigger : public Trigger {
	public:
		HasAuraTrigger(PlayerbotAI* ai, string spell, int interval = 1) : Trigger(ai, spell, interval) {}

		virtual string GetTargetName() { return "self target"; }
		virtual bool IsActive();

	};

    class HasNoAuraTrigger : public Trigger {
    public:
        HasNoAuraTrigger(PlayerbotAI* ai, string spell) : Trigger(ai, spell) {}

        virtual string GetTargetName() { return "self target"; }
        virtual bool IsActive();

    };

    class TimerTrigger : public Trigger
    {
    public:
        TimerTrigger(PlayerbotAI* ai) : Trigger(ai, "timer")
        {
            lastCheck = 0;
        }

    public:
        virtual bool IsActive()
        {
            if (time(0) != lastCheck)
            {
                lastCheck = time(0);
                return true;
            }
            return false;
        }

    private:
        time_t lastCheck;
    };

	class TankAssistTrigger : public NoAttackersTrigger
	{
	public:
        TankAssistTrigger(PlayerbotAI* ai) : NoAttackersTrigger(ai) {}

	public:
		virtual bool IsActive();

	};

    class IsBehindTargetTrigger : public Trigger
    {
    public:
        IsBehindTargetTrigger(PlayerbotAI* ai) : Trigger(ai) {}

    public:
        virtual bool IsActive();
    };

    class IsNotBehindTargetTrigger : public Trigger
    {
    public:
        IsNotBehindTargetTrigger(PlayerbotAI* ai) : Trigger(ai) {}

    public:
        virtual bool IsActive();
    };

    class IsNotFacingTargetTrigger : public Trigger
    {
    public:
        IsNotFacingTargetTrigger(PlayerbotAI* ai) : Trigger(ai) {}

    public:
        virtual bool IsActive();
    };

    class HasCcTargetTrigger : public Trigger
    {
    public:
        HasCcTargetTrigger(PlayerbotAI* ai, string name) : Trigger(ai, name) {}

    public:
        virtual bool IsActive();
    };

	class NoMovementTrigger : public Trigger
	{
	public:
		NoMovementTrigger(PlayerbotAI* ai, string name) : Trigger(ai, name) {}

	public:
		virtual bool IsActive();
	};


    class NoPossibleTargetsTrigger : public Trigger
    {
    public:
        NoPossibleTargetsTrigger(PlayerbotAI* ai) : Trigger(ai, "no possible targets") {}

    public:
        virtual bool IsActive();
    };

    class NotDpsTargetActiveTrigger : public Trigger
    {
    public:
        NotDpsTargetActiveTrigger(PlayerbotAI* ai) : Trigger(ai, "not dps target active", 2) {}

    public:
        virtual bool IsActive();
    };

    class NotDpsAoeTargetActiveTrigger : public Trigger
    {
    public:
        NotDpsAoeTargetActiveTrigger(PlayerbotAI* ai) : Trigger(ai, "not dps aoe target active") {}

    public:
        virtual bool IsActive();
    };

    class PossibleAddsTrigger : public Trigger
    {
    public:
        PossibleAddsTrigger(PlayerbotAI* ai) : Trigger(ai, "possible adds") {}

    public:
        virtual bool IsActive();
    };

    class IsSwimmingTrigger : public Trigger
    {
    public:
        IsSwimmingTrigger(PlayerbotAI* ai) : Trigger(ai, "swimming") {}

    public:
        virtual bool IsActive();
    };

    class HasNearestAddsTrigger : public Trigger
    {
    public:
        HasNearestAddsTrigger(PlayerbotAI* ai) : Trigger(ai, "has nearest adds") {}

    public:
        virtual bool IsActive();
    };

    class HasItemForSpellTrigger : public Trigger
    {
    public:
        HasItemForSpellTrigger(PlayerbotAI* ai, string spell) : Trigger(ai, spell) {}

    public:
        virtual bool IsActive();
    };

    class TargetChangedTrigger : public Trigger
    {
    public:
        TargetChangedTrigger(PlayerbotAI* ai) : Trigger(ai, "target changed") {}

    public:
        virtual bool IsActive();
    };

    class InterruptEnemyHealerTrigger : public SpellTrigger
    {
    public:
        InterruptEnemyHealerTrigger(PlayerbotAI* ai, string spell) : SpellTrigger(ai, spell) {}
    public:
        virtual Value<Unit*>* GetTargetValue();
        virtual string getName() { return spell + " on enemy healer"; }
    };

    class RandomBotUpdateTrigger : public RandomTrigger
    {
    public:
        RandomBotUpdateTrigger(PlayerbotAI* ai) : RandomTrigger(ai, "random bot update", 30) {}

    public:
        virtual bool IsActive()
        {
            return RandomTrigger::IsActive() && AI_VALUE(bool, "random bot update");
        }
    };

    class NoNonBotPlayersAroundTrigger : public Trigger
    {
    public:
        NoNonBotPlayersAroundTrigger(PlayerbotAI* ai) : Trigger(ai, "no non bot players around", 10) {}

    public:
        virtual bool IsActive()
        {
            return !ai->HasPlayerNearby();
            /*if (!bot->InBattleGround())
                return AI_VALUE(list<ObjectGuid>, "nearest non bot players").empty();
            else
                return false;*/
        }
    };

    class NewPlayerNearbyTrigger : public Trigger
    {
    public:
        NewPlayerNearbyTrigger(PlayerbotAI* ai) : Trigger(ai, "new player nearby", 10) {}

    public:
        virtual bool IsActive()
        {
            return AI_VALUE(ObjectGuid, "new player nearby");
        }
    };

    class CollisionTrigger : public Trigger
    {
    public:
        CollisionTrigger(PlayerbotAI* ai) : Trigger(ai, "collision", 5) {}

    public:
        virtual bool IsActive()
        {
            return AI_VALUE2(bool, "collision", "self target");
        }
    };

    class StayTimeTrigger : public Trigger
    {
    public:
        StayTimeTrigger(PlayerbotAI* ai, uint32 delay, string name) : Trigger(ai, name, 5), delay(delay) {}

    public:
        virtual bool IsActive();

    private:
        uint32 delay;
    };

    class SitTrigger : public StayTimeTrigger
    {
    public:
        SitTrigger(PlayerbotAI* ai) : StayTimeTrigger(ai, sPlayerbotAIConfig.sitDelay, "sit") {}
    };

    class ReturnTrigger : public StayTimeTrigger
    {
    public:
        ReturnTrigger(PlayerbotAI* ai) : StayTimeTrigger(ai, sPlayerbotAIConfig.returnDelay, "return") {}
    };

    class GiveItemTrigger : public Trigger
    {
    public:
        GiveItemTrigger(PlayerbotAI* ai, string name, string item) : Trigger(ai, name, 2), item(item) {}

    public:
        virtual bool IsActive()
        {
            return AI_VALUE2(Unit*, "party member without item", item) && AI_VALUE2(uint32, "item count", item);
        }

    protected:
        string item;
    };

    class GiveFoodTrigger : public GiveItemTrigger
    {
    public:
        GiveFoodTrigger(PlayerbotAI* ai) : GiveItemTrigger(ai, "give food", "conjured food") {}

    public:
        virtual bool IsActive()
        {
            return AI_VALUE(Unit*, "party member without food") && AI_VALUE2(uint32, "item count", item);
        }
    };

    class GiveWaterTrigger : public GiveItemTrigger
    {
    public:
        GiveWaterTrigger(PlayerbotAI* ai) : GiveItemTrigger(ai, "give water", "conjured water") {}
    public:
        virtual bool IsActive()
        {
            return AI_VALUE(Unit*, "party member without water") && AI_VALUE2(uint32, "item count", item);
        }
    };

    class IsMountedTrigger : public Trigger
    {
    public:
        IsMountedTrigger(PlayerbotAI* ai) : Trigger(ai, "mounted", 1) {}

    public:
        virtual bool IsActive();
    };

    class CorpseNearTrigger : public Trigger
    {
    public:
        CorpseNearTrigger(PlayerbotAI* ai) : Trigger(ai, "corpse near", 10) {}

    public:
        virtual bool IsActive();
    };

    class IsFallingTrigger : public Trigger
    {
    public:
        IsFallingTrigger(PlayerbotAI* ai) : Trigger(ai, "falling", 10) {}

    public:
        virtual bool IsActive();
    };

    class IsFallingFarTrigger : public Trigger
    {
    public:
        IsFallingFarTrigger(PlayerbotAI* ai) : Trigger(ai, "falling far", 10) {}

    public:
        virtual bool IsActive();
    };

    class UseTrinketTrigger : public BoostTrigger
    {
    public:
        UseTrinketTrigger(PlayerbotAI* ai) : BoostTrigger(ai, "use trinket", 3) {}

    public:
        virtual bool IsActive() { return AI_VALUE(list<Item*>, "trinkets on use").size() > 0; }
    };

    class HasAreaDebuffTrigger : public Trigger {
    public:
        HasAreaDebuffTrigger(PlayerbotAI* ai) : Trigger(ai, "has area debuff", 3) {}
        virtual bool IsActive();
    };

    // racials

    class BerserkingTrigger : public BoostTrigger
    {
    public:
        BerserkingTrigger(PlayerbotAI* ai) : BoostTrigger(ai, "berserking") {}
    };

    class BloodFuryTrigger : public BoostTrigger
    {
    public:
        BloodFuryTrigger(PlayerbotAI* ai) : BoostTrigger(ai, "blood fury") {}
    };

    class CannibalizeTrigger : public Trigger
    {
    public:
        CannibalizeTrigger(PlayerbotAI* ai) : Trigger(ai, "cannibalize") {}
        virtual bool IsActive()
        {
            if (AI_VALUE2(uint8, "health", "self target") > sPlayerbotAIConfig.almostFullHealth)
                return false;

            list<ObjectGuid> corpses = context->GetValue<list<ObjectGuid> >("nearest corpses")->Get();
            for (list<ObjectGuid>::iterator i = corpses.begin(); i != corpses.end(); i++)
            {
                if (!i->IsUnit())
                    continue;

                Unit* corpse = ai->GetUnit(*i);
                if (!corpse)
                    continue;

                if (!(corpse->GetCreatureType() == CREATURE_TYPE_HUMANOID || corpse->GetCreatureType() == CREATURE_TYPE_UNDEAD))
                    continue;

                if (sServerFacade.GetDistance2d(bot, corpse) <= 5.0f)
                    return true;
            }
            return false;
        }

    };

    class WOtFTrigger : public Trigger
    {
    public:
        WOtFTrigger(PlayerbotAI* ai) : Trigger(ai, "will of the forsaken") {}
        virtual bool IsActive()
        {
            return bot->HasAuraType(SPELL_AURA_MOD_FEAR);
            return bot->HasAuraType(SPELL_AURA_MOD_STUN);
            return bot->HasAuraType(SPELL_AURA_MOD_CHARM);
            return bot->HasAuraType(SPELL_AURA_MOD_CONFUSE);
        }
    };

    class RootedTrigger : public Trigger
    {
    public:
        RootedTrigger(PlayerbotAI* ai) : Trigger(ai, "rooted") {}
        virtual bool IsActive()
        {
            return bot->HasAuraType(SPELL_AURA_MOD_ROOT) ||
                bot->HasAuraType(SPELL_AURA_MOD_DECREASE_SPEED);
        }
    };

    class StoneformTrigger : public Trigger
    {
    public:
        StoneformTrigger(PlayerbotAI* ai) : Trigger(ai, "stoneform") {}
        virtual bool IsActive()
        {
            uint32 disMask = GetDispellMask(DISPEL_DISEASE);
            uint32 poisMask = GetDispellMask(DISPEL_POISON);
            uint32 bleedType = 1 << (MECHANIC_BLEED - 1);
            for (auto itr : bot->GetSpellAuraHolderMap())
            {
                SpellEntry const* spell = itr.second->GetSpellProto();
                if (IsPositiveSpell(spell->Id))
                    continue;

                if (((1 << spell->Dispel) & disMask) || ((1 << spell->Dispel) & poisMask))
                    return true;

                if (!spell->HasAttribute(SPELL_ATTR_NO_IMMUNITIES) && itr.second->HasMechanicMask(bleedType))
                    return true;
            }
            return false;
        }
    };

    class ShadowmeldTrigger : public Trigger
    {
    public:
        ShadowmeldTrigger(PlayerbotAI* ai) : Trigger(ai, "shadowmeld") {}
        virtual bool IsActive()
        {
            Unit* master = ai->GetMaster();
            if (ai->HasAura("shadowmeld", bot))
                return false;
            return ((!master || (master && master->HasStealthAura() && !master->IsMoving())) && !bot->IsMoving());
        }
    };

    class ManaTapTrigger : public Trigger
    {
    public:
        ManaTapTrigger(PlayerbotAI* ai) : Trigger(ai, "mana tap") {}
        virtual bool IsActive()
        {
            Unit* target = AI_VALUE(Unit*, "current target");
            return target && AI_VALUE2(bool, "has mana", "current target");
        }
    };

    class ArcanetorrentTrigger : public InterruptSpellTrigger
    {
    public:
        ArcanetorrentTrigger(PlayerbotAI* ai) : InterruptSpellTrigger(ai, "arcane torrent") {}
        virtual bool IsActive()
        {
            Unit* target = AI_VALUE(Unit*, "current target");
            return InterruptSpellTrigger::IsActive() && target && AI_VALUE2(float, "distance", "current target") <= 8.0f;
        }

    };

    class WarStompTrigger : public Trigger
    {
    public:
        WarStompTrigger(PlayerbotAI* ai) : Trigger(ai, "war stomp") {}
        virtual bool IsActive()
        {
            Unit* target = AI_VALUE(Unit*, "current target");
            return target && AI_VALUE2(bool, "combat", "self target") && AI_VALUE2(float, "distance", "current target") <= 8.0f &&
                (AI_VALUE2(uint8, "health", "self target") < sPlayerbotAIConfig.mediumHealth ||
                    AI_VALUE(uint8, "my attacker count") >= 3 ||
                    target->IsNonMeleeSpellCasted(true));
        }

    };

    class PerceptionTrigger : public BuffTrigger
    {
    public:
        PerceptionTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "perception") {}
        virtual bool IsActive()
        {
            for (auto& attacker : ai->GetAiObjectContext()->GetValue<list<ObjectGuid>>("possible attack targets")->Get())
            {
                Unit* enemy = ai->GetUnit(attacker);
                if (!enemy)
                    continue;

                if (enemy->getClass() == CLASS_ROGUE || enemy->getClass() == CLASS_DRUID)
                    return true;
            }
            return false;
        }
    };
}

#include "RangeTriggers.h"
#include "HealthTriggers.h"
#include "CureTriggers.h"
