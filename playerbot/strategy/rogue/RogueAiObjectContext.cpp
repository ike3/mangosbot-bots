#include "botpch.h"
#include "../../playerbot.h"
#include "RogueActions.h"
#include "RogueTriggers.h"
#include "RogueAiObjectContext.h"
#include "../generic/PullStrategy.h"
#include "../NamedObjectContext.h"
#include "CombatRogueStrategy.h"
#include "AssassinationRogueStrategy.h"
#include "SubtletyRogueStrategy.h"

namespace ai
{
    namespace rogue
    {
        class StrategyFactoryInternal : public NamedObjectContext<Strategy>
        {
        public:
            StrategyFactoryInternal()
            {
                creators["pull"] = &rogue::StrategyFactoryInternal::pull;
                creators["aoe"] = &rogue::StrategyFactoryInternal::aoe;
                creators["boost"] = &rogue::StrategyFactoryInternal::boost;
                creators["stealth"] = &rogue::StrategyFactoryInternal::stealth;
                creators["stealthed"] = &rogue::StrategyFactoryInternal::stealthed;
                creators["cc"] = &rogue::StrategyFactoryInternal::cc;
                creators["poisons"] = &rogue::StrategyFactoryInternal::poisons;
            }

        private:
            static Strategy* pull(PlayerbotAI* ai) { return new PullStrategy(ai, "shoot"); }
            static Strategy* aoe(PlayerbotAI* ai) { return new RogueAoePlaceholderStrategy(ai); }
            static Strategy* boost(PlayerbotAI* ai) { return new RogueBoostPlaceholderStrategy(ai); }
            static Strategy* stealth(PlayerbotAI* ai) { return new RogueStealthPlaceholderStrategy(ai); }
            static Strategy* stealthed(PlayerbotAI* ai) { return new RogueStealthedStrategy(ai); }
            static Strategy* cc(PlayerbotAI* ai) { return new RogueCcPlaceholderStrategy(ai); }
            static Strategy* poisons(PlayerbotAI* ai) { return new RoguePoisonsPlaceholderStrategy(ai); }
        };

        class AoeSituationStrategyFactoryInternal : public NamedObjectContext<Strategy>
        {
        public:
            AoeSituationStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
            {
                creators["aoe combat pve"] = &rogue::AoeSituationStrategyFactoryInternal::aoe_combat_pve;
                creators["aoe combat pvp"] = &rogue::AoeSituationStrategyFactoryInternal::aoe_combat_pvp;
                creators["aoe combat raid"] = &rogue::AoeSituationStrategyFactoryInternal::aoe_combat_raid;
                creators["aoe assassination pve"] = &rogue::AoeSituationStrategyFactoryInternal::aoe_assassination_pve;
                creators["aoe assassination pvp"] = &rogue::AoeSituationStrategyFactoryInternal::aoe_assassination_pvp;
                creators["aoe assassination raid"] = &rogue::AoeSituationStrategyFactoryInternal::aoe_assassination_raid;
                creators["aoe subtlety pve"] = &rogue::AoeSituationStrategyFactoryInternal::aoe_subtlety_pve;
                creators["aoe subtlety pvp"] = &rogue::AoeSituationStrategyFactoryInternal::aoe_subtlety_pvp;
                creators["aoe subtlety raid"] = &rogue::AoeSituationStrategyFactoryInternal::aoe_subtlety_raid;
            }

        private:
            static Strategy* aoe_combat_pve(PlayerbotAI* ai) { return new CombatRogueAoePveStrategy(ai); }
            static Strategy* aoe_combat_pvp(PlayerbotAI* ai) { return new CombatRogueAoePvpStrategy(ai); }
            static Strategy* aoe_combat_raid(PlayerbotAI* ai) { return new CombatRogueAoeRaidStrategy(ai); }
            static Strategy* aoe_assassination_pve(PlayerbotAI* ai) { return new AssassinationRogueAoePveStrategy(ai); }
            static Strategy* aoe_assassination_pvp(PlayerbotAI* ai) { return new AssassinationRogueAoePvpStrategy(ai); }
            static Strategy* aoe_assassination_raid(PlayerbotAI* ai) { return new AssassinationRogueAoeRaidStrategy(ai); }
            static Strategy* aoe_subtlety_pve(PlayerbotAI* ai) { return new SubtletyRogueAoePveStrategy(ai); }
            static Strategy* aoe_subtlety_pvp(PlayerbotAI* ai) { return new SubtletyRogueAoePvpStrategy(ai); }
            static Strategy* aoe_subtlety_raid(PlayerbotAI* ai) { return new SubtletyRogueAoeRaidStrategy(ai); }
        };

