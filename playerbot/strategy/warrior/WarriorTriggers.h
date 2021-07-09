#pragma once
#include "../triggers/GenericTriggers.h"

namespace ai
{
    BUFF_TRIGGER(BattleShoutTrigger, "battle shout");
    BUFF_TRIGGER(BattleStanceTrigger, "battle stance");
    BUFF_TRIGGER(DefensiveStanceTrigger, "defensive stance");
    BUFF_TRIGGER(BerserkerStanceTrigger, "berserker stance");
    BUFF_TRIGGER(ShieldBlockTrigger, "shield block");
    BUFF_TRIGGER(CommandingShoutTrigger, "commanding shout");

    DEBUFF_TRIGGER(RendDebuffTrigger, "rend");
    DEBUFF_TRIGGER(DisarmDebuffTrigger, "disarm");
    DEBUFF_TRIGGER(SunderArmorDebuffTrigger, "sunder armor");
    DEBUFF_TRIGGER(MortalStrikeDebuffTrigger, "mortal strike");
    DEBUFF_ENEMY_TRIGGER(RendDebuffOnAttackerTrigger, "rend");
    CAN_CAST_TRIGGER(RevengeAvailableTrigger, "revenge");
    CAN_CAST_TRIGGER(OverpowerAvailableTrigger, "overpower");
    CAN_CAST_TRIGGER(RampageAvailableTrigger, "rampage");
    BUFF_TRIGGER_A(BloodrageBuffTrigger, "bloodrage");
    HAS_AURA_TRIGGER(VictoryRushTrigger, "victory rush");
    HAS_AURA_TRIGGER(SwordAndBoardTrigger, "sword and board");
    SNARE_TRIGGER(ConcussionBlowTrigger, "concussion blow");
    SNARE_TRIGGER(HamstringTrigger, "hamstring");
    SNARE_TRIGGER(MockingBlowTrigger, "mocking blow");
    SNARE_TRIGGER(ThunderClapSnareTrigger, "thunder clap");
    SNARE_TRIGGER(TauntSnareTrigger, "taunt");
    SNARE_TRIGGER(InterceptSnareTrigger, "intercept");
    BOOST_TRIGGER(DeathWishTrigger, "death wish");
    BUFF_TRIGGER(BloodthirstBuffTrigger, "bloodthirst");
    BUFF_TRIGGER(BerserkerRageBuffTrigger, "berserker rage");
    INTERRUPT_HEALER_TRIGGER(ShieldBashInterruptEnemyHealerSpellTrigger, "shield bash");
    INTERRUPT_TRIGGER(ShieldBashInterruptSpellTrigger, "shield bash");
    INTERRUPT_HEALER_TRIGGER(PummelInterruptEnemyHealerSpellTrigger, "pummel");
    INTERRUPT_TRIGGER(PummelInterruptSpellTrigger, "pummel");
    INTERRUPT_HEALER_TRIGGER(InterceptInterruptEnemyHealerSpellTrigger, "intercept");
    INTERRUPT_TRIGGER(InterceptInterruptSpellTrigger, "intercept");
}
