#include "botpch.h"
#include "../../playerbot.h"
#include "WarriorActions.h"
#include "WarriorAiObjectContext.h"
#include "GenericWarriorNonCombatStrategy.h"
#include "TankWarriorStrategy.h"
#include "ArmsWarriorStrategy.h"
#include "FuryWarriorStrategy.h"
#include "../generic/PullStrategy.h"
#include "WarriorTriggers.h"
#include "../NamedObjectContext.h"

using namespace ai;


namespace ai
{
    namespace warrior
    {
        using namespace ai;

        class StrategyFactoryInternal : public NamedObjectContext<Strategy>
        {
        public:
            StrategyFactoryInternal()
            {
                creators["nc"] = &warrior::StrategyFactoryInternal::nc;
                creators["pull"] = &warrior::StrategyFactoryInternal::pull;
                creators["arms aoe"] = &warrior::StrategyFactoryInternal::arms_aoe;
                creators["fury aoe"] = &warrior::StrategyFactoryInternal::fury_aoe;
                creators["tank aoe"] = &warrior::StrategyFactoryInternal::tank_aoe;
            }

        private:
            static Strategy* nc(PlayerbotAI* ai) { return new GenericWarriorNonCombatStrategy(ai); }
            static Strategy* arms_aoe(PlayerbotAI* ai) { return new ArmsWarrirorAoeStrategy(ai); }
            static Strategy* fury_aoe(PlayerbotAI* ai) { return new FuryWarrirorAoeStrategy(ai); }
            static Strategy* tank_aoe(PlayerbotAI* ai) { return new TankWarrirorAoeStrategy(ai); }
            static Strategy* pull(PlayerbotAI* ai) { return new PullStrategy(ai, "shoot"); }
        };

        class CombatStrategyFactoryInternal : public NamedObjectContext<Strategy>
        {
        public:
            CombatStrategyFactoryInternal() : NamedObjectContext<Strategy>(false, true)
            {
                creators["tank"] = &warrior::CombatStrategyFactoryInternal::tank;
                creators["arms"] = &warrior::CombatStrategyFactoryInternal::arms;
                creators["fury"] = &warrior::CombatStrategyFactoryInternal::fury;
            }

        private:
            static Strategy* tank(PlayerbotAI* ai) { return new TankWarriorStrategy(ai); }
            static Strategy* arms(PlayerbotAI* ai) { return new ArmsWarriorStrategy(ai); }
            static Strategy* fury(PlayerbotAI* ai) { return new FuryWarriorStrategy(ai); }
        };
    };
};

namespace ai
{
    namespace warrior
    {
        using namespace ai;

        class TriggerFactoryInternal : public NamedObjectContext<Trigger>
        {
        public:
            TriggerFactoryInternal()
            {
                creators["hamstring"] = &TriggerFactoryInternal::hamstring;
                creators["victory rush"] = &TriggerFactoryInternal::victory_rush;
                creators["death wish"] = &TriggerFactoryInternal::death_wish;
                creators["battle shout"] = &TriggerFactoryInternal::battle_shout;
                creators["rend"] = &TriggerFactoryInternal::rend;
                creators["rend on attacker"] = &TriggerFactoryInternal::rend_on_attacker;
                creators["bloodrage"] = &TriggerFactoryInternal::bloodrage;
                creators["shield bash"] = &TriggerFactoryInternal::shield_bash;
                creators["disarm"] = &TriggerFactoryInternal::disarm;
                creators["concussion blow"] = &TriggerFactoryInternal::concussion_blow;
                creators["sword and board"] = &TriggerFactoryInternal::SwordAndBoard;
                creators["shield bash on enemy healer"] = &TriggerFactoryInternal::shield_bash_on_enemy_healer;
                creators["battle stance"] = &TriggerFactoryInternal::battle_stance;
                creators["defensive stance"] = &TriggerFactoryInternal::defensive_stance;
                creators["berserker stance"] = &TriggerFactoryInternal::berserker_stance;
                creators["shield block"] = &TriggerFactoryInternal::shield_block;
                creators["sunder armor"] = &TriggerFactoryInternal::sunder_armor;
                creators["revenge"] = &TriggerFactoryInternal::revenge;
                creators["overpower"] = &TriggerFactoryInternal::overpower;
                creators["mocking blow"] = &TriggerFactoryInternal::mocking_blow;
                creators["rampage"] = &TriggerFactoryInternal::rampage;
                creators["mortal strike"] = &TriggerFactoryInternal::mortal_strike;
                creators["thunder clap on snare target"] = &TriggerFactoryInternal::thunder_clap_on_snare_target;
                creators["thunder clap"] = &TriggerFactoryInternal::thunder_clap;
                creators["bloodthirst"] = &TriggerFactoryInternal::bloodthirst;
                creators["berserker rage"] = &TriggerFactoryInternal::berserker_rage;
                creators["pummel on enemy healer"] = &TriggerFactoryInternal::pummel_on_enemy_healer;
                creators["pummel"] = &TriggerFactoryInternal::pummel;
                creators["intercept on enemy healer"] = &TriggerFactoryInternal::intercept_on_enemy_healer;
                creators["intercept"] = &TriggerFactoryInternal::intercept;
                creators["taunt on snare target"] = &TriggerFactoryInternal::taunt_on_snare_target;
                creators["commanding shout"] = &TriggerFactoryInternal::commanding_shout;
                creators["intercept on snare target"] = &TriggerFactoryInternal::intercept_on_snare_target;
                creators["spell reflection"] = &TriggerFactoryInternal::spell_reflection;
                creators["sudden death"] = &TriggerFactoryInternal::sudden_death;
                creators["instant slam"] = &TriggerFactoryInternal::instant_slam;
                creators["shockwave"] = &TriggerFactoryInternal::shockwave;
                creators["shockwave on snare target"] = &TriggerFactoryInternal::shockwave_on_snare_target;
                creators["taste for blood"] = &TriggerFactoryInternal::taste_for_blood;
            }