        class BoostSituationStrategyFactoryInternal : public NamedObjectContext<Strategy>
        {
        public:
            BoostSituationStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
            {
                creators["boost combat pve"] = &rogue::BoostSituationStrategyFactoryInternal::boost_combat_pve;
                creators["boost combat pvp"] = &rogue::BoostSituationStrategyFactoryInternal::boost_combat_pvp;
                creators["boost combat raid"] = &rogue::BoostSituationStrategyFactoryInternal::boost_combat_raid;
                creators["boost assassination pve"] = &rogue::BoostSituationStrategyFactoryInternal::boost_assassination_pve;
                creators["boost assassination pvp"] = &rogue::BoostSituationStrategyFactoryInternal::boost_assassination_pvp;
                creators["boost assassination raid"] = &rogue::BoostSituationStrategyFactoryInternal::boost_assassination_raid;
                creators["boost subtlety pve"] = &rogue::BoostSituationStrategyFactoryInternal::boost_subtlety_pve;
                creators["boost subtlety pvp"] = &rogue::BoostSituationStrategyFactoryInternal::boost_subtlety_pvp;
                creators["boost subtlety raid"] = &rogue::BoostSituationStrategyFactoryInternal::boost_subtlety_raid;
            }

        private:
            static Strategy* boost_combat_pve(PlayerbotAI* ai) { return new CombatRogueBoostPveStrategy(ai); }
            static Strategy* boost_combat_pvp(PlayerbotAI* ai) { return new CombatRogueBoostPvpStrategy(ai); }
            static Strategy* boost_combat_raid(PlayerbotAI* ai) { return new CombatRogueBoostRaidStrategy(ai); }
            static Strategy* boost_assassination_pve(PlayerbotAI* ai) { return new AssassinationRogueBoostPveStrategy(ai); }
            static Strategy* boost_assassination_pvp(PlayerbotAI* ai) { return new AssassinationRogueBoostPvpStrategy(ai); }
            static Strategy* boost_assassination_raid(PlayerbotAI* ai) { return new AssassinationRogueBoostRaidStrategy(ai); }
            static Strategy* boost_subtlety_pve(PlayerbotAI* ai) { return new SubtletyRogueBoostPveStrategy(ai); }
            static Strategy* boost_subtlety_pvp(PlayerbotAI* ai) { return new SubtletyRogueBoostPvpStrategy(ai); }
            static Strategy* boost_subtlety_raid(PlayerbotAI* ai) { return new SubtletyRogueBoostRaidStrategy(ai); }
        };

        class CcSituationStrategyFactoryInternal : public NamedObjectContext<Strategy>
        {
        public:
            CcSituationStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
            {
                creators["cc combat pve"] = &rogue::CcSituationStrategyFactoryInternal::cc_combat_pve;
                creators["cc combat pvp"] = &rogue::CcSituationStrategyFactoryInternal::cc_combat_pvp;
                creators["cc combat raid"] = &rogue::CcSituationStrategyFactoryInternal::cc_combat_raid;
                creators["cc assassination pve"] = &rogue::CcSituationStrategyFactoryInternal::cc_assassination_pve;
                creators["cc assassination pvp"] = &rogue::CcSituationStrategyFactoryInternal::cc_assassination_pvp;
                creators["cc assassination raid"] = &rogue::CcSituationStrategyFactoryInternal::cc_assassination_raid;
                creators["cc subtlety pve"] = &rogue::CcSituationStrategyFactoryInternal::cc_subtlety_pve;
                creators["cc subtlety pvp"] = &rogue::CcSituationStrategyFactoryInternal::cc_subtlety_pvp;
                creators["cc subtlety raid"] = &rogue::CcSituationStrategyFactoryInternal::cc_subtlety_raid;
            }

        private:
            static Strategy* cc_combat_pve(PlayerbotAI* ai) { return new CombatRogueCcPveStrategy(ai); }
            static Strategy* cc_combat_pvp(PlayerbotAI* ai) { return new CombatRogueCcPvpStrategy(ai); }
            static Strategy* cc_combat_raid(PlayerbotAI* ai) { return new CombatRogueCcRaidStrategy(ai); }
            static Strategy* cc_assassination_pve(PlayerbotAI* ai) { return new AssassinationRogueCcPveStrategy(ai); }
            static Strategy* cc_assassination_pvp(PlayerbotAI* ai) { return new AssassinationRogueCcPvpStrategy(ai); }
            static Strategy* cc_assassination_raid(PlayerbotAI* ai) { return new AssassinationRogueCcRaidStrategy(ai); }
            static Strategy* cc_subtlety_pve(PlayerbotAI* ai) { return new SubtletyRogueCcPveStrategy(ai); }
            static Strategy* cc_subtlety_pvp(PlayerbotAI* ai) { return new SubtletyRogueCcPvpStrategy(ai); }
            static Strategy* cc_subtlety_raid(PlayerbotAI* ai) { return new SubtletyRogueCcRaidStrategy(ai); }
        };

