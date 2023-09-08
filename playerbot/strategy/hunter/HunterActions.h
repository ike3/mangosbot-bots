#pragma once

#include "../actions/GenericActions.h"

namespace ai
{
    BEGIN_RANGED_DEBUFF_ACTION(CastHuntersMarkAction, "hunter's mark")
    END_SPELL_ACTION()

    class CastAutoShotAction : public CastSpellAction
    {
    public:
        CastAutoShotAction(PlayerbotAI* ai) : CastSpellAction(ai, "auto shot") {}
        bool Execute(Event& event) override;
        bool isUseful() override;
    };

    BEGIN_RANGED_SPELL_ACTION(CastTranquilizingShotAction, "tranquilizing shot")
    END_SPELL_ACTION()

    BEGIN_RANGED_SPELL_ACTION(CastArcaneShotAction, "arcane shot")
    END_SPELL_ACTION()

    BEGIN_RANGED_SPELL_ACTION(CastExplosiveShotAction, "explosive shot")
    END_SPELL_ACTION()

    BEGIN_RANGED_SPELL_ACTION(CastAimedShotAction, "aimed shot")
    END_SPELL_ACTION()

    BEGIN_RANGED_SPELL_ACTION(CastChimeraShotAction, "chimera shot")
    END_SPELL_ACTION()

    BEGIN_RANGED_DEBUFF_ACTION(CastConcussiveShotAction, "concussive shot")
    END_SPELL_ACTION()

    SPELL_ACTION(CastSteadyShotAction, "steady shot");

    BEGIN_RANGED_DEBUFF_ACTION(CastScatterShotAction, "scatter shot")
    END_SPELL_ACTION()

    BEGIN_RANGED_SPELL_ACTION(CastDistractingShotAction, "distracting shot")
    END_SPELL_ACTION()

    BEGIN_RANGED_SPELL_ACTION(CastMultiShotAction, "multi-shot")
    END_SPELL_ACTION()

    BEGIN_RANGED_SPELL_ACTION(CastVolleyAction, "volley")
    END_SPELL_ACTION()

    BEGIN_RANGED_DEBUFF_ACTION(CastSerpentStingAction, "serpent sting")
        virtual bool isUseful();
    END_SPELL_ACTION()

    BEGIN_RANGED_DEBUFF_ACTION(CastViperStingAction, "viper sting")
        virtual bool isUseful();
    END_SPELL_ACTION()

    BEGIN_RANGED_SPELL_ACTION(CastScorpidStingAction, "scorpid sting")
    END_SPELL_ACTION()

