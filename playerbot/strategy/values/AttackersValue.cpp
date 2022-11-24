#include "botpch.h"
#include "../../playerbot.h"
#include "AttackersValue.h"
#include "PossibleTargetsValue.h"
#include "EnemyPlayerValue.h"

using namespace ai;
using namespace MaNGOS;

list<ObjectGuid> AttackersValue::Calculate()
{
    list<ObjectGuid> result;
    if (ai->AllowActivity(ALL_ACTIVITY))
    {
        if(bot->IsInWorld() && !bot->IsBeingTeleported())
        {
            set<Unit*> targets;

            // Check if we only need one attacker
            bool getOne = false;
            if(!qualifier.empty())
            {
                getOne = stoi(qualifier);
            }

            set<ObjectGuid> invalidTargets;

            // Add the targets of the bot
            AddTargetsOf(bot, targets, invalidTargets, getOne);

            // Don't check for group member targets if we only need one
            if (targets.empty() || !getOne)
            {
                // Add the targets of the members of the group
                Group* group = bot->GetGroup();
                if (group)
                {
                    AddTargetsOf(group, targets, invalidTargets, getOne);
                }
            }

            // Convert the targets to guids
            for (Unit* target : targets)
            {
                result.push_back(target->GetObjectGuid());
            }
        }
    }

	return result;
}

void AttackersValue::AddTargetsOf(Group* group, set<Unit*>& targets, set<ObjectGuid>& invalidTargets, bool getOne)
{
    Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
    for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
    {
        // Only add group member targets that are alive and near the player
        Player* member = sObjectMgr.GetPlayer(itr->guid);
        if (member && (member != bot) &&
           sServerFacade.IsAlive(member) &&
           member->IsInWorld() &&
           !member->IsBeingTeleported() &&
           (member->GetMapId() == bot->GetMapId()) && 
           (sServerFacade.GetDistance2d(bot, member) <= GetRange()))
        {
            AddTargetsOf(member, targets, invalidTargets, getOne);

            // Finish early if we only need one target
            if (getOne && !targets.empty())
            {
                break;
            }
        }
    }
}

void AttackersValue::AddTargetsOf(Player* player, set<Unit*>& targets, set<ObjectGuid>& invalidTargets, bool getOne)
{
    // If the player is available
    if (player)
    {
        list<Unit*> units;

        // Get all the units around the player
        PlayerbotAI* playerBot = player->GetPlayerbotAI();
        if (playerBot)
        {
            const string ignoreLos = std::to_string(true);
            const string range = std::to_string((int32)GetRange());
            const string ignoreValidate = std::to_string(true);
            const vector<string> qualifiers = { ignoreLos, range, ignoreValidate };
            for (auto guid : PAI_VALUE2(list<ObjectGuid>, "possible targets", Qualified::MultiQualify(qualifiers, ":")))
            {
                if (Unit* unit = ai->GetUnit(guid))
                {
                    units.push_back(unit);
                }
            }
        }
        else
        {
            PossibleTargetsValue::FindPossibleTargets(player, units, GetRange());
        }

        // Get the current attackers of the player
        for (Unit* attacker : player->getAttackers())
        {
            units.push_back(attacker);
        }

        // Add the duel opponent
        if (bot == player && bot->duel && bot->duel->opponent)
        {
            units.push_back(bot->duel->opponent);
        }

        // If the player is a bot try to retrieve the pull, current and previous targets
        if(playerBot)
        {
            Unit* currentTarget = PAI_VALUE(Unit*, "current target");
            if(currentTarget)
            {
                units.push_back(currentTarget);
            }

            Unit* oldTarget = PAI_VALUE(Unit*, "old target");
            if (oldTarget)
            {
                units.push_back(oldTarget);
            }

            // Only consider the owner bot pull target
            if(bot == player)
            {
                Unit* attackTarget = playerBot->GetUnit(PAI_VALUE(ObjectGuid, "attack target"));
                if (attackTarget)
                {
                    units.push_back(attackTarget);
                }

                Unit* pullTarget = PAI_VALUE(Unit*, "pull target");
                if (pullTarget)
                {
                    units.push_back(pullTarget);
                }
            }
        }

        // Filter the units that are valid
        for (Unit* unit : units)
        {
            // Prevent checking a target that has already been validated
            if((targets.find(unit) == targets.end()))
            {
                // Prevent checking a target that has already been invalidated
                if(InCombat(unit, player) || (invalidTargets.find(unit->GetObjectGuid()) == invalidTargets.end()))
                {
                    if (IsPossibleTarget(unit, player))
                    {
                        // Add the target to the list of combat targets
                        targets.insert(unit);

                        // Add the target's pet/guardian too
                        unit->CallForAllControlledUnits(AddGuardiansHelper(units), CONTROLLED_PET | CONTROLLED_GUARDIANS | CONTROLLED_CHARM | CONTROLLED_MINIPET | CONTROLLED_TOTEMS);

                        if (getOne)
                        {
                            break;
                        }
                    }
                    else
                    {
                        invalidTargets.insert(unit->GetObjectGuid());
                    }
                }
            }
        }
    }
}