        class StealthSituationStrategyFactoryInternal : public NamedObjectContext<Strategy>
        {
        public:
            StealthSituationStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
            {
                creators["stealth combat pve"] = &rogue::StealthSituationStrategyFactoryInternal::stealth_combat_pve;
                creators["stealth combat pvp"] = &rogue::StealthSituationStrategyFactoryInternal::stealth_combat_pvp;
                creators["stealth combat raid"] = &rogue::StealthSituationStrategyFactoryInternal::stealth_combat_raid;
                creators["stealth assassination pve"] = &rogue::StealthSituationStrategyFactoryInternal::stealth_assassination_pve;
                creators["stealth assassination pvp"] = &rogue::StealthSituationStrategyFactoryInternal::stealth_assassination_pvp;
                creators["stealth assassination raid"] = &rogue::StealthSituationStrategyFactoryInternal::stealth_assassination_raid;
                creators["stealth subtlety pve"] = &rogue::StealthSituationStrategyFactoryInternal::stealth_subtlety_pve;
                creators["stealth subtlety pvp"] = &rogue::StealthSituationStrategyFactoryInternal::stealth_subtlety_pvp;
                creators["stealth subtlety raid"] = &rogue::StealthSituationStrategyFactoryInternal::stealth_subtlety_raid;
            }

        private:
            static Strategy* stealth_combat_pve(PlayerbotAI* ai) { return new CombatRogueStealthPveStrategy(ai); }
            static Strategy* stealth_combat_pvp(PlayerbotAI* ai) { return new CombatRogueStealthPvpStrategy(ai); }
            static Strategy* stealth_combat_raid(PlayerbotAI* ai) { return new CombatRogueStealthRaidStrategy(ai); }
            static Strategy* stealth_assassination_pve(PlayerbotAI* ai) { return new AssassinationRogueStealthPveStrategy(ai); }
            static Strategy* stealth_assassination_pvp(PlayerbotAI* ai) { return new AssassinationRogueStealthPvpStrategy(ai); }
            static Strategy* stealth_assassination_raid(PlayerbotAI* ai) { return new AssassinationRogueStealthRaidStrategy(ai); }
            static Strategy* stealth_subtlety_pve(PlayerbotAI* ai) { return new SubtletyRogueStealthPveStrategy(ai); }
            static Strategy* stealth_subtlety_pvp(PlayerbotAI* ai) { return new SubtletyRogueStealthPvpStrategy(ai); }
            static Strategy* stealth_subtlety_raid(PlayerbotAI* ai) { return new SubtletyRogueStealthRaidStrategy(ai); }
        };

        class PoisonsSituationStrategyFactoryInternal : public NamedObjectContext<Strategy>
        {
        public:
            PoisonsSituationStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
            {
                creators["poisons combat pve"] = &rogue::PoisonsSituationStrategyFactoryInternal::poisons_combat_pve;
                creators["poisons combat pvp"] = &rogue::PoisonsSituationStrategyFactoryInternal::poisons_combat_pvp;
                creators["poisons combat raid"] = &rogue::PoisonsSituationStrategyFactoryInternal::poisons_combat_raid;
                creators["poisons assassination pve"] = &rogue::PoisonsSituationStrategyFactoryInternal::poisons_assassination_pve;
                creators["poisons assassination pvp"] = &rogue::PoisonsSituationStrategyFactoryInternal::poisons_assassination_pvp;
                creators["poisons assassination raid"] = &rogue::PoisonsSituationStrategyFactoryInternal::poisons_assassination_raid;
                creators["poisons subtlety pve"] = &rogue::PoisonsSituationStrategyFactoryInternal::poisons_subtlety_pve;
                creators["poisons subtlety pvp"] = &rogue::PoisonsSituationStrategyFactoryInternal::poisons_subtlety_pvp;
                creators["poisons subtlety raid"] = &rogue::PoisonsSituationStrategyFactoryInternal::poisons_subtlety_raid;
            }

