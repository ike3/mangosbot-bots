#include "botpch.h"
#include "../../playerbot.h"
#include "ShamanActions.h"
#include "ShamanAiObjectContext.h"
#include "ShamanTriggers.h"
#include "../NamedObjectContext.h"
#include "ElementalShamanStrategy.h"
#include "RestorationShamanStrategy.h"
#include "EnhancementShamanStrategy.h"

namespace ai
{
    namespace shaman
    {
        class StrategyFactoryInternal : public NamedObjectContext<Strategy>
        {
        public:
            StrategyFactoryInternal()
            {
                creators["aoe"] = &shaman::StrategyFactoryInternal::aoe;
                creators["cure"] = &shaman::StrategyFactoryInternal::cure;
                creators["pull"] = &shaman::StrategyFactoryInternal::pull;
            }

        private:
            static Strategy* aoe(PlayerbotAI* ai) { return new ShamanAoePlaceholderStrategy(ai); }
            static Strategy* cure(PlayerbotAI* ai) { return new ShamanCurePlaceholderStrategy(ai); }
            static Strategy* pull(PlayerbotAI* ai) { return new PullStrategy(ai, "lightning bolt"); }            
        };

        class AoePvePvpStrategyFactoryInternal : public NamedObjectContext<Strategy>
        {
        public:
            AoePvePvpStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
            {
                creators["aoe elemental pve"] = &shaman::AoePvePvpStrategyFactoryInternal::aoe_elemental_pve;
                creators["aoe elemental pvp"] = &shaman::AoePvePvpStrategyFactoryInternal::aoe_elemental_pvp;
                creators["aoe elemental boss"] = &shaman::AoePvePvpStrategyFactoryInternal::aoe_elemental_boss;
                creators["aoe restoration pve"] = &shaman::AoePvePvpStrategyFactoryInternal::aoe_restoration_pve;
                creators["aoe restoration pvp"] = &shaman::AoePvePvpStrategyFactoryInternal::aoe_restoration_pvp;
                creators["aoe restoration boss"] = &shaman::AoePvePvpStrategyFactoryInternal::aoe_restoration_boss;
                creators["aoe enhancement pve"] = &shaman::AoePvePvpStrategyFactoryInternal::aoe_enhancement_pve;
                creators["aoe enhancement pvp"] = &shaman::AoePvePvpStrategyFactoryInternal::aoe_enhancement_pvp;
                creators["aoe enhancement boss"] = &shaman::AoePvePvpStrategyFactoryInternal::aoe_enhancement_boss;
            }

        private:
            static Strategy* aoe_elemental_pve(PlayerbotAI* ai) { return new ElementalShamanAoePveStrategy(ai); }
            static Strategy* aoe_elemental_pvp(PlayerbotAI* ai) { return new ElementalShamanAoePvpStrategy(ai); }
            static Strategy* aoe_elemental_boss(PlayerbotAI* ai) { return new ElementalShamanAoeBossStrategy(ai); }
            static Strategy* aoe_restoration_pve(PlayerbotAI* ai) { return new RestorationShamanAoePveStrategy(ai); }
            static Strategy* aoe_restoration_pvp(PlayerbotAI* ai) { return new RestorationShamanAoePvpStrategy(ai); }
            static Strategy* aoe_restoration_boss(PlayerbotAI* ai) { return new RestorationShamanAoeBossStrategy(ai); }
            static Strategy* aoe_enhancement_pve(PlayerbotAI* ai) { return new EnhancementShamanAoePveStrategy(ai); }
            static Strategy* aoe_enhancement_pvp(PlayerbotAI* ai) { return new EnhancementShamanAoePvpStrategy(ai); }
            static Strategy* aoe_enhancement_boss(PlayerbotAI* ai) { return new EnhancementShamanAoeBossStrategy(ai); }
        };

