#pragma once

#include "../triggers/GenericTriggers.h"

namespace ai
{
    class HunterNoStingsActiveTrigger : public Trigger
    {
    public:
        HunterNoStingsActiveTrigger(PlayerbotAI* ai) : Trigger(ai, "hunter no stings active", 1) {}

    public:
        virtual bool IsActive();
    };

    class HunterAspectOfTheHawkTrigger : public BuffTrigger
    {
    public:
        HunterAspectOfTheHawkTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "aspect of the hawk") {
			checkInterval = 0;
		}
    };

	class HunterAspectOfTheWildTrigger : public BuffTrigger
	{
	public:
		HunterAspectOfTheWildTrigger(PlayerbotAI* ai) : BuffTrigger(ai, "aspect of the wild") {
			checkInterval = 0;
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
        virtual bool IsActive() {
			return BuffTrigger::IsActive() && !ai->HasAura("aspect of the cheetah", GetTarget());
        };
    };

    class HuntersPetDeadTrigger : public Trigger
    {
    public:
        HuntersPetDeadTrigger(PlayerbotAI* ai) : Trigger(ai, "hunter pet dead", 1) {}

    public:
        virtual bool IsActive();
    };

    class HuntersPetLowHealthTrigger : public Trigger
    {
    public:
        HuntersPetLowHealthTrigger(PlayerbotAI* ai) : Trigger(ai, "hunter pet llow health", 1) {}

    public:
        virtual bool IsActive();
    };

    class BlackArrowTrigger : public DebuffTrigger
    {
    public:
        BlackArrowTrigger(PlayerbotAI* ai) : DebuffTrigger(ai, "black arrow") {}
    };

    class HuntersMarkTrigger : public DebuffTrigger
    {
    public:
        HuntersMarkTrigger(PlayerbotAI* ai) : DebuffTrigger(ai, "hunter's mark") {}
    };

    class FreezingTrapTrigger : public HasCcTargetTrigger
    {
    public:
        FreezingTrapTrigger(PlayerbotAI* ai) : HasCcTargetTrigger(ai, "freezing trap") {}
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
    };

    BEGIN_TRIGGER(HunterPetNotHappy, Trigger)
    END_TRIGGER()

    class ConsussiveShotSnareTrigger : public SnareTargetTrigger
    {
    public:
        ConsussiveShotSnareTrigger(PlayerbotAI* ai) : SnareTargetTrigger(ai, "concussive shot") {}
    };

    class ScareBeastTrigger : public HasCcTargetTrigger
    {
    public:
        ScareBeastTrigger(PlayerbotAI* ai) : HasCcTargetTrigger(ai, "scare beast") {}
    };
}
