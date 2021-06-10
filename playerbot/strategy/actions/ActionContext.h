#pragma once

#include "GenericActions.h"
#include "NonCombatActions.h"
#include "EmoteAction.h"
#include "AddLootAction.h"
#include "LootAction.h"
#include "AddLootAction.h"
#include "StayActions.h"
#include "FollowActions.h"
#include "ChangeStrategyAction.h"
#include "ChooseTargetActions.h"
#include "SuggestWhatToDoAction.h"
#include "PositionAction.h"
#include "AttackAction.h"
#include "CheckMailAction.h"
#include "CheckValuesAction.h"
#include "ChooseRpgTargetAction.h"
#include "ChooseTravelTargetAction.h"
#include "DelayAction.h"
#include "GiveItemAction.h"
#include "GreetAction.h"
#include "ImbueAction.h"
#include "MovementActions.h"
#include "MoveToRpgTargetAction.h"
#include "MoveToTravelTargetAction.h"
#include "OutfitAction.h"
#include "RevealGatheringItemAction.h"
#include "SayAction.h"
#include "OutfitAction.h"
#include "RandomBotUpdateAction.h"
#include "RemoveAuraAction.h"
#include "RpgAction.h"
#include "TravelAction.h"
#include "RtiAction.h"
#include "BattlegroundTactics.h"
#include "CheckMountStateAction.h"
#include "ChangeTalentsAction.h"
#include "AutoLearnSpellAction.h"
#include "XpGainAction.h"
#include "InviteToGroupAction.h"
#include "LeaveGroupAction.h"
#include "ReleaseSpiritAction.h"
#include "CombatActions.h"
#include "WorldBuffAction.h"
#include "CastCustomSpellAction.h"
#include "BattleGroundJoinAction.h"
#include "ChooseMoveDoAction.h"
#include "ActiveQuestActions.h"