        private:
            static Strategy* poisons_combat_pve(PlayerbotAI* ai) { return new CombatRoguePoisonsPveStrategy(ai); }
            static Strategy* poisons_combat_pvp(PlayerbotAI* ai) { return new CombatRoguePoisonsPvpStrategy(ai); }
            static Strategy* poisons_combat_raid(PlayerbotAI* ai) { return new CombatRoguePoisonsRaidStrategy(ai); }
            static Strategy* poisons_assassination_pve(PlayerbotAI* ai) { return new AssassinationRoguePoisonsPveStrategy(ai); }
            static Strategy* poisons_assassination_pvp(PlayerbotAI* ai) { return new AssassinationRoguePoisonsPvpStrategy(ai); }
            static Strategy* poisons_assassination_raid(PlayerbotAI* ai) { return new AssassinationRoguePoisonsRaidStrategy(ai); }
            static Strategy* poisons_subtlety_pve(PlayerbotAI* ai) { return new SubtletyRoguePoisonsPveStrategy(ai); }
            static Strategy* poisons_subtlety_pvp(PlayerbotAI* ai) { return new SubtletyRoguePoisonsPvpStrategy(ai); }
            static Strategy* poisons_subtlety_raid(PlayerbotAI* ai) { return new SubtletyRoguePoisonsRaidStrategy(ai); }
        };

        class ClassStrategyFactoryInternal : public NamedObjectContext<Strategy>
        {
        public:
            ClassStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
            {
                creators["combat"] = &rogue::ClassStrategyFactoryInternal::combat;
                creators["subtlety"] = &rogue::ClassStrategyFactoryInternal::subtlety;
                creators["assassination"] = &rogue::ClassStrategyFactoryInternal::assassination;
            }

        private:
            static Strategy* combat(PlayerbotAI* ai) { return new CombatRoguePlaceholderStrategy(ai); }
            static Strategy* subtlety(PlayerbotAI* ai) { return new SubtletyRoguePlaceholderStrategy(ai); }
            static Strategy* assassination(PlayerbotAI* ai) { return new AssassinationRoguePlaceholderStrategy(ai); }
        };

        class ClassSituationStrategyFactoryInternal : public NamedObjectContext<Strategy>
        {
        public:
            ClassSituationStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
            {
                creators["combat pvp"] = &rogue::ClassSituationStrategyFactoryInternal::combat_pvp;
                creators["combat pve"] = &rogue::ClassSituationStrategyFactoryInternal::combat_pve;
                creators["combat raid"] = &rogue::ClassSituationStrategyFactoryInternal::combat_raid;
                creators["assassination pvp"] = &rogue::ClassSituationStrategyFactoryInternal::assassination_pvp;
                creators["assassination pve"] = &rogue::ClassSituationStrategyFactoryInternal::assassination_pve;
                creators["assassination raid"] = &rogue::ClassSituationStrategyFactoryInternal::assassination_raid;
                creators["subtlety pvp"] = &rogue::ClassSituationStrategyFactoryInternal::subtlety_pvp;
                creators["subtlety pve"] = &rogue::ClassSituationStrategyFactoryInternal::subtlety_pve;
                creators["subtlety raid"] = &rogue::ClassSituationStrategyFactoryInternal::subtlety_raid;
            }

        private:
            static Strategy* combat_pvp(PlayerbotAI* ai) { return new CombatRoguePvpStrategy(ai); }
            static Strategy* combat_pve(PlayerbotAI* ai) { return new CombatRoguePveStrategy(ai); }
            static Strategy* combat_raid(PlayerbotAI* ai) { return new CombatRogueRaidStrategy(ai); }
            static Strategy* assassination_pvp(PlayerbotAI* ai) { return new AssassinationRoguePvpStrategy(ai); }
            static Strategy* assassination_pve(PlayerbotAI* ai) { return new AssassinationRoguePveStrategy(ai); }
            static Strategy* assassination_raid(PlayerbotAI* ai) { return new AssassinationRogueRaidStrategy(ai); }
            static Strategy* subtlety_pvp(PlayerbotAI* ai) { return new SubtletyRoguePvpStrategy(ai); }
            static Strategy* subtlety_pve(PlayerbotAI* ai) { return new SubtletyRoguePveStrategy(ai); }
            static Strategy* subtlety_raid(PlayerbotAI* ai) { return new SubtletyRogueRaidStrategy(ai); }
        };
    };
};