        private:
            static Trigger* taste_for_blood(PlayerbotAI* ai) { return new TasteForBloodTrigger(ai); }
            static Trigger* shockwave_on_snare_target(PlayerbotAI* ai) { return new ShockwaveSnareTrigger(ai); }
            static Trigger* shockwave(PlayerbotAI* ai) { return new ShockwaveTrigger(ai); }
            static Trigger* instant_slam(PlayerbotAI* ai) { return new SlamInstantTrigger(ai); }
            static Trigger* sudden_death(PlayerbotAI* ai) { return new SuddenDeathTrigger(ai); }
            static Trigger* spell_reflection(PlayerbotAI* ai) { return new SpellReflectionTrigger(ai); }
            static Trigger* intercept_on_snare_target(PlayerbotAI* ai) { return new InterceptSnareTrigger(ai); }
            static Trigger* commanding_shout(PlayerbotAI* ai) { return new CommandingShoutTrigger(ai); }
            static Trigger* taunt_on_snare_target(PlayerbotAI* ai) { return new TauntSnareTrigger(ai); }
            static Trigger* intercept(PlayerbotAI* ai) { return new InterceptInterruptSpellTrigger(ai); }
            static Trigger* intercept_on_enemy_healer(PlayerbotAI* ai) { return new InterceptInterruptEnemyHealerSpellTrigger(ai); }
            static Trigger* pummel(PlayerbotAI* ai) { return new PummelInterruptSpellTrigger(ai); }
            static Trigger* pummel_on_enemy_healer(PlayerbotAI* ai) { return new PummelInterruptEnemyHealerSpellTrigger(ai); }
            static Trigger* berserker_rage(PlayerbotAI* ai) { return new BerserkerRageBuffTrigger(ai); }
            static Trigger* bloodthirst(PlayerbotAI* ai) { return new BloodthirstBuffTrigger(ai); }
            static Trigger* thunder_clap_on_snare_target(PlayerbotAI* ai) { return new ThunderClapSnareTrigger(ai); }
            static Trigger* thunder_clap(PlayerbotAI* ai) { return new ThunderClapTrigger(ai); }
            static Trigger* mortal_strike(PlayerbotAI* ai) { return new MortalStrikeDebuffTrigger(ai); }
            static Trigger* rampage(PlayerbotAI* ai) { return new RampageAvailableTrigger(ai); }
            static Trigger* mocking_blow(PlayerbotAI* ai) { return new MockingBlowTrigger(ai); }
            static Trigger* overpower(PlayerbotAI* ai) { return new OverpowerAvailableTrigger(ai); }
            static Trigger* revenge(PlayerbotAI* ai) { return new RevengeAvailableTrigger(ai); }
            static Trigger* sunder_armor(PlayerbotAI* ai) { return new SunderArmorDebuffTrigger(ai); }
            static Trigger* shield_block(PlayerbotAI* ai) { return new ShieldBlockTrigger(ai); }
            static Trigger* berserker_stance(PlayerbotAI* ai) { return new BerserkerStanceTrigger(ai); }
            static Trigger* defensive_stance(PlayerbotAI* ai) { return new DefensiveStanceTrigger(ai); }
            static Trigger* battle_stance(PlayerbotAI* ai) { return new BattleStanceTrigger(ai); }
            static Trigger* hamstring(PlayerbotAI* ai) { return new HamstringTrigger(ai); }
            static Trigger* victory_rush(PlayerbotAI* ai) { return new VictoryRushTrigger(ai); }
            static Trigger* death_wish(PlayerbotAI* ai) { return new DeathWishTrigger(ai); }
            static Trigger* battle_shout(PlayerbotAI* ai) { return new BattleShoutTrigger(ai); }
            static Trigger* rend(PlayerbotAI* ai) { return new RendDebuffTrigger(ai); }
            static Trigger* rend_on_attacker(PlayerbotAI* ai) { return new RendDebuffOnAttackerTrigger(ai); }
            static Trigger* bloodrage(PlayerbotAI* ai) { return new BloodrageBuffTrigger(ai); }
            static Trigger* shield_bash(PlayerbotAI* ai) { return new ShieldBashInterruptSpellTrigger(ai); }
            static Trigger* disarm(PlayerbotAI* ai) { return new DisarmDebuffTrigger(ai); }
            static Trigger* concussion_blow(PlayerbotAI* ai) { return new ConcussionBlowTrigger(ai); }
            static Trigger* SwordAndBoard(PlayerbotAI* ai) { return new SwordAndBoardTrigger(ai); }
            static Trigger* shield_bash_on_enemy_healer(PlayerbotAI* ai) { return new ShieldBashInterruptEnemyHealerSpellTrigger(ai); }
        };
    };
};