        class CurePvePvpStrategyFactoryInternal : public NamedObjectContext<Strategy>
        {
        public:
            CurePvePvpStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
            {
                creators["cure elemental pve"] = &shaman::CurePvePvpStrategyFactoryInternal::cure_elemental_pve;
                creators["cure elemental pvp"] = &shaman::CurePvePvpStrategyFactoryInternal::cure_elemental_pvp;
                creators["cure elemental boss"] = &shaman::CurePvePvpStrategyFactoryInternal::cure_elemental_boss;
                creators["cure restoration pve"] = &shaman::CurePvePvpStrategyFactoryInternal::cure_restoration_pve;
                creators["cure restoration pvp"] = &shaman::CurePvePvpStrategyFactoryInternal::cure_restoration_pvp;
                creators["cure restoration boss"] = &shaman::CurePvePvpStrategyFactoryInternal::cure_restoration_boss;
                creators["cure enhancement pve"] = &shaman::CurePvePvpStrategyFactoryInternal::cure_enhancement_pve;
                creators["cure enhancement pvp"] = &shaman::CurePvePvpStrategyFactoryInternal::cure_enhancement_pvp;
                creators["cure enhancement boss"] = &shaman::CurePvePvpStrategyFactoryInternal::cure_enhancement_boss;
            }

        private:
            static Strategy* cure_elemental_pve(PlayerbotAI* ai) { return new ElementalShamanCurePveStrategy(ai); }
            static Strategy* cure_elemental_pvp(PlayerbotAI* ai) { return new ElementalShamanCurePvpStrategy(ai); }
            static Strategy* cure_elemental_boss(PlayerbotAI* ai) { return new ElementalShamanCureBossStrategy(ai); }
            static Strategy* cure_restoration_pve(PlayerbotAI* ai) { return new RestorationShamanCurePveStrategy(ai); }
            static Strategy* cure_restoration_pvp(PlayerbotAI* ai) { return new RestorationShamanCurePvpStrategy(ai); }
            static Strategy* cure_restoration_boss(PlayerbotAI* ai) { return new RestorationShamanCureBossStrategy(ai); }
            static Strategy* cure_enhancement_pve(PlayerbotAI* ai) { return new EnhancementShamanCurePveStrategy(ai); }
            static Strategy* cure_enhancement_pvp(PlayerbotAI* ai) { return new EnhancementShamanCurePvpStrategy(ai); }
            static Strategy* cure_enhancement_boss(PlayerbotAI* ai) { return new EnhancementShamanCureBossStrategy(ai); }
        };

        class TotemStrategyFactoryInternal : public NamedObjectContext<Strategy>
        {
        public:
            TotemStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
            {
                creators["totems"] = &shaman::TotemStrategyFactoryInternal::totems;
                creators["totembar elements"] = &shaman::TotemStrategyFactoryInternal::totem_bar_elements;
                creators["totembar ancestors"] = &shaman::TotemStrategyFactoryInternal::totem_bar_ancestors;
                creators["totembar spirits"] = &shaman::TotemStrategyFactoryInternal::totem_bar_spirits;
            }

        private:
            static Strategy* totems(PlayerbotAI* ai) { return new ShamanTotemsPlaceholderStrategy(ai); }         
            static Strategy* totem_bar_elements(PlayerbotAI* ai) { return new ShamanTotemBarElementsStrategy(ai); }
            static Strategy* totem_bar_ancestors(PlayerbotAI* ai) { return new ShamanTotemBarAncestorsStrategy(ai); }
            static Strategy* totem_bar_spirits(PlayerbotAI* ai) { return new ShamanTotemBarSpiritsStrategy(ai); }
        };

        class TotemsPvePvpStrategyFactoryInternal : public NamedObjectContext<Strategy>
        {
        public:
            TotemsPvePvpStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
            {
                creators["totems elemental pve"] = &shaman::TotemsPvePvpStrategyFactoryInternal::totems_elemental_pve;
                creators["totems elemental pvp"] = &shaman::TotemsPvePvpStrategyFactoryInternal::totems_elemental_pvp;
                creators["totems elemental boss"] = &shaman::TotemsPvePvpStrategyFactoryInternal::totems_elemental_boss;
                creators["totems restoration pve"] = &shaman::TotemsPvePvpStrategyFactoryInternal::totems_restoration_pve;
                creators["totems restoration pvp"] = &shaman::TotemsPvePvpStrategyFactoryInternal::totems_restoration_pvp;
                creators["totems restoration boss"] = &shaman::TotemsPvePvpStrategyFactoryInternal::totems_restoration_boss;
                creators["totems enhancement pve"] = &shaman::TotemsPvePvpStrategyFactoryInternal::totems_enhancement_pve;
                creators["totems enhancement pvp"] = &shaman::TotemsPvePvpStrategyFactoryInternal::totems_enhancement_pvp;
                creators["totems enhancement boss"] = &shaman::TotemsPvePvpStrategyFactoryInternal::totems_enhancement_boss;
            }