namespace ai
{
    namespace rogue
    {
        class TriggerFactoryInternal : public NamedObjectContext<Trigger>
        {
        public:
            TriggerFactoryInternal()
            {
                creators["kick"] = &TriggerFactoryInternal::kick;
                creators["rupture"] = &TriggerFactoryInternal::rupture;
                creators["slice and dice"] = &TriggerFactoryInternal::slice_and_dice;
                creators["expose armor"] = &TriggerFactoryInternal::expose_armor;
                creators["kick on enemy healer"] = &TriggerFactoryInternal::kick_on_enemy_healer;
                creators["adrenaline rush"] = &TriggerFactoryInternal::adrenaline_rush;
                creators["unstealth"] = &TriggerFactoryInternal::unstealth;
                creators["sap"] = &TriggerFactoryInternal::sap;
                creators["in stealth"] = &TriggerFactoryInternal::in_stealth;
                creators["no stealth"] = &TriggerFactoryInternal::no_stealth;
                creators["stealth"] = &TriggerFactoryInternal::stealth;
                creators["sprint"] = &TriggerFactoryInternal::sprint;
                creators["2 combo"] = &TriggerFactoryInternal::combo2;
                creators["3 combo"] = &TriggerFactoryInternal::combo3;
                creators["4 combo"] = &TriggerFactoryInternal::combo3;
                creators["sinister strike"] = &TriggerFactoryInternal::sinister_strike;
                creators["hemorrhage"] = &TriggerFactoryInternal::hemorrhage;
                creators["killing spree"] = &TriggerFactoryInternal::killing_spree;
                creators["eviscerate"] = &TriggerFactoryInternal::eviscerate;
                creators["blade flurry"] = &TriggerFactoryInternal::blade_flurry;
                //creators["tricks of the trade on tank"] = &TriggerFactoryInternal::tricks_of_the_trade_on_tank;
                creators["cloak of shadows"] = &TriggerFactoryInternal::cloak_of_shadows;
                creators["fan of knives"] = &TriggerFactoryInternal::fan_of_knives;
                creators["riposte"] = &TriggerFactoryInternal::riposte;
                creators["apply deadly poison main hand"] = &TriggerFactoryInternal::apply_deadly_poison_main_hand;
                creators["apply crippling poison main hand"] = &TriggerFactoryInternal::apply_crippling_poison_main_hand;
                creators["apply mind poison main hand"] = &TriggerFactoryInternal::apply_mind_poison_main_hand;
                creators["apply instant poison main hand"] = &TriggerFactoryInternal::apply_instant_poison_main_hand;
                creators["apply wound poison main hand"] = &TriggerFactoryInternal::apply_wound_poison_main_hand;
                creators["apply anesthetic poison main hand"] = &TriggerFactoryInternal::apply_anesthetic_poison_main_hand;
                creators["apply deadly poison off hand"] = &TriggerFactoryInternal::apply_deadly_poison_off_hand;
                creators["apply crippling poison off hand"] = &TriggerFactoryInternal::apply_crippling_poison_off_hand;
                creators["apply mind poison off hand"] = &TriggerFactoryInternal::apply_mind_poison_off_hand;
                creators["apply instant poison off hand"] = &TriggerFactoryInternal::apply_instant_poison_off_hand;
                creators["apply wound poison off hand"] = &TriggerFactoryInternal::apply_wound_poison_off_hand;
                creators["apply anesthetic poison off hand"] = &TriggerFactoryInternal::apply_anesthetic_poison_off_hand;
            }