namespace ai
{
    namespace warrior
    {
        using namespace ai;

        class AiObjectContextInternal : public NamedObjectContext<Action>
        {
        public:
            AiObjectContextInternal()
            {
                creators["devastate"] = &AiObjectContextInternal::devastate;
                creators["overpower"] = &AiObjectContextInternal::overpower;
                creators["charge"] = &AiObjectContextInternal::charge;
                creators["bloodthirst"] = &AiObjectContextInternal::bloodthirst;
                creators["rend"] = &AiObjectContextInternal::rend;
                creators["rend on attacker"] = &AiObjectContextInternal::rend_on_attacker;
                creators["mocking blow"] = &AiObjectContextInternal::mocking_blow;
                creators["death wish"] = &AiObjectContextInternal::death_wish;
                creators["berserker rage"] = &AiObjectContextInternal::berserker_rage;
                creators["victory rush"] = &AiObjectContextInternal::victory_rush;
                creators["execute"] = &AiObjectContextInternal::execute;
                creators["defensive stance"] = &AiObjectContextInternal::defensive_stance;
                creators["hamstring"] = &AiObjectContextInternal::hamstring;
                creators["shield bash"] = &AiObjectContextInternal::shield_bash;
                creators["shield block"] = &AiObjectContextInternal::shield_block;
                creators["bloodrage"] = &AiObjectContextInternal::bloodrage;
                creators["battle stance"] = &AiObjectContextInternal::battle_stance;
                creators["heroic strike"] = &AiObjectContextInternal::heroic_strike;
                creators["intimidating shout"] = &AiObjectContextInternal::intimidating_shout;
                creators["demoralizing shout"] = &AiObjectContextInternal::demoralizing_shout;
                creators["challenging shout"] = &AiObjectContextInternal::challenging_shout;
                creators["shield wall"] = &AiObjectContextInternal::shield_wall;
                creators["battle shout"] = &AiObjectContextInternal::battle_shout;
                creators["battle shout taunt"] = &AiObjectContextInternal::battle_shout_taunt;
                creators["thunder clap"] = &AiObjectContextInternal::thunder_clap;
                creators["taunt"] = &AiObjectContextInternal::taunt;
                creators["revenge"] = &AiObjectContextInternal::revenge;
                creators["slam"] = &AiObjectContextInternal::slam;
                creators["shield slam"] = &AiObjectContextInternal::shield_slam;
                creators["disarm"] = &AiObjectContextInternal::disarm;
                creators["sunder armor"] = &AiObjectContextInternal::sunder_armor;
                creators["last stand"] = &AiObjectContextInternal::last_stand;
                creators["shockwave"] = &AiObjectContextInternal::shockwave;
                creators["shockwave on snare target"] = &AiObjectContextInternal::shockwave_on_snare_target;
                creators["cleave"] = &AiObjectContextInternal::cleave;
                creators["concussion blow"] = &AiObjectContextInternal::concussion_blow;
                creators["shield bash on enemy healer"] = &AiObjectContextInternal::shield_bash_on_enemy_healer;
                creators["berserker stance"] = &AiObjectContextInternal::berserker_stance;
                creators["commanding shout"] = &AiObjectContextInternal::commanding_shout;
                creators["retaliation"] = &AiObjectContextInternal::retaliation;
                creators["mortal strike"] = &AiObjectContextInternal::mortal_strike;
                creators["sweeping strikes"] = &AiObjectContextInternal::sweeping_strikes;
                creators["intercept"] = &AiObjectContextInternal::intercept;
                creators["whirlwind"] = &AiObjectContextInternal::whirlwind;
                creators["pummel"] = &AiObjectContextInternal::pummel;
                creators["pummel on enemy healer"] = &AiObjectContextInternal::pummel_on_enemy_healer;
                creators["recklessness"] = &AiObjectContextInternal::recklessness;
                creators["piercing howl"] = &AiObjectContextInternal::piercing_howl;
                creators["rampage"] = &AiObjectContextInternal::rampage;
                creators["intervene"] = &AiObjectContextInternal::intervene;
                creators["spell reflection"] = &AiObjectContextInternal::spell_reflection;
                creators["thunder clap on snare target"] = &AiObjectContextInternal::thunder_clap_on_snare_target;
                creators["taunt on snare target"] = &AiObjectContextInternal::taunt_on_snare_target;
                creators["intercept on enemy healer"] = &AiObjectContextInternal::intercept_on_enemy_healer;
                creators["intercept on snare target"] = &AiObjectContextInternal::intercept_on_snare_target;
                creators["bladestorm"] = &AiObjectContextInternal::bladestorm;
                creators["heroic throw"] = &AiObjectContextInternal::heroic_throw;
                creators["heroic throw on snare target"] = &AiObjectContextInternal::heroic_throw_on_snare_target;
                creators["shattering throw"] = &AiObjectContextInternal::shattering_throw;

            }