    class CastAspectOfTheMonkeyAction : public CastBuffSpellAction
    {
    public:
        CastAspectOfTheMonkeyAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "aspect of the monkey") {}
    };

    class CastAspectOfTheHawkAction : public CastBuffSpellAction
    {
    public:
        CastAspectOfTheHawkAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "aspect of the hawk") {}
    };

    class CastAspectOfTheWildAction : public CastBuffSpellAction
    {
    public:
        CastAspectOfTheWildAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "aspect of the wild") {}
    };

    class CastAspectOfTheCheetahAction : public CastBuffSpellAction
    {
    public:
        CastAspectOfTheCheetahAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "aspect of the cheetah") {}
        virtual bool isUseful();
    };

    class CastAspectOfThePackAction : public CastBuffSpellAction
    {
    public:
        CastAspectOfThePackAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "aspect of the pack") {}
    };

    class CastAspectOfTheViperAction : public CastBuffSpellAction
    {
    public:
        CastAspectOfTheViperAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "aspect of the viper") {}
    };

    class CastCallPetAction : public CastBuffSpellAction
    {
    public:
        CastCallPetAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "call pet") {}
    };

    class CastMendPetAction : public CastAuraSpellAction
    {
    public:
        CastMendPetAction(PlayerbotAI* ai) : CastAuraSpellAction(ai, "mend pet") {}
        virtual string GetTargetName() { return "pet target"; }
    };

    class CastRevivePetAction : public CastBuffSpellAction
    {
    public:
        CastRevivePetAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "revive pet") {}
    };

    class CastTrueshotAuraAction : public CastBuffSpellAction
    {
    public:
        CastTrueshotAuraAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "trueshot aura") {}
    };

    class CastFeignDeathAction : public CastBuffSpellAction
    {
    public:
        CastFeignDeathAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "feign death") {}
    };

    REMOVE_BUFF_ACTION(RemoveFeignDeathAction, "feign death");

    class CastRapidFireAction : public CastBuffSpellAction
    {
    public:
        CastRapidFireAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "rapid fire") {}
    };

    class CastBlackArrow : public CastRangedDebuffSpellAction
    {
    public:
        CastBlackArrow(PlayerbotAI* ai) : CastRangedDebuffSpellAction(ai, "black arrow") {}
    };

    SNARE_ACTION(CastBlackArrowSnareAction, "black arrow");
    SPELL_ACTION(CastSilencingShotAction, "silencing shot");
    ENEMY_HEALER_ACTION(CastSilencingShotOnHealerAction, "silencing shot");
    BUFF_ACTION(CastReadinessAction, "readiness");
    ;
    class CastWingClipAction : public CastMeleeSpellAction
    {
    public:
        CastWingClipAction(PlayerbotAI* ai) : CastMeleeSpellAction(ai, "wing clip") {}

        virtual bool isUseful()
        {
            return CastMeleeSpellAction::isUseful() && !ai->HasAura(GetSpellName(), GetTarget());
        }
    };

    class CastRaptorStrikeAction : public CastMeleeSpellAction
    {
    public:
        CastRaptorStrikeAction(PlayerbotAI* ai) : CastMeleeSpellAction(ai, "raptor strike") {}

        virtual bool isUseful()
        {
            Unit* target = GetTarget();
            return CastMeleeSpellAction::isUseful() && (ai->HasStrategy("close", BotState::BOT_STATE_COMBAT) || bot->hasUnitState(UNIT_STAT_MELEE_ATTACKING));
        }
    };

    class CastSerpentStingOnAttackerAction : public CastRangedDebuffSpellOnAttackerAction
    {
    public:
        CastSerpentStingOnAttackerAction(PlayerbotAI* ai) : CastRangedDebuffSpellOnAttackerAction(ai, "serpent sting") {}
    };

    class CastViperStingOnAttackerAction : public CastRangedDebuffSpellOnAttackerAction
    {
    public:
        CastViperStingOnAttackerAction(PlayerbotAI* ai) : CastRangedDebuffSpellOnAttackerAction(ai, "viper sting") {}
    };

    class FeedPetAction : public Action
    {
    public:
        FeedPetAction(PlayerbotAI* ai) : Action(ai, "feed pet") {}
        virtual bool Execute(Event& event);
    };

    class CastBestialWrathAction : public CastBuffSpellAction
    {
    public:
        CastBestialWrathAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "bestial wrath") {}
    };

    class CastScareBeastAction : public CastSpellAction
    {
    public:
        CastScareBeastAction(PlayerbotAI* ai) : CastSpellAction(ai, "scare beast") {}
    };

    class CastScareBeastCcAction : public CastCrowdControlSpellAction
    {
    public:
        CastScareBeastCcAction(PlayerbotAI* ai) : CastCrowdControlSpellAction(ai, "scare beast") {}
    };

    BUFF_ACTION(IntimidationAction, "intimidation");
    BUFF_ACTION(DeterrenceAction, "deterrence");
    MELEE_ACTION(CastCounterattackAction, "counterattack");
    SNARE_ACTION(WyvernStingSnareAction, "wyvern sting");
    MELEE_ACTION(MongooseBiteAction, "mongoose bite");

    class TameBeastAction : public CastSpellAction
    {
    public:
        TameBeastAction(PlayerbotAI* ai) : CastSpellAction(ai, "tame beast") {}
    };

    class CastFlareAction : public CastSpellAction
    {
    public:
        CastFlareAction(PlayerbotAI* ai) : CastSpellAction(ai, "flare") {}
        virtual string GetTargetName() override { return "nearest stealthed unit"; }
    };

    class TrapOnTargetAction : public CastSpellAction
    {
    public:
#ifdef MANGOSBOT_ZERO
        // For vanilla, bots need to feign death before dropping the trap
        TrapOnTargetAction(PlayerbotAI* ai, string spell) : CastSpellAction(ai, "feign death"), trapSpell(spell)
        {
            trapSpellID = AI_VALUE2(uint32, "spell id", trapSpell);
        }
#else
        TrapOnTargetAction(PlayerbotAI* ai, string spell) : CastSpellAction(ai, spell), trapSpell(spell) {}
#endif

    protected:
        // Traps don't really have target for the spell
        string GetTargetName() override { return "self target"; }

        // The move to target
        virtual string GetTrapTargetName() { return "current target"; }

        // The trap spell that will be used
        string GetTrapSpellName() { return trapSpell; }

        string GetReachActionName() override { return "reach melee"; }
        string GetTargetQualifier() override { return GetTrapSpellName(); }
        ActionThreatType getThreatType() override { return ActionThreatType::ACTION_THREAT_NONE; }

        NextAction** getPrerequisites() override
        {
            const string reachAction = GetReachActionName();
            const string spellName = GetSpellName();
            const string targetName = GetTrapTargetName();

            // Generate the reach action with qualifiers
            vector<string> qualifiers = { spellName, targetName, trapSpell };
            const string qualifiersStr = Qualified::MultiQualify(qualifiers, "::");
            return NextAction::merge(NextAction::array(0, new NextAction(reachAction + "::" + qualifiersStr), NULL), Action::getPrerequisites());
        }

#ifdef MANGOSBOT_ZERO
        bool isPossible() override
        {
            // If the trap spell and feign death are not on cooldown
            return sServerFacade.IsSpellReady(bot, trapSpellID) && sServerFacade.IsSpellReady(bot, 5384);
        }

        NextAction** getContinuers() override
        {
            return NextAction::merge(NextAction::array(0, new NextAction(trapSpell, ACTION_PASSTROUGH), NULL), CastSpellAction::getContinuers());
        }
#endif

private:
        string trapSpell;
        uint32 trapSpellID;
    };

    class TrapOnCcTargetAction : public TrapOnTargetAction
    {
    public:
        TrapOnCcTargetAction(PlayerbotAI* ai, string spell) : TrapOnTargetAction(ai, spell) {}
        string GetTrapTargetName() override { return "cc target"; }
    };

    class TrapInPlace : public TrapOnTargetAction
    {
    public:
        TrapInPlace(PlayerbotAI* ai, string spell) : TrapOnTargetAction(ai, spell) {}
        string GetTrapTargetName() override { return "self target"; }
    };

    class CastTrapAction : public CastSpellAction
    {
    public:
        CastTrapAction(PlayerbotAI* ai, string spell) : CastSpellAction(ai, spell) {}

        // Traps don't really have target for the spell
        string GetTargetName() override { return "self target"; }

#ifdef MANGOSBOT_ZERO
        bool Execute(Event& event) override
        {
            // The trap could come just after feign death, so better remove it
            ai->RemoveAura("feign death");
            return CastSpellAction::Execute(event);
        }
#endif
    };

    class CastImmolationTrapAction : public CastTrapAction
    {
    public:
        CastImmolationTrapAction(PlayerbotAI* ai) : CastTrapAction(ai, "immolation trap") {}
    };

    class CastImmolationTrapOnTargetAction : public TrapOnTargetAction
    {
    public:
        CastImmolationTrapOnTargetAction(PlayerbotAI* ai) : TrapOnTargetAction(ai, "immolation trap") {}
    };

    class CastImmolationTrapInPlaceAction : public TrapInPlace
    {
    public:
        CastImmolationTrapInPlaceAction(PlayerbotAI* ai) : TrapInPlace(ai, "immolation trap") {}
    };

    class CastFreezingTrapAction : public CastTrapAction
    {
    public:
        CastFreezingTrapAction(PlayerbotAI* ai) : CastTrapAction(ai, "freezing trap") {}
    };

    class CastFreezingTrapOnTargetAction : public TrapOnTargetAction
    {
    public:
        CastFreezingTrapOnTargetAction(PlayerbotAI* ai) : TrapOnTargetAction(ai, "freezing trap") {}
    };

    class CastFreezingTrapInPlaceAction : public TrapInPlace
    {
    public:
        CastFreezingTrapInPlaceAction(PlayerbotAI* ai) : TrapInPlace(ai, "freezing trap") {}
    };

    class CastFreezingTrapOnCcAction : public TrapOnCcTargetAction
    {
    public:
        CastFreezingTrapOnCcAction(PlayerbotAI* ai) : TrapOnCcTargetAction(ai, "freezing trap") {}
    };

    class CastFrostTrapAction : public CastTrapAction
    {
    public:
        CastFrostTrapAction(PlayerbotAI* ai) : CastTrapAction(ai, "frost trap") {}
    };

    class CastFrostTrapOnTargetAction : public TrapOnTargetAction
    {
    public:
        CastFrostTrapOnTargetAction(PlayerbotAI* ai) : TrapOnTargetAction(ai, "frost trap") {}
    };

    class CastFrostTrapInPlaceAction : public TrapInPlace
    {
    public:
        CastFrostTrapInPlaceAction(PlayerbotAI* ai) : TrapInPlace(ai, "frost trap") {}
    };

    class CastExplosiveTrapAction : public CastTrapAction
    {
    public:
        CastExplosiveTrapAction(PlayerbotAI* ai) : CastTrapAction(ai, "explosive trap") {}
    };

    class CastExplosiveTrapOnTargetAction : public TrapOnTargetAction
    {
    public:
        CastExplosiveTrapOnTargetAction(PlayerbotAI* ai) : TrapOnTargetAction(ai, "explosive trap") {}
    };

    class CastExplosiveTrapInPlaceAction : public TrapInPlace
    {
    public:
        CastExplosiveTrapInPlaceAction(PlayerbotAI* ai) : TrapInPlace(ai, "explosive trap") {}
    };
}