        private:
            static Strategy* totems_elemental_pve(PlayerbotAI* ai) { return new ElementalShamanTotemsPveStrategy(ai); }
            static Strategy* totems_elemental_pvp(PlayerbotAI* ai) { return new ElementalShamanTotemsPvpStrategy(ai); }
            static Strategy* totems_elemental_boss(PlayerbotAI* ai) { return new ElementalShamanTotemsBossStrategy(ai); }
            static Strategy* totems_restoration_pve(PlayerbotAI* ai) { return new RestorationShamanTotemsPveStrategy(ai); }
            static Strategy* totems_restoration_pvp(PlayerbotAI* ai) { return new RestorationShamanTotemsPvpStrategy(ai); }
            static Strategy* totems_restoration_boss(PlayerbotAI* ai) { return new RestorationShamanTotemsBossStrategy(ai); }
            static Strategy* totems_enhancement_pve(PlayerbotAI* ai) { return new EnhancementShamanTotemsPveStrategy(ai); }
            static Strategy* totems_enhancement_pvp(PlayerbotAI* ai) { return new EnhancementShamanTotemsPvpStrategy(ai); }
            static Strategy* totems_enhancement_boss(PlayerbotAI* ai) { return new EnhancementShamanTotemsBossStrategy(ai); }
        };

        class BuffStrategyFactoryInternal : public NamedObjectContext<Strategy>
        {
        public:
            BuffStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
            {
                creators["bmana"] = &shaman::BuffStrategyFactoryInternal::bmana;
                creators["bdps"] = &shaman::BuffStrategyFactoryInternal::bdps;
            }

        private:
            static Strategy* bmana(PlayerbotAI* ai) { return new ShamanBuffManaStrategy(ai); }
            static Strategy* bdps(PlayerbotAI* ai) { return new ShamanBuffDpsStrategy(ai); }
        };

        class ClassStrategyFactoryInternal : public NamedObjectContext<Strategy>
        {
        public:
            ClassStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
            {
                creators["restoration"] = &shaman::ClassStrategyFactoryInternal::restoration;
                creators["enhancement"] = &shaman::ClassStrategyFactoryInternal::enhancement;
                creators["elemental"] = &shaman::ClassStrategyFactoryInternal::elemental;
            }

        private:
            static Strategy* restoration(PlayerbotAI* ai) { return new RestorationShamanPlaceholderStrategy(ai); }
            static Strategy* enhancement(PlayerbotAI* ai) { return new EnhancementShamanPlaceholderStrategy(ai); }
            static Strategy* elemental(PlayerbotAI* ai) { return new ElementalShamanPlaceholderStrategy(ai); }
        };

        class ClassPvePvpStrategyFactoryInternal : public NamedObjectContext<Strategy>
        {
        public:
            ClassPvePvpStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
            {
                creators["elemental pvp"] = &shaman::ClassPvePvpStrategyFactoryInternal::elemental_pvp;
                creators["elemental pve"] = &shaman::ClassPvePvpStrategyFactoryInternal::elemental_pve;
                creators["elemental boss"] = &shaman::ClassPvePvpStrategyFactoryInternal::elemental_boss;
                creators["restoration pvp"] = &shaman::ClassPvePvpStrategyFactoryInternal::restoration_pvp;
                creators["restoration pve"] = &shaman::ClassPvePvpStrategyFactoryInternal::restoration_pve;
                creators["restoration boss"] = &shaman::ClassPvePvpStrategyFactoryInternal::restoration_boss;
                creators["enhancement pvp"] = &shaman::ClassPvePvpStrategyFactoryInternal::enhancement_pvp;
                creators["enhancement pve"] = &shaman::ClassPvePvpStrategyFactoryInternal::enhancement_pve;
                creators["enhancement boss"] = &shaman::ClassPvePvpStrategyFactoryInternal::enhancement_boss;
            }

        private:
            static Strategy* elemental_pvp(PlayerbotAI* ai) { return new ElementalShamanPvpStrategy(ai); }
            static Strategy* elemental_pve(PlayerbotAI* ai) { return new ElementalShamanPveStrategy(ai); }
            static Strategy* elemental_boss(PlayerbotAI* ai) { return new ElementalShamanBossStrategy(ai); }
            static Strategy* restoration_pvp(PlayerbotAI* ai) { return new RestorationShamanPvpStrategy(ai); }
            static Strategy* restoration_pve(PlayerbotAI* ai) { return new RestorationShamanPveStrategy(ai); }
            static Strategy* restoration_boss(PlayerbotAI* ai) { return new RestorationShamanBossStrategy(ai); }
            static Strategy* enhancement_pvp(PlayerbotAI* ai) { return new EnhancementShamanPvpStrategy(ai); }
            static Strategy* enhancement_pve(PlayerbotAI* ai) { return new EnhancementShamanPveStrategy(ai); }
            static Strategy* enhancement_boss(PlayerbotAI* ai) { return new EnhancementShamanBossStrategy(ai); }
        };
    };
};