        private:
            static Action* shattering_throw(PlayerbotAI* ai) { return new CastShatteringThrowAction(ai); }
            static Action* heroic_throw_on_snare_target(PlayerbotAI* ai) { return new CastHeroicThrowSnareAction(ai); }
            static Action* heroic_throw(PlayerbotAI* ai) { return new CastHeroicThrowAction(ai); }
            static Action* bladestorm(PlayerbotAI* ai) { return new CastBladestormAction(ai); }
            static Action* intercept_on_snare_target(PlayerbotAI* ai) { return new CastInterceptOnSnareTargetAction(ai); }
            static Action* intercept_on_enemy_healer(PlayerbotAI* ai) { return new CastInterceptOnEnemyHealerAction(ai); }
            static Action* taunt_on_snare_target(PlayerbotAI* ai) { return new CastTauntOnSnareTargetAction(ai); }
            static Action* thunder_clap_on_snare_target(PlayerbotAI* ai) { return new CastThunderClapSnareAction(ai); }
            static Action* berserker_stance(PlayerbotAI* ai) { return new CastBerserkerStanceAction(ai); }
            static Action* commanding_shout(PlayerbotAI* ai) { return new CastCommandingShoutAction(ai); }
            static Action* retaliation(PlayerbotAI* ai) { return new CastRetaliationAction(ai); }
            static Action* mortal_strike(PlayerbotAI* ai) { return new CastMortalStrikeAction(ai); }
            static Action* sweeping_strikes(PlayerbotAI* ai) { return new CastSweepingStrikesAction(ai); }
            static Action* intercept(PlayerbotAI* ai) { return new CastInterceptAction(ai); }
            static Action* whirlwind(PlayerbotAI* ai) { return new CastWhirlwindAction(ai); }
            static Action* pummel(PlayerbotAI* ai) { return new CastPummelAction (ai); }
            static Action* pummel_on_enemy_healer(PlayerbotAI* ai) { return new CastPummelOnEnemyHealerAction(ai); }
            static Action* recklessness(PlayerbotAI* ai) { return new CastRecklessnessAction(ai); }
            static Action* piercing_howl(PlayerbotAI* ai) { return new CastPiercingHowlAction(ai); }
            static Action* rampage(PlayerbotAI* ai) { return new CastRampageAction(ai); }
            static Action* intervene(PlayerbotAI* ai) { return new CastInterveneAction(ai); }
            static Action* spell_reflection(PlayerbotAI* ai) { return new CastSpellReflectionAction(ai); }
            static Action* devastate(PlayerbotAI* ai) { return new CastDevastateAction(ai); }
            static Action* last_stand(PlayerbotAI* ai) { return new CastLastStandAction(ai); }
            static Action* shockwave(PlayerbotAI* ai) { return new CastShockwaveAction(ai); }
            static Action* shockwave_on_snare_target(PlayerbotAI* ai) { return new CastShockwaveSnareAction(ai); }
            static Action* cleave(PlayerbotAI* ai) { return new CastCleaveAction(ai); }
            static Action* concussion_blow(PlayerbotAI* ai) { return new CastConcussionBlowAction(ai); }
            static Action* taunt(PlayerbotAI* ai) { return new CastTauntAction(ai); }
            static Action* revenge(PlayerbotAI* ai) { return new CastRevengeAction(ai); }
            static Action* slam(PlayerbotAI* ai) { return new CastSlamAction(ai); }
            static Action* shield_slam(PlayerbotAI* ai) { return new CastShieldSlamAction(ai); }
            static Action* disarm(PlayerbotAI* ai) { return new CastDisarmAction(ai); }
            static Action* sunder_armor(PlayerbotAI* ai) { return new CastSunderArmorAction(ai); }
            static Action* overpower(PlayerbotAI* ai) { return new CastOverpowerAction(ai); }
            static Action* charge(PlayerbotAI* ai) { return new CastChargeAction(ai); }
            static Action* bloodthirst(PlayerbotAI* ai) { return new CastBloodthirstAction(ai); }
            static Action* rend(PlayerbotAI* ai) { return new CastRendAction(ai); }
            static Action* rend_on_attacker(PlayerbotAI* ai) { return new CastRendOnAttackerAction(ai); }
            static Action* mocking_blow(PlayerbotAI* ai) { return new CastMockingBlowAction(ai); }
            static Action* death_wish(PlayerbotAI* ai) { return new CastDeathWishAction(ai); }
            static Action* berserker_rage(PlayerbotAI* ai) { return new CastBerserkerRageAction(ai); }
            static Action* victory_rush(PlayerbotAI* ai) { return new CastVictoryRushAction(ai); }
            static Action* execute(PlayerbotAI* ai) { return new CastExecuteAction(ai); }
            static Action* defensive_stance(PlayerbotAI* ai) { return new CastDefensiveStanceAction(ai); }
            static Action* hamstring(PlayerbotAI* ai) { return new CastHamstringAction(ai); }
            static Action* shield_bash(PlayerbotAI* ai) { return new CastShieldBashAction(ai); }
            static Action* shield_block(PlayerbotAI* ai) { return new CastShieldBlockAction(ai); }
            static Action* bloodrage(PlayerbotAI* ai) { return new CastBloodrageAction(ai); }
            static Action* battle_stance(PlayerbotAI* ai) { return new CastBattleStanceAction(ai); }
            static Action* heroic_strike(PlayerbotAI* ai) { return new CastHeroicStrikeAction(ai); }
            static Action* intimidating_shout(PlayerbotAI* ai) { return new CastIntimidatingShoutAction(ai); }
            static Action* demoralizing_shout(PlayerbotAI* ai) { return new CastDemoralizingShoutAction(ai); }
            static Action* challenging_shout(PlayerbotAI* ai) { return new CastChallengingShoutAction(ai); }
            static Action* shield_wall(PlayerbotAI* ai) { return new CastShieldWallAction(ai); }
            static Action* battle_shout(PlayerbotAI* ai) { return new CastBattleShoutAction(ai); }
            static Action* battle_shout_taunt(PlayerbotAI* ai) { return new CastBattleShoutTauntAction(ai); }
            static Action* thunder_clap(PlayerbotAI* ai) { return new CastThunderClapAction(ai); }
            static Action* shield_bash_on_enemy_healer(PlayerbotAI* ai) { return new CastShieldBashOnEnemyHealerAction(ai); }

        };
    };
};

WarriorAiObjectContext::WarriorAiObjectContext(PlayerbotAI* ai) : AiObjectContext(ai)
{
    strategyContexts.Add(new ai::warrior::StrategyFactoryInternal());
    strategyContexts.Add(new ai::warrior::CombatStrategyFactoryInternal());
    actionContexts.Add(new ai::warrior::AiObjectContextInternal());
    triggerContexts.Add(new ai::warrior::TriggerFactoryInternal());
}