        private:
            static Trigger* riposte(PlayerbotAI* ai) { return new RiposteCastTrigger(ai); }
            static Trigger* adrenaline_rush(PlayerbotAI* ai) { return new RogueBoostBuffTrigger(ai, "adrenaline rush"); }
            static Trigger* kick(PlayerbotAI* ai) { return new KickInterruptSpellTrigger(ai); }
            static Trigger* rupture(PlayerbotAI* ai) { return new RuptureTrigger(ai); }
            static Trigger* slice_and_dice(PlayerbotAI* ai) { return new SliceAndDiceTrigger(ai); }
            static Trigger* expose_armor(PlayerbotAI* ai) { return new ExposeArmorTrigger(ai); }
            static Trigger* kick_on_enemy_healer(PlayerbotAI* ai) { return new KickInterruptEnemyHealerSpellTrigger(ai); }
            static Trigger* unstealth(PlayerbotAI* ai) { return new UnstealthTrigger(ai); }
            static Trigger* sap(PlayerbotAI* ai) { return new SapTrigger(ai); }
            static Trigger* in_stealth(PlayerbotAI* ai) { return new InStealthTrigger(ai); }
            static Trigger* no_stealth(PlayerbotAI* ai) { return new NoStealthTrigger(ai); }
            static Trigger* stealth(PlayerbotAI* ai) { return new StealthTrigger(ai); }
            static Trigger* sprint(PlayerbotAI* ai) { return new SprintTrigger(ai); }
            static Trigger* combo2(PlayerbotAI* ai) { return new ComboPointsAvailableTrigger(ai, 2); }
            static Trigger* combo3(PlayerbotAI* ai) { return new ComboPointsAvailableTrigger(ai, 3); }
            static Trigger* combo4(PlayerbotAI* ai) { return new ComboPointsAvailableTrigger(ai, 4); }
            static Trigger* sinister_strike(PlayerbotAI* ai) { return new SpellCanBeCastTrigger(ai, "sinister strike"); }
            static Trigger* hemorrhage(PlayerbotAI* ai) { return new SpellCanBeCastTrigger(ai, "hemorrhage"); }
            static Trigger* killing_spree(PlayerbotAI* ai) { return new RogueBoostBuffTrigger(ai, "killing spree"); }
            static Trigger* eviscerate(PlayerbotAI* ai) { return new EviscerateTrigger(ai); }
            static Trigger* blade_flurry(PlayerbotAI* ai) { return new RogueBoostBuffTrigger(ai, "blade flurry"); }
            //static Trigger* tricks_of_the_trade_on_tank(PlayerbotAI* ai) { return new TricksOfTheTradeOnTankTrigger(ai); }
            static Trigger* cloak_of_shadows(PlayerbotAI* ai) { return new CloakOfShadowsTrigger(ai); }
            static Trigger* fan_of_knives(PlayerbotAI* ai) { return new SpellCanBeCastTrigger(ai, "fan of knives"); }
            static Trigger* apply_deadly_poison_main_hand(PlayerbotAI* ai) { return new ApplyDeadlyPoisonTrigger(ai, true); }
            static Trigger* apply_crippling_poison_main_hand(PlayerbotAI* ai) { return new ApplyCripplingPoisonTrigger(ai, true); }
            static Trigger* apply_mind_poison_main_hand(PlayerbotAI* ai) { return new ApplyMindPoisonTrigger(ai, true); }
            static Trigger* apply_instant_poison_main_hand(PlayerbotAI* ai) { return new ApplyInstantPoisonTrigger(ai, true); }
            static Trigger* apply_wound_poison_main_hand(PlayerbotAI* ai) { return new ApplyWoundPoisonTrigger(ai, true); }
            static Trigger* apply_anesthetic_poison_main_hand(PlayerbotAI* ai) { return new ApplyAnestheticPoisonTrigger(ai, true); }
            static Trigger* apply_deadly_poison_off_hand(PlayerbotAI* ai) { return new ApplyDeadlyPoisonTrigger(ai, false); }
            static Trigger* apply_crippling_poison_off_hand(PlayerbotAI* ai) { return new ApplyCripplingPoisonTrigger(ai, false); }
            static Trigger* apply_mind_poison_off_hand(PlayerbotAI* ai) { return new ApplyMindPoisonTrigger(ai, false); }
            static Trigger* apply_instant_poison_off_hand(PlayerbotAI* ai) { return new ApplyInstantPoisonTrigger(ai, false); }
            static Trigger* apply_wound_poison_off_hand(PlayerbotAI* ai) { return new ApplyWoundPoisonTrigger(ai, false); }
            static Trigger* apply_anesthetic_poison_off_hand(PlayerbotAI* ai) { return new ApplyAnestheticPoisonTrigger(ai, false); }
        };
    };
};


