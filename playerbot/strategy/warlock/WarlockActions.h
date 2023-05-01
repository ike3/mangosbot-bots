#pragma once

#include "../actions/GenericActions.h"
#include "../actions/UseItemAction.h"

namespace ai
{
	SNARE_ACTION(CastDeathCoilSnareAction, "death coil");
	ENEMY_HEALER_ACTION(CastDeathCoilOnHealerAction, "death coil");
	SPELL_ACTION(CastDeathCoilAction, "death coil");
	
	class CastDemonSkinAction : public CastBuffSpellAction 
	{
	public:
		CastDemonSkinAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "demon skin") {}
	};

	class CastDemonArmorAction : public CastBuffSpellAction
	{
	public:
		CastDemonArmorAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "demon armor") {}
	};

	class CastFelArmorAction : public CastBuffSpellAction
	{
	public:
		CastFelArmorAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "fel armor") {}
	};

    BEGIN_RANGED_SPELL_ACTION(CastShadowBoltAction, "shadow bolt")
    END_SPELL_ACTION()

	class CastSoulShardAction : public CastSpellAction
	{
	public:
		CastSoulShardAction(PlayerbotAI* ai, std::string name) : CastSpellAction(ai, name) {}

		bool Execute(Event& event) override
		{
			if (ai->HasCheat(BotCheatMask::item))
			{
                // Add a soul shard to the bag
                const uint32 soulShardItemId = 6265;

                // Check space and find places
                ItemPosCountVec dest;
                uint8 msg = bot->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, soulShardItemId, 1);
                if (msg == EQUIP_ERR_OK)
                {
                    Item* item = bot->StoreNewItem(dest, soulShardItemId, true, Item::GenerateItemRandomPropertyId(soulShardItemId));
                    bot->SendNewItem(item, 1, false, true, false);
                }
			}

            return CastSpellAction::Execute(event);
		}

		virtual bool isPossible() override
		{
            Unit* spellTarget = GetTarget();
			if (spellTarget)
			{
				bool canReach = true;
				if (spellTarget != bot)
				{
					float dist = bot->GetDistance(spellTarget, true);
                    float max_range, min_range;
                    if (ai->GetSpellRange(GetSpellName(), &max_range, &min_range))
                    {
                        canReach = dist < max_range&& dist >= min_range;
                    }
				}

				if (canReach)
				{
					const bool ignoreReagents = ai->HasCheat(BotCheatMask::item);
					return ai->CanCastSpell(GetSpellName(), GetTarget(), 0, nullptr, true, ignoreReagents);
				}
			}

			return false;
		}

		virtual bool isUseful() override
		{
            // If no item cheats enabled
            if (!ai->HasCheat(BotCheatMask::item))
            {
                // Check if it has a soul shard
                if (!bot->HasItemCount(6265, 1))
                {
					return false;
				}
			}

			return CastSpellAction::isUseful();
		}

        virtual string GetTargetName() override { return "self target"; }
	};

	class CastDrainSoulAction : public CastSpellAction
	{
	public:
		CastDrainSoulAction(PlayerbotAI* ai) : CastSpellAction(ai, "drain soul") {}
	};

    class CastShadowburnAction : public CastSoulShardAction
    {
    public:
		CastShadowburnAction(PlayerbotAI* ai) : CastSoulShardAction(ai, "shadowburn") {}
        string GetTargetName() override { return "current target"; }
    };

    class CastSoulShatterAction : public CastSoulShardAction
    {
    public:
		CastSoulShatterAction(PlayerbotAI* ai) : CastSoulShardAction(ai, "soulshatter") {}
    };

    class CastSoulFireAction : public CastSoulShardAction
    {
    public:
		CastSoulFireAction(PlayerbotAI* ai) : CastSoulShardAction(ai, "soul fire") {}
        string GetTargetName() override { return "current target"; }
    };

	BUFF_ACTION(CastDarkPactAction, "dark pact");

	class CastDrainManaAction : public CastSpellAction
	{
	public:
		CastDrainManaAction(PlayerbotAI* ai) : CastSpellAction(ai, "drain mana") {}
	};

	class CastDrainLifeAction : public CastSpellAction
	{
	public:
		CastDrainLifeAction(PlayerbotAI* ai) : CastSpellAction(ai, "drain life") {}
	};

    class CastCurseOfExhaustionAction : public CastRangedDebuffSpellAction
    {
    public:
		CastCurseOfExhaustionAction(PlayerbotAI* ai) : CastRangedDebuffSpellAction(ai, "curse of exhaustion") {}
    };

	class CastCorruptionAction : public CastRangedDebuffSpellAction
	{
	public:
		CastCorruptionAction(PlayerbotAI* ai) : CastRangedDebuffSpellAction(ai, "corruption") {}
	};

	class CastCorruptionOnAttackerAction : public CastRangedDebuffSpellOnAttackerAction
	{
	public:
	    CastCorruptionOnAttackerAction(PlayerbotAI* ai) : CastRangedDebuffSpellOnAttackerAction(ai, "corruption") {}
	};

    class CastSeedOfCorruptionOnAttackerAction : public CastRangedDebuffSpellOnAttackerAction
    {
    public:
		CastSeedOfCorruptionOnAttackerAction(PlayerbotAI* ai) : CastRangedDebuffSpellOnAttackerAction(ai, "seed of corruption") {}
    };

    class CastUnstableAfflictionOnAttackerAction : public CastRangedDebuffSpellOnAttackerAction
    {
    public:
		CastUnstableAfflictionOnAttackerAction(PlayerbotAI* ai) : CastRangedDebuffSpellOnAttackerAction(ai, "unstable affliction") {}
    };

    class CastCurseOfAgonyAction : public CastRangedDebuffSpellAction
    {
    public:
        CastCurseOfAgonyAction(PlayerbotAI* ai) : CastRangedDebuffSpellAction(ai, "curse of agony") {}
    };

    class CastCurseOfAgonyOnAttackerAction : public CastRangedDebuffSpellOnAttackerAction
    {
    public:
        CastCurseOfAgonyOnAttackerAction(PlayerbotAI* ai) : CastRangedDebuffSpellOnAttackerAction(ai, "curse of agony") {}
    };

    class CastCurseOfDoomAction : public CastRangedDebuffSpellAction
    {
    public:
		CastCurseOfDoomAction(PlayerbotAI* ai) : CastRangedDebuffSpellAction(ai, "curse of doom") {}
    };

    class CastCurseOfDoomOnAttackerAction : public CastRangedDebuffSpellOnAttackerAction
    {
    public:
		CastCurseOfDoomOnAttackerAction(PlayerbotAI* ai) : CastRangedDebuffSpellOnAttackerAction(ai, "curse of doom") {}
    };

    class CastCurseOfTheElementsAction : public CastRangedDebuffSpellAction
    {
    public:
        CastCurseOfTheElementsAction(PlayerbotAI* ai) : CastRangedDebuffSpellAction(ai, "curse of the elements") {}
    };

    class CastCurseOfTheElementsOnAttackerAction : public CastRangedDebuffSpellOnAttackerAction
    {
    public:
        CastCurseOfTheElementsOnAttackerAction(PlayerbotAI* ai) : CastRangedDebuffSpellOnAttackerAction(ai, "curse of the elements") {}
    };

    class CastCurseOfRecklessnessAction : public CastRangedDebuffSpellAction
    {
    public:
        CastCurseOfRecklessnessAction(PlayerbotAI* ai) : CastRangedDebuffSpellAction(ai, "curse of recklessness") {}
    };

    class CastCurseOfRecklessnessOnAttackerAction : public CastRangedDebuffSpellOnAttackerAction
    {
    public:
        CastCurseOfRecklessnessOnAttackerAction(PlayerbotAI* ai) : CastRangedDebuffSpellOnAttackerAction(ai, "curse of recklessness") {}
    };

    class CastCurseOfWeaknessAction : public CastRangedDebuffSpellAction
    {
    public:
        CastCurseOfWeaknessAction(PlayerbotAI* ai) : CastRangedDebuffSpellAction(ai, "curse of weakness") {}
    };

    class CastCurseOfWeaknessOnAttackerAction : public CastRangedDebuffSpellOnAttackerAction
    {
    public:
        CastCurseOfWeaknessOnAttackerAction(PlayerbotAI* ai) : CastRangedDebuffSpellOnAttackerAction(ai, "curse of weakness") {}
    };

    class CastCurseOfTonguesAction : public CastRangedDebuffSpellAction
    {
    public:
        CastCurseOfTonguesAction(PlayerbotAI* ai) : CastRangedDebuffSpellAction(ai, "curse of tongues") {}
    };

    class CastCurseOfTonguesOnAttackerAction : public CastRangedDebuffSpellOnAttackerAction
    {
    public:
        CastCurseOfTonguesOnAttackerAction(PlayerbotAI* ai) : CastRangedDebuffSpellOnAttackerAction(ai, "curse of tongues") {}
    };

    class CastCurseOfShadowAction : public CastRangedDebuffSpellAction
    {
    public:
        CastCurseOfShadowAction(PlayerbotAI* ai) : CastRangedDebuffSpellAction(ai, "curse of shadow") {}
    };

    class CastCurseOfShadowOnAttackerAction : public CastRangedDebuffSpellOnAttackerAction
    {
    public:
        CastCurseOfShadowOnAttackerAction(PlayerbotAI* ai) : CastRangedDebuffSpellOnAttackerAction(ai, "curse of shadow") {}
    };

	class CastSummonVoidwalkerAction : public CastSoulShardAction
	{
	public:
		CastSummonVoidwalkerAction(PlayerbotAI* ai) : CastSoulShardAction(ai, "summon voidwalker") {}
	};

	class CastSummonFelguardAction : public CastSoulShardAction
	{
	public:
		CastSummonFelguardAction(PlayerbotAI* ai) : CastSoulShardAction(ai, "summon felguard") {}
	};

    class CastDemonicSacrificeAction : public CastBuffSpellAction
    {
    public:
		CastDemonicSacrificeAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "demonic sacrifice") {}
    };

    class CastSoulLinkAction : public CastBuffSpellAction
    {
    public:
		CastSoulLinkAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "soul link") {}
    };

	class CastSummonImpAction : public CastBuffSpellAction
	{
	public:
		CastSummonImpAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "summon imp") {}
	};

    class CastSummonSuccubusAction : public CastSoulShardAction
    {
    public:
        CastSummonSuccubusAction(PlayerbotAI* ai) : CastSoulShardAction(ai, "summon succubus") {}
    };

	class CastSummonFelhunterAction : public CastSoulShardAction
	{
	public:
		CastSummonFelhunterAction(PlayerbotAI* ai) : CastSoulShardAction(ai, "summon felhunter") {}
	};

	class CastSummonInfernoAction : public CastSpellAction
	{
	public:
		CastSummonInfernoAction(PlayerbotAI* ai) : CastSpellAction(ai, "inferno") {}
		virtual bool isPossible() { return true; }
	};

	class CastCreateHealthstoneAction : public CastSoulShardAction
	{
	public:
		CastCreateHealthstoneAction(PlayerbotAI* ai) : CastSoulShardAction(ai, "create healthstone") {}
	};

	class CastCreateFirestoneAction : public CastSoulShardAction
	{
	public:
		CastCreateFirestoneAction(PlayerbotAI* ai) : CastSoulShardAction(ai, "create firestone") {}
	};

	class CastCreateSpellstoneAction : public CastSoulShardAction
	{
	public:
		CastCreateSpellstoneAction(PlayerbotAI* ai) : CastSoulShardAction(ai, "create spellstone") {}
	};

    class CastBanishAction : public CastSoulShardAction
    {
    public:
        CastBanishAction(PlayerbotAI* ai) : CastSoulShardAction(ai, "banish") {}
        virtual string GetTargetName() override { return "snare target"; }
        virtual string GetTargetQualifier() override { return GetSpellName(); }
        virtual ActionThreatType getThreatType() { return ActionThreatType::ACTION_THREAT_NONE; }
    };

    class CastSeedOfCorruptionAction : public CastRangedDebuffSpellAction
    {
    public:
        CastSeedOfCorruptionAction(PlayerbotAI* ai) : CastRangedDebuffSpellAction(ai, "seed of corruption") {}
    };

    class CastRainOfFireAction : public CastSpellAction
    {
    public:
        CastRainOfFireAction(PlayerbotAI* ai) : CastSpellAction(ai, "rain of fire") {}
    };

    class CastShadowfuryAction : public CastSpellAction
    {
    public:
        CastShadowfuryAction(PlayerbotAI* ai) : CastSpellAction(ai, "shadowfury") {}
    };

	SNARE_ACTION(CastShadowfurySnareAction, "shadowfury");
	RANGED_DEBUFF_ACTION(CastUnstableAfflictionAction, "unstable affliction");

    class CastImmolateAction : public CastRangedDebuffSpellAction
    {
    public:
        CastImmolateAction(PlayerbotAI* ai) : CastRangedDebuffSpellAction(ai, "immolate") {}
    };

    class CastConflagrateAction : public CastSpellAction
    {
    public:
        CastConflagrateAction(PlayerbotAI* ai) : CastSpellAction(ai, "conflagrate") {}
    };

    class CastIncinerateAction : public CastSpellAction
    {
    public:
        CastIncinerateAction(PlayerbotAI* ai) : CastSpellAction(ai, "incinerate") {}
    };

    class CastFearAction : public CastRangedDebuffSpellAction
    {
    public:
        CastFearAction(PlayerbotAI* ai) : CastRangedDebuffSpellAction(ai, "fear") {}
    };

    class CastFearOnCcAction : public CastCrowdControlSpellAction
    {
    public:
        CastFearOnCcAction(PlayerbotAI* ai) : CastCrowdControlSpellAction(ai, "fear") {}
    };

    class CastLifeTapAction: public CastSpellAction
    {
    public:
        CastLifeTapAction(PlayerbotAI* ai) : CastSpellAction(ai, "life tap") {}
        virtual string GetTargetName() { return "self target"; }
        virtual bool isUseful() { return AI_VALUE2(uint8, "health", "self target") > sPlayerbotAIConfig.lowHealth; }
    };

    class CastAmplifyCurseAction : public CastBuffSpellAction
    {
    public:
        CastAmplifyCurseAction(PlayerbotAI* ai) : CastBuffSpellAction(ai, "amplify curse") {}
    };

    class CastSiphonLifeAction : public CastRangedDebuffSpellAction
    {
    public:
        CastSiphonLifeAction(PlayerbotAI* ai) : CastRangedDebuffSpellAction(ai, "siphon life") {}
    };

    class CastSiphonLifeOnAttackerAction : public CastRangedDebuffSpellOnAttackerAction
    {
    public:
        CastSiphonLifeOnAttackerAction(PlayerbotAI* ai) : CastRangedDebuffSpellOnAttackerAction(ai, "siphon life") {}
    };

    class CastHowlOfTerrorAction : public CastMeleeAoeSpellAction
    {
    public:
		CastHowlOfTerrorAction(PlayerbotAI* ai) : CastMeleeAoeSpellAction(ai, "howl of terror", 10.0f) {}
    };

	SPELL_ACTION(CastSearingPainAction, "searing pain");

    class UpdateWarlockPveStrategiesAction : public UpdateStrategyDependenciesAction
    {
    public:
        UpdateWarlockPveStrategiesAction(PlayerbotAI* ai) : UpdateStrategyDependenciesAction(ai, "update pve strats")
        {
            std::vector<std::string> strategiesRequired = { "demonology" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "demonology pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "demonology pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_DEAD, "demonology pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_REACTION, "demonology pve", strategiesRequired);

            strategiesRequired = { "demonology", "aoe" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "aoe demonology pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "aoe demonology pve", strategiesRequired);

            strategiesRequired = { "demonology", "cc" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "cc demonology pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "cc demonology pve", strategiesRequired);

            strategiesRequired = { "demonology", "pet" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "pet demonology pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "pet demonology pve", strategiesRequired);

            strategiesRequired = { "demonology", "buff" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "buff demonology pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "buff demonology pve", strategiesRequired);

            strategiesRequired = { "demonology", "curse" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "curse demonology pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "curse demonology pve", strategiesRequired);

            strategiesRequired = { "destruction" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "destruction pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "destruction pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_DEAD, "destruction pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_REACTION, "destruction pve", strategiesRequired);

            strategiesRequired = { "destruction", "aoe" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "aoe destruction pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "aoe destruction pve", strategiesRequired);

            strategiesRequired = { "destruction", "cc" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "cc destruction pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "cc destruction pve", strategiesRequired);

            strategiesRequired = { "destruction", "pet" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "pet destruction pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "pet destruction pve", strategiesRequired);

            strategiesRequired = { "destruction", "buff" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "buff destruction pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "buff destruction pve", strategiesRequired);

            strategiesRequired = { "destruction", "curse" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "curse destruction pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "curse destruction pve", strategiesRequired);

            strategiesRequired = { "affliction" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "affliction pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "affliction pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_DEAD, "affliction pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_REACTION, "affliction pve", strategiesRequired);

            strategiesRequired = { "affliction", "aoe" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "aoe affliction pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "aoe affliction pve", strategiesRequired);

            strategiesRequired = { "affliction", "cc" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "cc affliction pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "cc affliction pve", strategiesRequired);

            strategiesRequired = { "affliction", "pet" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "pet affliction pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "pet affliction pve", strategiesRequired);

            strategiesRequired = { "affliction", "buff" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "buff affliction pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "buff affliction pve", strategiesRequired);

            strategiesRequired = { "affliction", "curse" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "curse affliction pve", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "curse affliction pve", strategiesRequired);
        }
    };

    class UpdateWarlockPvpStrategiesAction : public UpdateStrategyDependenciesAction
    {
    public:
        UpdateWarlockPvpStrategiesAction(PlayerbotAI* ai) : UpdateStrategyDependenciesAction(ai, "update pvp strats")
        {
            std::vector<std::string> strategiesRequired = { "demonology" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "demonology pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "demonology pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_DEAD, "demonology pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_REACTION, "demonology pvp", strategiesRequired);

            strategiesRequired = { "demonology", "aoe" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "aoe demonology pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "aoe demonology pvp", strategiesRequired);

            strategiesRequired = { "demonology", "cc" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "cc demonology pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "cc demonology pvp", strategiesRequired);

            strategiesRequired = { "demonology", "pet" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "pet demonology pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "pet demonology pvp", strategiesRequired);

            strategiesRequired = { "demonology", "buff" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "buff demonology pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "buff demonology pvp", strategiesRequired);

            strategiesRequired = { "demonology", "curse" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "curse demonology pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "curse demonology pvp", strategiesRequired);

            strategiesRequired = { "destruction" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "destruction pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "destruction pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_DEAD, "destruction pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_REACTION, "destruction pvp", strategiesRequired);

            strategiesRequired = { "destruction", "aoe" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "aoe destruction pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "aoe destruction pvp", strategiesRequired);

            strategiesRequired = { "destruction", "cc" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "cc destruction pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "cc destruction pvp", strategiesRequired);

            strategiesRequired = { "destruction", "pet" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "pet destruction pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "pet destruction pvp", strategiesRequired);

            strategiesRequired = { "destruction", "buff" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "buff destruction pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "buff destruction pvp", strategiesRequired);

            strategiesRequired = { "destruction", "curse" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "curse destruction pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "curse destruction pvp", strategiesRequired);

            strategiesRequired = { "affliction" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "affliction pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "affliction pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_DEAD, "affliction pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_REACTION, "affliction pvp", strategiesRequired);

            strategiesRequired = { "affliction", "aoe" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "aoe affliction pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "aoe affliction pvp", strategiesRequired);

            strategiesRequired = { "affliction", "cc" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "cc affliction pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "cc affliction pvp", strategiesRequired);

            strategiesRequired = { "affliction", "pet" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "pet affliction pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "pet affliction pvp", strategiesRequired);

            strategiesRequired = { "affliction", "buff" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "buff affliction pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "buff affliction pvp", strategiesRequired);

            strategiesRequired = { "affliction", "curse" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "curse affliction pvp", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "curse affliction pvp", strategiesRequired);
        }
    };

    class UpdateWarlockRaidStrategiesAction : public UpdateStrategyDependenciesAction
    {
    public:
        UpdateWarlockRaidStrategiesAction(PlayerbotAI* ai) : UpdateStrategyDependenciesAction(ai, "update raid strats")
        {
            std::vector<std::string> strategiesRequired = { "demonology" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "demonology raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "demonology raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_DEAD, "demonology raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_REACTION, "demonology raid", strategiesRequired);

            strategiesRequired = { "demonology", "aoe" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "aoe demonology raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "aoe demonology raid", strategiesRequired);

            strategiesRequired = { "demonology", "cc" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "cc demonology raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "cc demonology raid", strategiesRequired);

            strategiesRequired = { "demonology", "pet" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "pet demonology raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "pet demonology raid", strategiesRequired);

            strategiesRequired = { "demonology", "buff" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "buff demonology raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "buff demonology raid", strategiesRequired);

            strategiesRequired = { "demonology", "curse" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "curse demonology raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "curse demonology raid", strategiesRequired);

            strategiesRequired = { "destruction" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "destruction raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "destruction raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_DEAD, "destruction raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_REACTION, "destruction raid", strategiesRequired);

            strategiesRequired = { "destruction", "aoe" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "aoe destruction raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "aoe destruction raid", strategiesRequired);

            strategiesRequired = { "destruction", "cc" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "cc destruction raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "cc destruction raid", strategiesRequired);

            strategiesRequired = { "destruction", "pet" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "pet destruction raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "pet destruction raid", strategiesRequired);

            strategiesRequired = { "destruction", "buff" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "buff destruction raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "buff destruction raid", strategiesRequired);

            strategiesRequired = { "destruction", "curse" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "curse destruction raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "curse destruction raid", strategiesRequired);

            strategiesRequired = { "affliction" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "affliction raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "affliction raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_DEAD, "affliction raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_REACTION, "affliction raid", strategiesRequired);

            strategiesRequired = { "affliction", "aoe" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "aoe affliction raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "aoe affliction raid", strategiesRequired);

            strategiesRequired = { "affliction", "cc" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "cc affliction raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "cc affliction raid", strategiesRequired);

            strategiesRequired = { "affliction", "pet" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "pet affliction raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "pet affliction raid", strategiesRequired);

            strategiesRequired = { "affliction", "buff" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "buff affliction raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "buff affliction raid", strategiesRequired);

            strategiesRequired = { "affliction", "curse" };
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_COMBAT, "curse affliction raid", strategiesRequired);
            strategiesToUpdate.emplace_back(BotState::BOT_STATE_NON_COMBAT, "curse affliction raid", strategiesRequired);
        }
    };
}