namespace ai
{
    namespace shaman
    {
        using namespace ai;

        class TriggerFactoryInternal : public NamedObjectContext<Trigger>
        {
        public:
            TriggerFactoryInternal()
            {
                creators["wind shear"] = &TriggerFactoryInternal::wind_shear;
                creators["purge"] = &TriggerFactoryInternal::purge;
                creators["shaman weapon"] = &TriggerFactoryInternal::shaman_weapon;
                creators["water shield"] = &TriggerFactoryInternal::water_shield;
                creators["lightning shield"] = &TriggerFactoryInternal::lightning_shield;
                creators["water breathing"] = &TriggerFactoryInternal::water_breathing;
                creators["water walking"] = &TriggerFactoryInternal::water_walking;
                creators["water breathing on party"] = &TriggerFactoryInternal::water_breathing_on_party;
                creators["water walking on party"] = &TriggerFactoryInternal::water_walking_on_party;
                creators["cleanse spirit poison"] = &TriggerFactoryInternal::cleanse_poison;
                creators["cleanse spirit curse"] = &TriggerFactoryInternal::cleanse_curse;
                creators["cleanse spirit disease"] = &TriggerFactoryInternal::cleanse_disease;
                creators["party member cleanse spirit poison"] = &TriggerFactoryInternal::party_member_cleanse_poison;
                creators["party member cleanse spirit curse"] = &TriggerFactoryInternal::party_member_cleanse_curse;
                creators["party member cleanse spirit disease"] = &TriggerFactoryInternal::party_member_cleanse_disease;
                creators["shock"] = &TriggerFactoryInternal::shock;
                creators["frost shock snare"] = &TriggerFactoryInternal::frost_shock_snare;
                creators["heroism"] = &TriggerFactoryInternal::heroism;
                creators["bloodlust"] = &TriggerFactoryInternal::bloodlust;
                creators["maelstrom weapon"] = &TriggerFactoryInternal::maelstrom_weapon;
                creators["wind shear on enemy healer"] = &TriggerFactoryInternal::wind_shear_on_enemy_healer;
                creators["cure poison"] = &TriggerFactoryInternal::cure_poison;
                creators["party member cure poison"] = &TriggerFactoryInternal::party_member_cure_poison;
                creators["cure disease"] = &TriggerFactoryInternal::cure_disease;
                creators["party member cure disease"] = &TriggerFactoryInternal::party_member_cure_disease;
                creators["fire totem"] = &TriggerFactoryInternal::fire_totem;
                creators["fire totem aoe"] = &TriggerFactoryInternal::fire_totem_aoe;
                creators["earth totem"] = &TriggerFactoryInternal::earth_totem;
                creators["water totem"] = &TriggerFactoryInternal::water_totem;
                creators["air totem"] = &TriggerFactoryInternal::air_totem;
                creators["call of the elements"] = &TriggerFactoryInternal::call_of_the_elements;
                creators["call of the ancestors"] = &TriggerFactoryInternal::call_of_the_ancestors;
                creators["call of the spirits"] = &TriggerFactoryInternal::call_of_the_spirits;
                creators["totemic recall"] = &TriggerFactoryInternal::totemic_recall;
            }