namespace ai
{
    namespace rogue
    {
        class AiObjectContextInternal : public NamedObjectContext<Action>
        {
        public:
            AiObjectContextInternal()
            {
                creators["riposte"] = &AiObjectContextInternal::riposte;
                creators["mutilate"] = &AiObjectContextInternal::mutilate;
                creators["sinister strike"] = &AiObjectContextInternal::sinister_strike;
                creators["hemorrhage"] = &AiObjectContextInternal::hemorrhage;
                creators["ghostly strike"] = &AiObjectContextInternal::ghostly_strike;
                creators["gouge"] = &AiObjectContextInternal::gouge;
                creators["kidney shot"] = &AiObjectContextInternal::kidney_shot;
                creators["rupture"] = &AiObjectContextInternal::rupture;
                creators["slice and dice"] = &AiObjectContextInternal::slice_and_dice;
                creators["eviscerate"] = &AiObjectContextInternal::eviscerate;
                creators["vanish"] = &AiObjectContextInternal::vanish;
                creators["evasion"] = &AiObjectContextInternal::evasion;
                creators["kick"] = &AiObjectContextInternal::kick;
                creators["feint"] = &AiObjectContextInternal::feint;
                creators["backstab"] = &AiObjectContextInternal::backstab;
                creators["expose armor"] = &AiObjectContextInternal::expose_armor;
                creators["kick on enemy healer"] = &AiObjectContextInternal::kick_on_enemy_healer;
                creators["blade flurry"] = &AiObjectContextInternal::blade_flurry;
                creators["adrenaline rush"] = &AiObjectContextInternal::adrenaline_rush;
                creators["ambush"] = &AiObjectContextInternal::ambush;
                creators["stealth"] = &AiObjectContextInternal::stealth;
                creators["sprint"] = &AiObjectContextInternal::sprint;
                creators["garrote"] = &AiObjectContextInternal::garrote;
                creators["cheap shot"] = &AiObjectContextInternal::cheap_shot;
                creators["blind"] = &AiObjectContextInternal::blind;
                creators["unstealth"] = &AiObjectContextInternal::unstealth;
                creators["sap"] = &AiObjectContextInternal::sap;
                creators["check stealth"] = &AiObjectContextInternal::check_stealth;
                creators["killing spree"] = &AiObjectContextInternal::killing_spree;
                creators["tricks of the trade"] = &AiObjectContextInternal::tricks_of_the_trade;
                creators["cloak of shadows"] = &AiObjectContextInternal::cloak_of_shadows;
                //creators["fan of knives"] = &AiObjectContextInternal::fan_of_knives;
                creators["cold blood"] = &AiObjectContextInternal::cold_blood;
                creators["preparation"] = &AiObjectContextInternal::preparation;
                creators["premeditation"] = &AiObjectContextInternal::premeditation;
                creators["shadowstep"] = &AiObjectContextInternal::shadowstep;
                creators["update pve strats"] = &AiObjectContextInternal::update_pve_strats;
                creators["update pvp strats"] = &AiObjectContextInternal::update_pvp_strats;
                creators["update raid strats"] = &AiObjectContextInternal::update_raid_strats;
                creators["apply deadly poison main hand"] = &AiObjectContextInternal::apply_deadly_poison_main_hand;
                creators["apply crippling poison main hand"] = &AiObjectContextInternal::apply_crippling_poison_main_hand;
                creators["apply mind poison main hand"] = &AiObjectContextInternal::apply_mind_poison_main_hand;
                creators["apply instant poison main hand"] = &AiObjectContextInternal::apply_instant_poison_main_hand;
                creators["apply wound poison main hand"] = &AiObjectContextInternal::apply_wound_poison_main_hand;
                creators["apply anesthetic poison main hand"] = &AiObjectContextInternal::apply_anesthetic_poison_main_hand;
                creators["apply deadly poison off hand"] = &AiObjectContextInternal::apply_deadly_poison_off_hand;
                creators["apply crippling poison off hand"] = &AiObjectContextInternal::apply_crippling_poison_off_hand;
                creators["apply mind poison off hand"] = &AiObjectContextInternal::apply_mind_poison_off_hand;
                creators["apply instant poison off hand"] = &AiObjectContextInternal::apply_instant_poison_off_hand;
                creators["apply wound poison off hand"] = &AiObjectContextInternal::apply_wound_poison_off_hand;
                creators["apply anesthetic poison off hand"] = &AiObjectContextInternal::apply_anesthetic_poison_off_hand;
            }