bool AttackersValue::IsFriendly(Unit* target, Player* player)
{
    bool friendly = false;
    if (sServerFacade.IsFriendlyTo(target, player))
    {
        friendly = true;

#ifndef MANGOSBOT_ZERO
        // Check if the target is another player in a duel/arena
        Player* targetPlayer = dynamic_cast<Player*>(target);
        if (targetPlayer)
        {
            // If the target is in an arena with the player and is not on the same team
            if (targetPlayer->InArena() && player->InArena() && (targetPlayer->GetBGTeam() != player->GetBGTeam()))
            {
                friendly = false;
            }
        }
#endif
    }

    return friendly;
}

bool AttackersValue::IsAttackable(Unit* target, Player* player, bool inVehicle)
{
    return !target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1) &&
           !target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNTARGETABLE) &&
           (inVehicle || !target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE)) &&
           !target->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION);
}

bool AttackersValue::InCombat(Unit* target, Player* player, bool checkPullTargets)
{
    // Check if the the target is attacking the player
    bool inCombat = (target->getThreatManager().getThreat(player) > 0.0f) ||
                    (target->GetVictim() && (target->GetVictim() == player));

    // Check if the target is attacking the player's pet
    if(!inCombat)
    {
        Pet* pet = player->GetPet();
        if (pet)
        {
            inCombat = (target->getThreatManager().getThreat(pet) > 0.0f) ||
                       (target->GetVictim() && (target->GetVictim() == pet));
        }
    }

    if(!inCombat && checkPullTargets && player->GetPlayerbotAI())
    {
        inCombat = (PAI_VALUE(ObjectGuid, "attack target") == target->GetObjectGuid()) ||
                   (PAI_VALUE(Unit*, "pull target") == target);
    }

    return inCombat;
}