namespace ai
{
    class ActionContext : public NamedObjectContext<Action>
    {
    public:
        ActionContext()
        {
            creators["mark rti"] = &ActionContext::mark_rti;
            creators["set return position"] = &ActionContext::set_return_position;
            creators["rpg"] = &ActionContext::rpg;
            creators["choose rpg target"] = &ActionContext::choose_rpg_target;
            creators["move to rpg target"] = &ActionContext::move_to_rpg_target;
			creators["travel"] = &ActionContext::travel;
			creators["choose travel target"] = &ActionContext::choose_travel_target;
			creators["move to travel target"] = &ActionContext::move_to_travel_target;
            creators["move out of collision"] = &ActionContext::move_out_of_collision;
            creators["move random"] = &ActionContext::move_random;
            creators["attack"] = &ActionContext::melee;
            creators["melee"] = &ActionContext::melee;
            creators["switch to melee"] = &ActionContext::switch_to_melee;
            creators["switch to ranged"] = &ActionContext::switch_to_ranged;
            creators["reach spell"] = &ActionContext::ReachSpell;
            creators["reach melee"] = &ActionContext::ReachMelee;
            creators["reach party member to heal"] = &ActionContext::reach_party_member_to_heal;
            creators["flee"] = &ActionContext::flee;
            creators["flee with pet"] = &ActionContext::flee_with_pet;
            creators["gift of the naaru"] = &ActionContext::gift_of_the_naaru;
            creators["shoot"] = &ActionContext::shoot;
            creators["lifeblood"] = &ActionContext::lifeblood;
            creators["arcane torrent"] = &ActionContext::arcane_torrent;
            creators["end pull"] = &ActionContext::end_pull;
            creators["healthstone"] = &ActionContext::healthstone;
            creators["healing potion"] = &ActionContext::healing_potion;
            creators["mana potion"] = &ActionContext::mana_potion;
            creators["food"] = &ActionContext::food;
            creators["drink"] = &ActionContext::drink;
            creators["tank assist"] = &ActionContext::tank_assist;
            creators["dps assist"] = &ActionContext::dps_assist;
            creators["dps aoe"] = &ActionContext::dps_aoe;
            creators["attack rti target"] = &ActionContext::attack_rti_target;
            creators["loot"] = &ActionContext::loot;
            creators["add loot"] = &ActionContext::add_loot;
            creators["add gathering loot"] = &ActionContext::add_gathering_loot;
            creators["add all loot"] = &ActionContext::add_all_loot;
            creators["release loot"] = &ActionContext::release_loot;
            creators["shoot"] = &ActionContext::shoot;
            creators["follow"] = &ActionContext::follow;
            creators["flee to master"] = &ActionContext::flee_to_master;
            creators["runaway"] = &ActionContext::runaway;
            creators["stay"] = &ActionContext::stay;
            creators["sit"] = &ActionContext::sit;
            creators["attack anything"] = &ActionContext::attack_anything;
            creators["attack least hp target"] = &ActionContext::attack_least_hp_target;
            creators["attack enemy player"] = &ActionContext::attack_enemy_player;
            creators["emote"] = &ActionContext::emote;
            creators["talk"] = &ActionContext::talk;
            creators["suggest what to do"] = &ActionContext::suggest_what_to_do;
            creators["suggest trade"] = &ActionContext::suggest_trade;
            creators["return"] = &ActionContext::_return;
            creators["move to loot"] = &ActionContext::move_to_loot;
            creators["open loot"] = &ActionContext::open_loot;
            creators["guard"] = &ActionContext::guard;
            creators["move out of enemy contact"] = &ActionContext::move_out_of_enemy_contact;
            creators["set facing"] = &ActionContext::set_facing;
            creators["set behind"] = &ActionContext::set_behind;
            creators["attack duel opponent"] = &ActionContext::attack_duel_opponent;
            creators["drop target"] = &ActionContext::drop_target;
            creators["check mail"] = &ActionContext::check_mail;
            creators["say"] = &ActionContext::say;
            creators["reveal gathering item"] = &ActionContext::reveal_gathering_item;
            creators["outfit"] = &ActionContext::outfit;
            creators["random bot update"] = &ActionContext::random_bot_update;
            creators["delay"] = &ActionContext::delay;
            creators["greet"] = &ActionContext::greet;
            creators["check values"] = &ActionContext::check_values;
            creators["ra"] = &ActionContext::ra;
            creators["apply poison"] = &ActionContext::apply_poison;
            creators["apply stone"] = &ActionContext::apply_stone;
            creators["apply oil"] = &ActionContext::apply_oil;
            creators["try emergency"] = &ActionContext::try_emergency;
            creators["give food"] = &ActionContext::give_food;
            creators["give water"] = &ActionContext::give_water;
            creators["mount"] = &ActionContext::mount;
            creators["war stomp"] = &ActionContext::war_stomp;
            creators["auto talents"] = &ActionContext::auto_talents;
			creators["auto learn spell"] = &ActionContext::auto_learn_spell;
            creators["xp gain"] = &ActionContext::xp_gain;
            creators["invite nearby"] = &ActionContext::invite_nearby;
            creators["leave far away"] = &ActionContext::leave_far_away;
            creators["move to dark portal"] = &ActionContext::move_to_dark_portal;
            creators["move from dark portal"] = &ActionContext::move_from_dark_portal;
            creators["use dark portal azeroth"] = &ActionContext::use_dark_portal_azeroth;
            creators["world buff"] = &ActionContext::world_buff;
            creators["cast random spell"] = &ActionContext::cast_random_spell;
            creators["continue action"] = &ActionContext::continue_action;
            creators["queue at bm"] = &ActionContext::queue_at_bm;
            creators["pick up quest"] = &ActionContext::pick_up_quest;
            creators["do quest objective"] = &ActionContext::do_quest_objective;
            creators["hand in quest"] = &ActionContext::hand_in_quest;


            // BG Tactics
            creators["bg tactics"] = &ActionContext::bg_tactics;
            creators["bg move to start"] = &ActionContext::bg_move_to_start;
            creators["bg move to objective"] = &ActionContext::bg_move_to_objective;
            creators["bg select objective"] = &ActionContext::bg_select_objective;
            creators["bg check objective"] = &ActionContext::bg_check_objective;
            creators["bg attack fc"] = &ActionContext::bg_attack_fc;
            creators["bg protect fc"] = &ActionContext::bg_protect_fc;
            creators["bg use buff"] = &ActionContext::bg_use_buff;
            creators["attack enemy flag carrier"] = &ActionContext::attack_enemy_fc;
            creators["bg check flag"] = &ActionContext::bg_check_flag;
        }