        private:
            static Trigger* maelstrom_weapon(PlayerbotAI* ai) { return new MaelstromWeaponTrigger(ai); }
            static Trigger* heroism(PlayerbotAI* ai) { return new HeroismTrigger(ai); }
            static Trigger* bloodlust(PlayerbotAI* ai) { return new BloodlustTrigger(ai); }
            static Trigger* party_member_cleanse_disease(PlayerbotAI* ai) { return new PartyMemberCleanseSpiritDiseaseTrigger(ai); }
            static Trigger* party_member_cleanse_curse(PlayerbotAI* ai) { return new PartyMemberCleanseSpiritCurseTrigger(ai); }
            static Trigger* party_member_cleanse_poison(PlayerbotAI* ai) { return new PartyMemberCleanseSpiritPoisonTrigger(ai); }
            static Trigger* cleanse_disease(PlayerbotAI* ai) { return new CleanseSpiritDiseaseTrigger(ai); }
            static Trigger* cleanse_curse(PlayerbotAI* ai) { return new CleanseSpiritCurseTrigger(ai); }
            static Trigger* cleanse_poison(PlayerbotAI* ai) { return new CleanseSpiritPoisonTrigger(ai); }
            static Trigger* water_breathing(PlayerbotAI* ai) { return new WaterBreathingTrigger(ai); }
            static Trigger* water_walking(PlayerbotAI* ai) { return new WaterWalkingTrigger(ai); }
            static Trigger* water_breathing_on_party(PlayerbotAI* ai) { return new WaterBreathingOnPartyTrigger(ai); }
            static Trigger* water_walking_on_party(PlayerbotAI* ai) { return new WaterWalkingOnPartyTrigger(ai); }
            static Trigger* wind_shear(PlayerbotAI* ai) { return new WindShearInterruptSpellTrigger(ai); }
            static Trigger* purge(PlayerbotAI* ai) { return new PurgeTrigger(ai); }
            static Trigger* shaman_weapon(PlayerbotAI* ai) { return new ShamanWeaponTrigger(ai); }
            static Trigger* water_shield(PlayerbotAI* ai) { return new WaterShieldTrigger(ai); }
            static Trigger* lightning_shield(PlayerbotAI* ai) { return new LightningShieldTrigger(ai); }
            static Trigger* shock(PlayerbotAI* ai) { return new ShockTrigger(ai); }
            static Trigger* frost_shock_snare(PlayerbotAI* ai) { return new FrostShockSnareTrigger(ai); }
            static Trigger* wind_shear_on_enemy_healer(PlayerbotAI* ai) { return new WindShearInterruptEnemyHealerSpellTrigger(ai); }
            static Trigger* cure_poison(PlayerbotAI* ai) { return new CurePoisonTrigger(ai); }
            static Trigger* party_member_cure_poison(PlayerbotAI* ai) { return new PartyMemberCurePoisonTrigger(ai); }
            static Trigger* cure_disease(PlayerbotAI* ai) { return new CureDiseaseTrigger(ai); }
            static Trigger* party_member_cure_disease(PlayerbotAI* ai) { return new PartyMemberCureDiseaseTrigger(ai); }
            static Trigger* fire_totem(PlayerbotAI* ai) { return new FireTotemTrigger(ai); }
            static Trigger* fire_totem_aoe(PlayerbotAI* ai) { return new FireTotemAoeTrigger(ai); }
            static Trigger* earth_totem(PlayerbotAI* ai) { return new EarthTotemTrigger(ai); }
            static Trigger* water_totem(PlayerbotAI* ai) { return new WaterTotemTrigger(ai); }
            static Trigger* air_totem(PlayerbotAI* ai) { return new AirTotemTrigger(ai); }
            static Trigger* call_of_the_elements(PlayerbotAI* ai) { return new TotemsAreNotSummonedTrigger(ai); }
            static Trigger* call_of_the_ancestors(PlayerbotAI* ai) { return new TotemsAreNotSummonedTrigger(ai); }
            static Trigger* call_of_the_spirits(PlayerbotAI* ai) { return new TotemsAreNotSummonedTrigger(ai); }
            static Trigger* totemic_recall(PlayerbotAI* ai) { return new ReadyToRemoveTotemsTrigger(ai); }
        };
    };
};

namespace ai
{
    namespace shaman
    {
        using namespace ai;