        private:
            static Action* shadowstep(PlayerbotAI* ai) { return new CastShadowstepAction(ai); }
            static Action* premeditation(PlayerbotAI* ai) { return new CastPremeditationAction(ai); }
            static Action* preparation(PlayerbotAI* ai) { return new CastPreparationAction(ai); }
            static Action* cold_blood(PlayerbotAI* ai) { return new CastColdBloodAction(ai); }
            static Action* check_stealth(PlayerbotAI* ai) { return new CheckStealthAction(ai); }
            static Action* sap(PlayerbotAI* ai) { return new CastSapAction(ai); }
            static Action* unstealth(PlayerbotAI* ai) { return new UnstealthAction(ai); }
            static Action* blind(PlayerbotAI* ai) { return new CastBlindAction(ai); }
            static Action* ambush(PlayerbotAI* ai) { return new CastAmbushAction(ai); }
            static Action* stealth(PlayerbotAI* ai) { return new CastStealthAction(ai); }
            static Action* sprint(PlayerbotAI* ai) { return new CastSprintAction(ai); }
            static Action* garrote(PlayerbotAI* ai) { return new CastGarroteAction(ai); }
            static Action* cheap_shot(PlayerbotAI* ai) { return new CastCheapShotAction(ai); }
            static Action* adrenaline_rush(PlayerbotAI* ai) { return new CastAdrenalineRushAction(ai); }
            static Action* blade_flurry(PlayerbotAI* ai) { return new CastBladeFlurryAction(ai); }
            static Action* riposte(PlayerbotAI* ai) { return new CastRiposteAction(ai); }
            static Action* mutilate(PlayerbotAI* ai) { return new CastMutilateAction(ai); }
            static Action* sinister_strike(PlayerbotAI* ai) { return new CastSinisterStrikeAction(ai); }
            static Action* hemorrhage(PlayerbotAI* ai) { return new CastHemorrhageAction(ai); }
            static Action* ghostly_strike(PlayerbotAI* ai) { return new CastGhostlyStrikeAction(ai); }
            static Action* gouge(PlayerbotAI* ai) { return new CastGougeAction(ai); }
            static Action* kidney_shot(PlayerbotAI* ai) { return new CastKidneyShotAction(ai); }
            static Action* rupture(PlayerbotAI* ai) { return new CastRuptureAction(ai); }
            static Action* slice_and_dice(PlayerbotAI* ai) { return new CastSliceAndDiceAction(ai); }
            static Action* eviscerate(PlayerbotAI* ai) { return new CastEviscerateAction(ai); }
            static Action* vanish(PlayerbotAI* ai) { return new CastVanishAction(ai); }
            static Action* evasion(PlayerbotAI* ai) { return new CastEvasionAction(ai); }
            static Action* kick(PlayerbotAI* ai) { return new CastKickAction(ai); }
            static Action* feint(PlayerbotAI* ai) { return new CastFeintAction(ai); }
            static Action* backstab(PlayerbotAI* ai) { return new CastBackstabAction(ai); }
            static Action* expose_armor(PlayerbotAI* ai) { return new CastExposeArmorAction(ai); }
            static Action* kick_on_enemy_healer(PlayerbotAI* ai) { return new CastKickOnEnemyHealerAction(ai); }
            static Action* killing_spree(PlayerbotAI* ai) { return new CastKillingSpreeAction(ai); }
            static Action* tricks_of_the_trade(PlayerbotAI* ai) { return new CastTricksOfTheTradeOnPartyAction(ai); }
            static Action* cloak_of_shadows(PlayerbotAI* ai) { return new CastCloakOfShadowsAction(ai); }
            //static Action* fan_of_knives(PlayerbotAI* ai) { return new CastFanOfKnivesAction(ai); }
            static Action* update_pve_strats(PlayerbotAI* ai) { return new UpdateRoguePveStrategiesAction(ai); }
            static Action* update_pvp_strats(PlayerbotAI* ai) { return new UpdateRoguePvpStrategiesAction(ai); }
            static Action* update_raid_strats(PlayerbotAI* ai) { return new UpdateRogueRaidStrategiesAction(ai); }
            static Action* apply_deadly_poison_main_hand(PlayerbotAI* ai) { return new ApplyDeadlyPoisonAction(ai, true); }
            static Action* apply_crippling_poison_main_hand(PlayerbotAI* ai) { return new ApplyCripplingPoisonAction(ai, true); }
            static Action* apply_mind_poison_main_hand(PlayerbotAI* ai) { return new ApplyMindPoisonAction(ai, true); }
            static Action* apply_instant_poison_main_hand(PlayerbotAI* ai) { return new ApplyInstantPoisonAction(ai, true); }
            static Action* apply_wound_poison_main_hand(PlayerbotAI* ai) { return new ApplyWoundPoisonAction(ai, true); }
            static Action* apply_anesthetic_poison_main_hand(PlayerbotAI* ai) { return new ApplyAnestheticPoisonAction(ai, true); }
            static Action* apply_deadly_poison_off_hand(PlayerbotAI* ai) { return new ApplyDeadlyPoisonAction(ai, false); }
            static Action* apply_crippling_poison_off_hand(PlayerbotAI* ai) { return new ApplyCripplingPoisonAction(ai, false); }
            static Action* apply_mind_poison_off_hand(PlayerbotAI* ai) { return new ApplyMindPoisonAction(ai, false); }
            static Action* apply_instant_poison_off_hand(PlayerbotAI* ai) { return new ApplyInstantPoisonAction(ai, false); }
            static Action* apply_wound_poison_off_hand(PlayerbotAI* ai) { return new ApplyWoundPoisonAction(ai, false); }
            static Action* apply_anesthetic_poison_off_hand(PlayerbotAI* ai) { return new ApplyAnestheticPoisonAction(ai, false); }
        };
    };
};

RogueAiObjectContext::RogueAiObjectContext(PlayerbotAI* ai) : AiObjectContext(ai)
{
    strategyContexts.Add(new ai::rogue::StrategyFactoryInternal());
    strategyContexts.Add(new ai::rogue::AoeSituationStrategyFactoryInternal());
    strategyContexts.Add(new ai::rogue::BoostSituationStrategyFactoryInternal());
    strategyContexts.Add(new ai::rogue::CcSituationStrategyFactoryInternal());
    strategyContexts.Add(new ai::rogue::StealthSituationStrategyFactoryInternal());
    strategyContexts.Add(new ai::rogue::PoisonsSituationStrategyFactoryInternal());
    strategyContexts.Add(new ai::rogue::ClassStrategyFactoryInternal());
    strategyContexts.Add(new ai::rogue::ClassSituationStrategyFactoryInternal());
    actionContexts.Add(new ai::rogue::AiObjectContextInternal());
    triggerContexts.Add(new ai::rogue::TriggerFactoryInternal());
}