    private:
        static Action* give_water(PlayerbotAI* ai) { return new GiveWaterAction(ai); }
        static Action* give_food(PlayerbotAI* ai) { return new GiveFoodAction(ai); }
        static Action* ra(PlayerbotAI* ai) { return new RemoveAuraAction(ai); }
        static Action* mark_rti(PlayerbotAI* ai) { return new MarkRtiAction(ai); }
        static Action* set_return_position(PlayerbotAI* ai) { return new SetReturnPositionAction(ai); }
        static Action* rpg(PlayerbotAI* ai) { return new RpgAction(ai); }
        static Action* choose_rpg_target(PlayerbotAI* ai) { return new ChooseRpgTargetAction(ai); }
        static Action* move_to_rpg_target(PlayerbotAI* ai) { return new MoveToRpgTargetAction(ai); }
        static Action* travel(PlayerbotAI* ai) { return new TravelAction(ai); }
        static Action* choose_travel_target(PlayerbotAI* ai) { return new ChooseTravelTargetAction(ai); }
        static Action* move_to_travel_target(PlayerbotAI* ai) { return new MoveToTravelTargetAction(ai); }
        static Action* move_out_of_collision(PlayerbotAI* ai) { return new MoveOutOfCollisionAction(ai); }
        static Action* move_random(PlayerbotAI* ai) { return new MoveRandomAction(ai); }
        static Action* check_values(PlayerbotAI* ai) { return new CheckValuesAction(ai); }
        static Action* greet(PlayerbotAI* ai) { return new GreetAction(ai); }
        static Action* apply_poison(PlayerbotAI* ai) { return new ImbueWithPoisonAction(ai); }
        static Action* apply_oil(PlayerbotAI* ai) { return new ImbueWithOilAction(ai); }
        static Action* try_emergency(PlayerbotAI* ai) { return new TryEmergencyAction(ai); }
        static Action* apply_stone(PlayerbotAI* ai) { return new ImbueWithStoneAction(ai); }
        static Action* check_mail(PlayerbotAI* ai) { return new CheckMailAction(ai); }
        static Action* drop_target(PlayerbotAI* ai) { return new DropTargetAction(ai); }
        static Action* attack_duel_opponent(PlayerbotAI* ai) { return new AttackDuelOpponentAction(ai); }
        static Action* guard(PlayerbotAI* ai) { return new GuardAction(ai); }
        static Action* open_loot(PlayerbotAI* ai) { return new OpenLootAction(ai); }
        static Action* move_to_loot(PlayerbotAI* ai) { return new MoveToLootAction(ai); }
        static Action* _return(PlayerbotAI* ai) { return new ReturnAction(ai); }
        static Action* shoot(PlayerbotAI* ai) { return new CastShootAction(ai); }
        static Action* melee(PlayerbotAI* ai) { return new MeleeAction(ai); }
        static Action* switch_to_melee(PlayerbotAI* ai) { return new SwitchToMeleeAction(ai); }
        static Action* switch_to_ranged(PlayerbotAI* ai) { return new SwitchToRangedAction(ai); }
        static Action* ReachSpell(PlayerbotAI* ai) { return new ReachSpellAction(ai); }
        static Action* ReachMelee(PlayerbotAI* ai) { return new ReachMeleeAction(ai); }
        static Action* reach_party_member_to_heal(PlayerbotAI* ai) { return new ReachPartyMemberToHealAction(ai); }
        static Action* flee_with_pet(PlayerbotAI* ai) { return new FleeWithPetAction(ai); }
        static Action* flee(PlayerbotAI* ai) { return new FleeAction(ai); }
        static Action* gift_of_the_naaru(PlayerbotAI* ai) { return new CastGiftOfTheNaaruAction(ai); }
        static Action* lifeblood(PlayerbotAI* ai) { return new CastLifeBloodAction(ai); }
        static Action* arcane_torrent(PlayerbotAI* ai) { return new CastArcaneTorrentAction(ai); }
        static Action* mana_tap(PlayerbotAI* ai) { return new CastManaTapAction(ai); }
        static Action* war_stomp(PlayerbotAI* ai) { return new CastWarStompAction(ai); }
        static Action* end_pull(PlayerbotAI* ai) { return new ChangeCombatStrategyAction(ai, "-pull"); }