        class AiObjectContextInternal : public NamedObjectContext<Action>
        {
        public:
            AiObjectContextInternal()
            {
                creators["water shield"] = &AiObjectContextInternal::water_shield;
                creators["lightning shield"] = &AiObjectContextInternal::lightning_shield;
                creators["strength of earth totem"] = &AiObjectContextInternal::strength_of_earth_totem;
                creators["flametongue totem"] = &AiObjectContextInternal::flametongue_totem;
                creators["searing totem"] = &AiObjectContextInternal::searing_totem;
                creators["magma totem"] = &AiObjectContextInternal::magma_totem;
                creators["totem of wrath"] = &AiObjectContextInternal::totem_of_wrath;
                creators["windfury totem"] = &AiObjectContextInternal::windfury_totem;
                creators["grace of air totem"] = &AiObjectContextInternal::grace_of_air_totem;
                creators["mana spring totem"] = &AiObjectContextInternal::mana_spring_totem;
                creators["mana tide totem"] = &AiObjectContextInternal::mana_tide_totem;
                creators["earthbind totem"] = &AiObjectContextInternal::earthbind_totem;
                creators["healing stream totem"] = &AiObjectContextInternal::healing_stream_totem;
                creators["wrath of air totem"] = &AiObjectContextInternal::wrath_of_air_totem;
                creators["wind shear"] = &AiObjectContextInternal::wind_shear;
                creators["wind shear on enemy healer"] = &AiObjectContextInternal::wind_shear_on_enemy_healer;
                creators["rockbiter weapon"] = &AiObjectContextInternal::rockbiter_weapon;
                creators["flametongue weapon"] = &AiObjectContextInternal::flametongue_weapon;
                creators["frostbrand weapon"] = &AiObjectContextInternal::frostbrand_weapon;
                creators["windfury weapon"] = &AiObjectContextInternal::windfury_weapon;
                creators["earthliving weapon"] = &AiObjectContextInternal::earthliving_weapon;
                creators["purge"] = &AiObjectContextInternal::purge;
                creators["healing wave"] = &AiObjectContextInternal::healing_wave;
                creators["lesser healing wave"] = &AiObjectContextInternal::lesser_healing_wave;
                creators["healing wave on party"] = &AiObjectContextInternal::healing_wave_on_party;
                creators["lesser healing wave on party"] = &AiObjectContextInternal::lesser_healing_wave_on_party;
                creators["earth shield"] = &AiObjectContextInternal::earth_shield;
                creators["earth shield on party"] = &AiObjectContextInternal::earth_shield_on_party;
                creators["chain heal"] = &AiObjectContextInternal::chain_heal;
                creators["riptide"] = &AiObjectContextInternal::riptide;
                creators["riptide on party"] = &AiObjectContextInternal::riptide_on_party;
                creators["stormstrike"] = &AiObjectContextInternal::stormstrike;
                creators["lava lash"] = &AiObjectContextInternal::lava_lash;
                creators["fire nova"] = &AiObjectContextInternal::fire_nova;
                creators["ancestral spirit"] = &AiObjectContextInternal::ancestral_spirit;
                creators["water walking"] = &AiObjectContextInternal::water_walking;
                creators["water breathing"] = &AiObjectContextInternal::water_breathing;
                creators["water walking on party"] = &AiObjectContextInternal::water_walking_on_party;
                creators["water breathing on party"] = &AiObjectContextInternal::water_breathing_on_party;
                creators["cleanse spirit"] = &AiObjectContextInternal::cleanse_spirit;
                creators["cleanse spirit poison on party"] = &AiObjectContextInternal::cleanse_spirit_poison_on_party;
                creators["cleanse spirit disease on party"] = &AiObjectContextInternal::cleanse_spirit_disease_on_party;
                creators["cleanse spirit curse on party"] = &AiObjectContextInternal::cleanse_spirit_curse_on_party;
                creators["flame shock"] = &AiObjectContextInternal::flame_shock;
                creators["earth shock"] = &AiObjectContextInternal::earth_shock;
                creators["frost shock"] = &AiObjectContextInternal::frost_shock;
                creators["chain lightning"] = &AiObjectContextInternal::chain_lightning;
                creators["lightning bolt"] = &AiObjectContextInternal::lightning_bolt;
                creators["thunderstorm"] = &AiObjectContextInternal::thunderstorm;
                creators["heroism"] = &AiObjectContextInternal::heroism;
                creators["bloodlust"] = &AiObjectContextInternal::bloodlust;
                creators["cure disease"] = &AiObjectContextInternal::cure_disease;
                creators["cure disease on party"] = &AiObjectContextInternal::cure_disease_on_party;
                creators["cure poison"] = &AiObjectContextInternal::cure_poison;
                creators["cure poison on party"] = &AiObjectContextInternal::cure_poison_on_party;
                creators["ghost wolf"] = &AiObjectContextInternal::ghost_wolf;
                creators["call of the elements"] = &AiObjectContextInternal::call_of_the_elements;
                creators["call of the ancestors"] = &AiObjectContextInternal::call_of_the_ancestors;
                creators["call of the spirits"] = &AiObjectContextInternal::call_of_the_spirits;
                creators["totemic recall"] = &AiObjectContextInternal::totemic_recall;
                creators["set totembars on levelup"] = &AiObjectContextInternal::set_totembars;
                creators["update pve strats"] = &AiObjectContextInternal::update_pve_strats;
                creators["update pvp strats"] = &AiObjectContextInternal::update_pvp_strats;
                creators["update boss strats"] = &AiObjectContextInternal::update_boss_strats;
            }

