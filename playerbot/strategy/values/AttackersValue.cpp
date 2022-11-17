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

            // Add the targets of the bot
            AddTargetsOf(bot, targets);

            // Add the targets of the members of the group
            Group* group = bot->GetGroup();
            if (group)
            {
                AddTargetsOf(group, targets);
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

void AttackersValue::AddTargetsOf(Group* group, set<Unit*>& targets)
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
            AddTargetsOf(member, targets);
        }
    }
}

void AttackersValue::AddTargetsOf(Player* player, set<Unit*>& targets)
{
    // If the player is available
    if (player)
    {
        list<Unit*> units;

        // Get all the units around the player
        PossibleTargetsValue::FindPossibleTargets(player, units, GetRange());

        // Get the current attackers of the player
        for (Unit* attacker : player->getAttackers())
        {
            units.push_back(attacker);
        }

        // Add the duel opponent
        if (bot->duel && bot->duel->opponent)
        {
            units.push_back(bot->duel->opponent);
        }

        // If the player is a bot try to retrieve the pull, current and previous targets
        PlayerbotAI* playerBot = player->GetPlayerbotAI();
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
            // Prevent checking a target that has already been checked
            if(targets.find(unit) == targets.end())
            {
                if (IsPossibleTarget(unit, player))
                {
                    // Add the target to the list of combat targets
                    targets.insert(unit);

                    // Add the target's pet/guardian too
                    unit->CallForAllControlledUnits(AddGuardiansHelper(units), CONTROLLED_PET | CONTROLLED_GUARDIANS | CONTROLLED_CHARM | CONTROLLED_MINIPET | CONTROLLED_TOTEMS);
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

        // Check if the target is another player in a duel/arena
        Player* targetPlayer = dynamic_cast<Player*>(target);
        if (targetPlayer)
        {
            // If the target is in a duel with the player
            if (player->duel && player->duel->opponent && (targetPlayer->GetObjectGuid() == player->duel->opponent->GetObjectGuid()))
            {
                friendly = false;
            }
#ifndef MANGOSBOT_ZERO
            // If the target is in an arena with the player and is not on the same team
            else if (targetPlayer->InArena() && player->InArena() && (targetPlayer->GetBGTeam() != player->GetBGTeam()))
            {
                friendly = false;
            }
#endif
        }
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
    bool inCombat = (target->getThreatManager().getThreat(player) > 0.0f) ||
                    (target->GetVictim() && (target->GetVictim() == player));

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
            // If the enemy player is in a PVP Prohibited zone
            if (inPvPProhibitedZone)
            {
                return false;
            }

            // If the enemy player is not within PvP distance (from the owner bot)
            const uint32 pvpDistance = (inVehicle || bot->GetHealth() > enemyPlayer->GetHealth()) ? EnemyPlayerValue::GetMaxAttackDistance(bot) : 20.0f;
            if (!bot->IsWithinDist(enemyPlayer, pvpDistance, false))
            {
                return false;
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

bool AttackersValue::IsValid(Unit* target, Player* player)
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
            // If the enemy player is in a PVP Prohibited zone
            if (inPvPProhibitedZone)
            {
                return false;
            }

            // If the enemy player is not within PvP distance (from the owner bot)
            const uint32 pvpDistance = (inVehicle || player->GetHealth() > enemyPlayer->GetHealth()) ? EnemyPlayerValue::GetMaxAttackDistance(player) : 20.0f;
            if (!player->IsWithinDist(enemyPlayer, pvpDistance, false))
            {
                return false;
            }
        }
        // If the target is a NPC
        else
        {
            // If the target is not fighting the player (and if the owner bot is not pulling the target)
            if (!AttackersValue::InCombat(target, player))
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
