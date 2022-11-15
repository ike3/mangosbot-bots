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

	return result;
}

void AttackersValue::AddTargetsOf(Group* group, set<Unit*>& targets)
{
    Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
    for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
    {
        // Only add group member targets that are alive and near the player
        Player* member = sObjectMgr.GetPlayer(itr->guid);
        if (member && sServerFacade.IsAlive(member) && 
            (member != bot) && 
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
    if (player && player->IsInWorld() && !player->IsBeingTeleported())
    {
        list<Unit*> units;

        // Get all the units around the player
        PossibleTargetsValue::FindPossibleTargets(player, units, GetRange());

        // Get the current attackers of the player
        for (Unit* attacker : player->getAttackers())
        {
            units.push_back(attacker);
        }

        // If the player is a bot try to retrieve the pull, current and previous targets
        PlayerbotAI* bot = player->GetPlayerbotAI();
        if(bot)
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

            // Only consider the owner bot's pull target
            if(ai->GetBot() == player)
            {
                Unit* attackTarget = bot->GetUnit(PAI_VALUE(ObjectGuid, "attack target"));
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
            if (IsValid(unit, player))
            {
                // Add the target to the list of combat targets
                targets.insert(unit);

                // Add the target's pet/guardian too
                unit->CallForAllControlledUnits(AddGuardiansHelper(units), CONTROLLED_PET | CONTROLLED_GUARDIANS | CONTROLLED_CHARM | CONTROLLED_MINIPET | CONTROLLED_TOTEMS);
            }
        }
    }
}

bool AttackersValue::IsValid(Unit* target, Player* player) const
{
    // If the target is available
    if (target && target->IsInWorld() && (target->GetMapId() == player->GetMapId()))
    {
        const bool isFriendly = sServerFacade.IsFriendlyTo(target, player);
        const bool isDead = sServerFacade.UnitIsDead(target);
        const bool inVehicle = player->GetPlayerbotAI() && player->GetPlayerbotAI()->IsInVehicle();
        const bool isAttackable = !target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1) &&
                                  !target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNTARGETABLE) &&
                                  (inVehicle || !target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE));

        const bool inPvPProhibitedZone = sPlayerbotAIConfig.IsInPvpProhibitedZone(sServerFacade.GetAreaId(target));

        // This checks things such as if it is too far, if it is invisible, if it is dead, in Los, etc...
        const bool isVisible = target->IsVisibleForOrDetect(player, player->GetCamera().GetBody(), true);

        // Check if the target is another player
        Player* playerEnemy = dynamic_cast<Player*>(target);
        if (playerEnemy)
        {
            // If the player is in a duel/arena
            bool inDuel = player->duel && player->duel->opponent && (playerEnemy->GetObjectGuid() == player->duel->opponent->GetObjectGuid());
#ifndef MANGOSBOT_ZERO
            if (playerEnemy->InArena() && player->InArena() && (playerEnemy->GetBGTeam() != player->GetBGTeam()))
            {
                inDuel = true;
            }
#endif

            const uint32 pvpDistance = (inVehicle || player->GetHealth() > playerEnemy->GetHealth()) ? EnemyPlayerValue::GetMaxAttackDistance(player) : 20.0f;
            const bool inPvpDistance = player->IsWithinDist(playerEnemy, pvpDistance, false);

            // Valid if the player target is:
            // - Not dead
            // - Not friendly or in duel with friendly
            // - Visible
            // - Is attackable
            // - Is not in PvP Prohibited zone
            // - Is in pvp distance (min distance for pvp interaction)
            return !isDead && 
                   (!isFriendly || inDuel) && 
                   isVisible && 
                   isAttackable && 
                   !inPvPProhibitedZone && 
                   inPvpDistance;
        }
        else
        {
            // Check if the npc target is in combat with the player
            const bool inCombatWithPlayer = (target->getThreatManager().getThreat(player) > 0.0f) ||
                                            (target->GetVictim() && (target->GetVictim() == player));

            const bool isPlayerPet = target->GetObjectGuid().IsPet();

            const Creature* creature = dynamic_cast<Creature*>(target);
#ifdef MANGOS
            const bool isInEvadeMode = creature && creature->IsInEvadeMode();
#endif
#ifdef CMANGOS
            const bool isInEvadeMode = creature && creature->GetCombatManager().IsInEvadeMode();
#endif

            // Check if the target has been requested to be attacked (only consider the owner bot's attack/pull targets)
            bool isPulling = false;
            if (ai->GetBot() == player)
            {
                isPulling = (PAI_VALUE(ObjectGuid, "attack target") == target->GetObjectGuid()) ||
                            (PAI_VALUE(Unit*, "pull target") == target);
            }

            // Valid if the npc target is:
            // - Not dead
            // - Not friendly
            // - In combat with the player or the player is pulling the target
            // - Visible
            // - Is attackable
            // - Is player pet not in PvP Prohibited zone
            // - Not in evade mode
            return !isDead && 
                   !isFriendly && 
                   (inCombatWithPlayer || isPulling) && 
                   isVisible && 
                   isAttackable && 
                   (!isPlayerPet || (isPlayerPet && !inPvPProhibitedZone)) &&
                   !isInEvadeMode;
        }
    }

    return false;
}
