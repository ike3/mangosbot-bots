#pragma once
#include "../triggers/GenericTriggers.h"

namespace ai
{
	class DemonArmorTrigger : public BuffTrigger
	{
	public:
		DemonArmorTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "demon armor") {}
		virtual bool IsActive();
	};

    class SpellstoneTrigger : public BuffTrigger
    {
    public:
        SpellstoneTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "spellstone") {}
        virtual bool IsActive();
    };

    DEBUFF_TRIGGER(CurseOfAgonyTrigger, "curse of agony");
    DEBUFF_TRIGGER_A(CorruptionTrigger, "corruption");
    DEBUFF_TRIGGER(SiphonLifeTrigger, "siphon life");
    DEBUFF_TRIGGER(UnstableAfflictionTrigger, "unstable affliction");

    INTERRUPT_TRIGGER(DeathCoilInterruptTrigger, "death coil");
    INTERRUPT_HEALER_TRIGGER(DeathCoilInterruptTHealerTrigger, "death coil");
    SNARE_TRIGGER(DeathCoilSnareTrigger, "death coil");
    SNARE_TRIGGER(ShadowfurySnareTrigger, "shadowfury");
    INTERRUPT_TRIGGER(ShadowfuryInterruptTrigger, "shadowfury");

    class CorruptionOnAttackerTrigger : public DebuffOnAttackerTrigger
    {
    public:
        CorruptionOnAttackerTrigger(PlayerbotAI* ai) : DebuffOnAttackerTrigger(ai, "corruption") {}
    };

    class CastCurseOfAgonyOnAttackerTrigger : public DebuffOnAttackerTrigger
    {
    public:
        CastCurseOfAgonyOnAttackerTrigger(PlayerbotAI* ai) : DebuffOnAttackerTrigger(ai, "curse of agony") {}
    };

    class SiphonLifeOnAttackerTrigger : public DebuffOnAttackerTrigger
    {
    public:
        SiphonLifeOnAttackerTrigger(PlayerbotAI* ai) : DebuffOnAttackerTrigger(ai, "siphon life") {}
    };

    DEBUFF_TRIGGER(ImmolateTrigger, "immolate");

    class ShadowTranceTrigger : public HasAuraTrigger
    {
    public:
        ShadowTranceTrigger(PlayerbotAI* ai) : HasAuraTrigger(ai, "shadow trance") {}
    };

    class BacklashTrigger : public HasAuraTrigger
    {
    public:
        BacklashTrigger(PlayerbotAI* ai) : HasAuraTrigger(ai, "backlash") {}
    };

    class BanishTrigger : public HasCcTargetTrigger
    {
    public:
        BanishTrigger(PlayerbotAI* ai) : HasCcTargetTrigger(ai, "banish") {}
    };

    class WarlockConjuredItemTrigger : public ItemCountTrigger
    {
    public:
        WarlockConjuredItemTrigger(PlayerbotAI* ai, string item) : ItemCountTrigger(ai, item, 1) {}

        virtual bool IsActive() { return ItemCountTrigger::IsActive() && AI_VALUE2(uint32, "item count", "soul shard") > 0; }
    };

    class HasSpellstoneTrigger : public WarlockConjuredItemTrigger
    {
    public:
        HasSpellstoneTrigger(PlayerbotAI* ai) : WarlockConjuredItemTrigger(ai, "spellstone") {}
    };

    class HasFirestoneTrigger : public WarlockConjuredItemTrigger
    {
    public:
        HasFirestoneTrigger(PlayerbotAI* ai) : WarlockConjuredItemTrigger(ai, "firestone") {}
    };

    class HasHealthstoneTrigger : public WarlockConjuredItemTrigger
    {
    public:
        HasHealthstoneTrigger(PlayerbotAI* ai) : WarlockConjuredItemTrigger(ai, "healthstone") {}
    };

    class FearTrigger : public HasCcTargetTrigger
    {
    public:
        FearTrigger(PlayerbotAI* ai) : HasCcTargetTrigger(ai, "fear") {}
    };

    class AmplifyCurseTrigger : public BuffTrigger
    {
    public:
        AmplifyCurseTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "amplify curse") {}
    };

    class InfernoTrigger : public BuffTrigger
    {
    public:
        InfernoTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "inferno", 10) {}
        virtual bool IsActive();
    };

}