        static Action* emote(PlayerbotAI* ai) { return new EmoteAction(ai); }
        static Action* talk(PlayerbotAI* ai) { return new TalkAction(ai); }
        static Action* suggest_what_to_do(PlayerbotAI* ai) { return new SuggestWhatToDoAction(ai); }
        static Action* suggest_trade(PlayerbotAI* ai) { return new SuggestTradeAction(ai); }
        static Action* attack_anything(PlayerbotAI* ai) { return new AttackAnythingAction(ai); }
        static Action* attack_least_hp_target(PlayerbotAI* ai) { return new AttackLeastHpTargetAction(ai); }
        static Action* attack_enemy_player(PlayerbotAI* ai) { return new AttackEnemyPlayerAction(ai); }
        static Action* stay(PlayerbotAI* ai) { return new StayAction(ai); }
        static Action* sit(PlayerbotAI* ai) { return new SitAction(ai); }
        static Action* runaway(PlayerbotAI* ai) { return new RunAwayAction(ai); }
        static Action* follow(PlayerbotAI* ai) { return new FollowAction(ai); }
        static Action* flee_to_master(PlayerbotAI* ai) { return new FleeToMasterAction(ai); }
        static Action* add_gathering_loot(PlayerbotAI* ai) { return new AddGatheringLootAction(ai); }
        static Action* add_loot(PlayerbotAI* ai) { return new AddLootAction(ai); }
        static Action* add_all_loot(PlayerbotAI* ai) { return new AddAllLootAction(ai); }
        static Action* loot(PlayerbotAI* ai) { return new LootAction(ai); }
        static Action* release_loot(PlayerbotAI* ai) { return new ReleaseLootAction(ai); }
        static Action* dps_assist(PlayerbotAI* ai) { return new DpsAssistAction(ai); }
        static Action* dps_aoe(PlayerbotAI* ai) { return new DpsAoeAction(ai); }
        static Action* attack_rti_target(PlayerbotAI* ai) { return new AttackRtiTargetAction(ai); }
        static Action* tank_assist(PlayerbotAI* ai) { return new TankAssistAction(ai); }
        static Action* drink(PlayerbotAI* ai) { return new DrinkAction(ai); }
        static Action* food(PlayerbotAI* ai) { return new EatAction(ai); }
        static Action* mana_potion(PlayerbotAI* ai) { return new UseManaPotion(ai); }
        static Action* healing_potion(PlayerbotAI* ai) { return new UseHealingPotion(ai); }
        static Action* healthstone(PlayerbotAI* ai) { return new UseItemAction(ai, "healthstone"); }
        static Action* move_out_of_enemy_contact(PlayerbotAI* ai) { return new MoveOutOfEnemyContactAction(ai); }
        static Action* set_facing(PlayerbotAI* ai) { return new SetFacingTargetAction(ai); }
        static Action* set_behind(PlayerbotAI* ai) { return new SetBehindTargetAction(ai); }
        static Action* say(PlayerbotAI* ai) { return new SayAction(ai); }
        static Action* reveal_gathering_item(PlayerbotAI* ai) { return new RevealGatheringItemAction(ai); }
        static Action* outfit(PlayerbotAI* ai) { return new OutfitAction(ai); }
        static Action* random_bot_update(PlayerbotAI* ai) { return new RandomBotUpdateAction(ai); }
        static Action* delay(PlayerbotAI* ai) { return new DelayAction(ai); }
        static Action* mount(PlayerbotAI *ai) { return new CastSpellAction(ai, "mount"); }
        static Action* auto_talents(PlayerbotAI* ai) { return new AutoSetTalentsAction(ai); }
		static Action* auto_learn_spell(PlayerbotAI* ai) { return new AutoLearnSpellAction(ai); }
        static Action* xp_gain(PlayerbotAI* ai) { return new XpGainAction(ai); }
        static Action* invite_nearby(PlayerbotAI* ai) { return new InviteNearbyToGroupAction(ai); }
        static Action* leave_far_away(PlayerbotAI* ai) { return new LeaveFarAwayAction(ai); }
        static Action* move_to_dark_portal(PlayerbotAI* ai) { return new MoveToDarkPortalAction(ai); }
        static Action* use_dark_portal_azeroth(PlayerbotAI* ai) { return new DarkPortalAzerothAction(ai); }
        static Action* move_from_dark_portal(PlayerbotAI* ai) { return new MoveFromDarkPortalAction(ai); }
        static Action* world_buff(PlayerbotAI* ai) { return new WorldBuffAction(ai); }
        static Action* cast_random_spell(PlayerbotAI* ai) { return new CastRandomSpellAction(ai); }
        static Action* continue_action(PlayerbotAI* ai) { return new ContinueMoveDoAction(ai); }
        static Action* queue_at_bm(PlayerbotAI* ai) { return new QueueAtBmAction(ai); }

        // BG Tactics
        static Action* bg_tactics(PlayerbotAI* ai) { return new BGTactics(ai); }
        static Action* bg_move_to_start(PlayerbotAI* ai) { return new BGTactics(ai, "move to start"); }
        static Action* bg_move_to_objective(PlayerbotAI* ai) { return new BGTactics(ai, "move to objective"); }
        static Action* bg_select_objective(PlayerbotAI* ai) { return new BGTactics(ai, "select objective"); }
        static Action* bg_check_objective(PlayerbotAI* ai) { return new BGTactics(ai, "check objective"); }
        static Action* bg_attack_fc(PlayerbotAI* ai) { return new BGTactics(ai, "attack fc"); }
        static Action* bg_protect_fc(PlayerbotAI* ai) { return new BGTactics(ai, "protect fc"); }
        static Action* attack_enemy_fc(PlayerbotAI* ai) { return new AttackEnemyFlagCarrierAction(ai); }
        static Action* bg_use_buff(PlayerbotAI* ai) { return new BGTactics(ai, "use buff"); }
        static Action* bg_check_flag(PlayerbotAI* ai) { return new BGTactics(ai, "check flag"); }       
        static Action* pick_up_quest(PlayerbotAI* ai) { return new PickUpQuestAction(ai); }
        static Action* do_quest_objective(PlayerbotAI* ai) { return new DoQuestObjectiveAction(ai); }
        static Action* hand_in_quest(PlayerbotAI* ai) { return new HandInQuestAction(ai); }

    };
};