        private:
            static Action* ghost_wolf(PlayerbotAI* ai) { return new CastGhostWolfAction(ai); }
            static Action* heroism(PlayerbotAI* ai) { return new CastHeroismAction(ai); }
            static Action* bloodlust(PlayerbotAI* ai) { return new CastBloodlustAction(ai); }
            static Action* thunderstorm(PlayerbotAI* ai) { return new CastThunderstormAction(ai); }
            static Action* lightning_bolt(PlayerbotAI* ai) { return new CastLightningBoltAction(ai); }
            static Action* chain_lightning(PlayerbotAI* ai) { return new CastChainLightningAction(ai); }
            static Action* frost_shock(PlayerbotAI* ai) { return new CastFrostShockAction(ai); }
            static Action* earth_shock(PlayerbotAI* ai) { return new CastEarthShockAction(ai); }
            static Action* flame_shock(PlayerbotAI* ai) { return new CastFlameShockAction(ai); }
            static Action* cleanse_spirit_poison_on_party(PlayerbotAI* ai) { return new CastCleanseSpiritPoisonOnPartyAction(ai); }
            static Action* cleanse_spirit_disease_on_party(PlayerbotAI* ai) { return new CastCleanseSpiritDiseaseOnPartyAction(ai); }
            static Action* cleanse_spirit_curse_on_party(PlayerbotAI* ai) { return new CastCleanseSpiritCurseOnPartyAction(ai); }
            static Action* cleanse_spirit(PlayerbotAI* ai) { return new CastCleanseSpiritAction(ai); }
            static Action* water_walking(PlayerbotAI* ai) { return new CastWaterWalkingAction(ai); }
            static Action* water_breathing(PlayerbotAI* ai) { return new CastWaterBreathingAction(ai); }
            static Action* water_walking_on_party(PlayerbotAI* ai) { return new CastWaterWalkingOnPartyAction(ai); }
            static Action* water_breathing_on_party(PlayerbotAI* ai) { return new CastWaterBreathingOnPartyAction(ai); }
            static Action* water_shield(PlayerbotAI* ai) { return new CastWaterShieldAction(ai); }
            static Action* lightning_shield(PlayerbotAI* ai) { return new CastLightningShieldAction(ai); }
            static Action* strength_of_earth_totem(PlayerbotAI* ai) { return new CastStrengthOfEarthTotemAction(ai); }
            static Action* flametongue_totem(PlayerbotAI* ai) { return new CastFlametongueTotemAction(ai); }
            static Action* magma_totem(PlayerbotAI* ai) { return new CastMagmaTotemAction(ai); }
            static Action* searing_totem(PlayerbotAI* ai) { return new CastSearingTotemAction(ai); }
            static Action* totem_of_wrath(PlayerbotAI* ai) { return new CastTotemOfWrathAction(ai); }
            static Action* fire_nova(PlayerbotAI* ai) { return new CastFireNovaAction(ai); }
            static Action* windfury_totem(PlayerbotAI* ai) { return new CastWindfuryTotemAction(ai); }
            static Action* grace_of_air_totem(PlayerbotAI* ai) { return new CastGraceOfAirTotemAction(ai); }
            static Action* mana_spring_totem(PlayerbotAI* ai) { return new CastManaSpringTotemAction(ai); }
            static Action* mana_tide_totem(PlayerbotAI* ai) { return new CastManaTideTotemAction(ai); }
            static Action* earthbind_totem(PlayerbotAI* ai) { return new CastEarthbindTotemAction(ai); }
            static Action* healing_stream_totem(PlayerbotAI* ai) { return new CastHealingStreamTotemAction(ai); }
            static Action* wrath_of_air_totem(PlayerbotAI* ai) { return new CastWrathOfAirTotemAction(ai); }
            static Action* wind_shear(PlayerbotAI* ai) { return new CastWindShearAction(ai); }
            static Action* rockbiter_weapon(PlayerbotAI* ai) { return new CastRockbiterWeaponAction(ai); }
            static Action* flametongue_weapon(PlayerbotAI* ai) { return new CastFlametongueWeaponAction(ai); }
            static Action* frostbrand_weapon(PlayerbotAI* ai) { return new CastFrostbrandWeaponAction(ai); }
            static Action* windfury_weapon(PlayerbotAI* ai) { return new CastWindfuryWeaponAction(ai); }
            static Action* earthliving_weapon(PlayerbotAI* ai) { return new CastEarthlivingWeaponAction(ai); }
            static Action* purge(PlayerbotAI* ai) { return new CastPurgeAction(ai); }
            static Action* healing_wave(PlayerbotAI* ai) { return new CastHealingWaveAction(ai); }
            static Action* lesser_healing_wave(PlayerbotAI* ai) { return new CastLesserHealingWaveAction(ai); }
            static Action* healing_wave_on_party(PlayerbotAI* ai) { return new CastHealingWaveOnPartyAction(ai); }
            static Action* lesser_healing_wave_on_party(PlayerbotAI* ai) { return new CastLesserHealingWaveOnPartyAction(ai); }
            static Action* earth_shield(PlayerbotAI* ai) { return new CastEarthShieldAction(ai); }
            static Action* earth_shield_on_party(PlayerbotAI* ai) { return new CastEarthShieldOnPartyAction(ai); }
            static Action* chain_heal(PlayerbotAI* ai) { return new CastChainHealAction(ai); }
            static Action* riptide(PlayerbotAI* ai) { return new CastRiptideAction(ai); }
            static Action* riptide_on_party(PlayerbotAI* ai) { return new CastRiptideOnPartyAction(ai); }
            static Action* stormstrike(PlayerbotAI* ai) { return new CastStormstrikeAction(ai); }
            static Action* lava_lash(PlayerbotAI* ai) { return new CastLavaLashAction(ai); }
            static Action* ancestral_spirit(PlayerbotAI* ai) { return new CastAncestralSpiritAction(ai); }
            static Action* wind_shear_on_enemy_healer(PlayerbotAI* ai) { return new CastWindShearOnEnemyHealerAction(ai); }
            static Action* cure_poison(PlayerbotAI* ai) { return new CastCurePoisonAction(ai); }
            static Action* cure_poison_on_party(PlayerbotAI* ai) { return new CastCurePoisonOnPartyAction(ai); }
            static Action* cure_disease(PlayerbotAI* ai) { return new CastCureDiseaseAction(ai); }
            static Action* cure_disease_on_party(PlayerbotAI* ai) { return new CastCureDiseaseOnPartyAction(ai); }
            static Action* call_of_the_elements(PlayerbotAI* ai) { return new CastCallOfTheElements(ai); }
            static Action* call_of_the_ancestors(PlayerbotAI* ai) { return new CastCallOfTheAncestors(ai); }
            static Action* call_of_the_spirits(PlayerbotAI* ai) { return new CastCallOfTheSpirits(ai); }
            static Action* totemic_recall(PlayerbotAI* ai) { return new CastTotemicRecall(ai); }
            static Action* set_totembars(PlayerbotAI* ai) { return new SetTotemBars(ai); }
            static Action* update_pve_strats(PlayerbotAI* ai) { return new UpdateShamanPveStrategiesAction(ai); }
            static Action* update_pvp_strats(PlayerbotAI* ai) { return new UpdateShamanPvpStrategiesAction(ai); }
            static Action* update_boss_strats(PlayerbotAI* ai) { return new UpdateShamanBossStrategiesAction(ai); }
        };
    };
};

ShamanAiObjectContext::ShamanAiObjectContext(PlayerbotAI* ai) : AiObjectContext(ai)
{
    strategyContexts.Add(new ai::shaman::StrategyFactoryInternal());
    strategyContexts.Add(new ai::shaman::TotemsPvePvpStrategyFactoryInternal());
    strategyContexts.Add(new ai::shaman::AoePvePvpStrategyFactoryInternal());
    strategyContexts.Add(new ai::shaman::CurePvePvpStrategyFactoryInternal());
    strategyContexts.Add(new ai::shaman::ClassStrategyFactoryInternal());
    strategyContexts.Add(new ai::shaman::ClassPvePvpStrategyFactoryInternal());
    strategyContexts.Add(new ai::shaman::BuffStrategyFactoryInternal());
    strategyContexts.Add(new ai::shaman::TotemStrategyFactoryInternal());
    actionContexts.Add(new ai::shaman::AiObjectContextInternal());
    triggerContexts.Add(new ai::shaman::TriggerFactoryInternal());
}