bool AttackersValue::IsPossibleTarget(Unit* target, Player* player) const
{
    // If the target is available
    if (target && target->IsInWorld() && (target->GetMapId() == player->GetMapId()))
    {
        // If the target is dead
        if(sServerFacade.UnitIsDead(target))
        {
            return false;
        }

        // If the target is friendly (to the owner bot)
        if(IsFriendly(target, bot))
        {
            return false;
        }

        // If the target can't be attacked (by the owner bot)
        const bool inVehicle = player->GetPlayerbotAI() && player->GetPlayerbotAI()->IsInVehicle();
        if(!IsAttackable(target, bot, inVehicle))
        {
            return false;
        }

        // If the target is not visible (to the owner bot)
        if (!target->IsVisibleForOrDetect(bot, bot->GetCamera().GetBody(), true))
        {
            return false;
        }

        // This will be used on both enemy player and npc checks
        const bool inPvPProhibitedZone = sPlayerbotAIConfig.IsInPvpProhibitedZone(sServerFacade.GetAreaId(target));

        // If the target is a player
        Player* enemyPlayer = dynamic_cast<Player*>(target);
        if (enemyPlayer)
        {
            // Don't consider enemy players if pvp strategy is not set
            if (!ai->HasStrategy("pvp", BotState::BOT_STATE_COMBAT))
            {
                return false;
            }

            // If the enemy player is in a PVP Prohibited zone
            if (inPvPProhibitedZone)
            {
                return false;
            }

            // Don't check distance on duel opponents
            if(player->duel && (player->duel->opponent != target))
            {
                // If the enemy player is not within sight distance (from the owner bot)
                if (!bot->IsWithinDist(enemyPlayer, GetRange(), false))
                {
                    return false;
                }
            }
        }
        // If the target is a NPC
        else
        {
            // If the target is not fighting the player (and if the owner bot is not pulling the target)
            if (!InCombat(target, player, (bot == player)))
            {
                return false;
            }

            // If the target is a player's pet and in a PvP prohibited zone
            if (target->GetObjectGuid().IsPet() && inPvPProhibitedZone)
            {
                return false;
            }

            // If the target is evading
            const Creature* creature = dynamic_cast<Creature*>(target);
            if(creature)
            {
                if(creature->GetCombatManager().IsInEvadeMode())
                {
                    return false;
                }
            }
        }

        return true;
    }

    return false;
}

bool AttackersValue::IsValid(Unit* target, Player* player, bool checkInCombat)
{
    // If the target is available
    if (target && target->IsInWorld() && (target->GetMapId() == player->GetMapId()))
    {
        // If the target is dead
        if (sServerFacade.UnitIsDead(target))
        {
            return false;
        }

        // If the target is friendly
        if (AttackersValue::IsFriendly(target, player))
        {
            return false;
        }

        // If the target can't be attacked
        const bool inVehicle = player->GetPlayerbotAI() && player->GetPlayerbotAI()->IsInVehicle();
        if (!AttackersValue::IsAttackable(target, player, inVehicle))
        {
            return false;
        }

        // If the target is not visible (to the owner bot)
        if (!target->IsVisibleForOrDetect(player, player->GetCamera().GetBody(), true))
        {
            return false;
        }

        // This will be used on both enemy player and npc checks
        const bool inPvPProhibitedZone = sPlayerbotAIConfig.IsInPvpProhibitedZone(sServerFacade.GetAreaId(target));

        // If the target is a player
        Player* enemyPlayer = dynamic_cast<Player*>(target);
        if (enemyPlayer)
        {
            // Don't consider enemy players if pvp strategy is not set
            if (player->GetPlayerbotAI() && !player->GetPlayerbotAI()->HasStrategy("pvp", BotState::BOT_STATE_COMBAT))
            {
                return false;
            }

            // If the enemy player is in a PVP Prohibited zone
            if (inPvPProhibitedZone)
            {
                return false;
            }

            // If the enemy player is not within sight distance
            if (!player->IsWithinDist(enemyPlayer, GetRange(), false))
            {
                return false;
            }
        }
        // If the target is a NPC
        else
        {
            // If the target is not fighting the player (and if the owner bot is not pulling the target)
            if (checkInCombat && !AttackersValue::InCombat(target, player))
            {
                return false;
            }

            // If the target is a player's pet and in a PvP prohibited zone
            if (target->GetObjectGuid().IsPet() && inPvPProhibitedZone)
            {
                return false;
            }

            // If the target is evading
            const Creature* creature = dynamic_cast<Creature*>(target);
            if (creature)
            {
                if (creature->GetCombatManager().IsInEvadeMode())
                {
                    return false;
                }
            }
        }

        return true;
    }

    return false;
}
