#include "botpch.h"
#include "../../playerbot.h"
#include "PossibleAttackTargetsValue.h"
#include "PossibleTargetsValue.h"

#include "../../ServerFacade.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "AttackersValue.h"

using namespace ai;
using namespace MaNGOS;

list<ObjectGuid> PossibleAttackTargetsValue::Calculate()
{
    list<ObjectGuid> result;
    if (ai->AllowActivity(ALL_ACTIVITY))
    {
        if (bot->IsInWorld() && !bot->IsBeingTeleported())
        {
            // Check if we only need one possible attack target
            bool getOne = false;
            if (!qualifier.empty())
            {
                getOne = stoi(qualifier);
            }

            if (getOne)
            {
                // Try to get one possible attack target
                result = AI_VALUE2(list<ObjectGuid>, "attackers", 1);
                RemoveNonThreating(result);
            }

            // If the one possible attack target failed, retry
            if (result.empty())
            {
                result = AI_VALUE(list<ObjectGuid>, "attackers");
                RemoveNonThreating(result);
            }
        }
    }

	return result;
}

void PossibleAttackTargetsValue::RemoveNonThreating(list<ObjectGuid>& targets)
{
    list<ObjectGuid> breakableCC;
    list<ObjectGuid> unBreakableCC;

    for(list<ObjectGuid>::iterator tIter = targets.begin(); tIter != targets.end();)
    {
        Unit* target = ai->GetUnit(*tIter);
        if (!IsValid(target, bot, sPlayerbotAIConfig.sightDistance, true, false))
        {
            list<ObjectGuid>::iterator tIter2 = tIter;
            ++tIter;
            targets.erase(tIter2);
        }
        else if (!HasIgnoreCCRti(target, bot) && HasBreakableCC(target, bot))
        {
            breakableCC.push_back(*tIter);
            list<ObjectGuid>::iterator tIter2 = tIter;
            ++tIter;
            targets.erase(tIter2);
        }
        else if (!HasIgnoreCCRti(target, bot) && HasUnBreakableCC(target, bot))
        {
            unBreakableCC.push_back(*tIter);
            list<ObjectGuid>::iterator tIter2 = tIter;
            ++tIter;
            targets.erase(tIter2);
        }
        else
        {
            ++tIter;
        }
    }

    if (targets.empty())
    {
        if (!unBreakableCC.empty())
        {
            targets = unBreakableCC;
        }
        else if(!breakableCC.empty())
        {
            targets = breakableCC;
        }
    }
}

bool PossibleAttackTargetsValue::HasIgnoreCCRti(Unit* target, Player* player)
{
    Group* group = player->GetGroup();
    return group && (group->GetTargetIcon(7) == target->GetObjectGuid());
}

bool PossibleAttackTargetsValue::HasBreakableCC(Unit* target, Player* player)
{
    if (target->IsPolymorphed())
    {
        return true;
    }

    if (sServerFacade.IsFrozen(target))
    {
        return true;
    }

    PlayerbotAI* ai = player->GetPlayerbotAI();
    if (ai)
    {
        if (ai->HasAura("sap", target))
        {
            return true;
        }

        if (ai->HasAura("gouge", target))
        {
            return true;
        }

        if (ai->HasAura("shackle undead", target))
        {
            return true;
        }
    }

    return false;
}

bool PossibleAttackTargetsValue::HasUnBreakableCC(Unit* target, Player* player)
{
    if (target->IsStunned())
    {
        return true;
    }

    if (sServerFacade.IsFeared(target))
    {
        return true;
    }

    if (sServerFacade.IsInRoots(target))
    {
        return true;
    }

    if (sServerFacade.IsCharmed(target) && target->IsInTeam(player, true))
    {
        return true;
    }

    PlayerbotAI* ai = player->GetPlayerbotAI();
    if (ai)
    {
        if (ai->HasAura("banish", target))
        {
            return true;
        }
    }

    return false;
}

bool PossibleAttackTargetsValue::IsTapped(Unit* target, Player* player)
{
    if (player)
    {
        PlayerbotAI* playerBot = player->GetPlayerbotAI();
        const Creature* creature = dynamic_cast<Creature*>(target);
        if (creature)
        {
            Unit* victim = creature->GetVictim();
            Player* master = playerBot ? playerBot->GetMaster() : nullptr;
            const bool leaderHasThreat = master && target->getThreatManager().getThreat(master);
            const bool inBotGroup = player->GetGroup() && (master && master->GetPlayerbotAI() && !master->GetPlayerbotAI()->IsRealPlayer());
            const bool hasAttackTaggedStrategy = playerBot && playerBot->HasStrategy("attack tagged", BotState::BOT_STATE_NON_COMBAT);
            const bool isAttackingGroupMember = victim && (player->IsInGroup(victim) || (master && victim == master));
            const bool hasLootRecipient = creature->HasLootRecipient();
            
            if (creature->IsTappedBy(player) || 
                leaderHasThreat || 
                ((!victim || isAttackingGroupMember) && !hasLootRecipient) ||
                (!inBotGroup && hasAttackTaggedStrategy))
            {
                return true;
            }
        }
    }

    return false;
}

bool PossibleAttackTargetsValue::IsValid(Unit* target, Player* player, float range, bool ignoreCC, bool checkAttackerValid)
{
    // Check for the valid attackers value
    if (checkAttackerValid && !AttackersValue::IsValid(target, player))
    {
        return false;
    }

    return IsPossibleTarget(target, player, range, ignoreCC) &&
                (sServerFacade.GetThreatManager(target).getCurrentVictim() ||
                target->GetGuidValue(UNIT_FIELD_TARGET) ||
                target->GetObjectGuid().IsPlayer() ||
                (player->GetPlayerbotAI() && (target->GetObjectGuid() == PAI_VALUE(ObjectGuid, "attack target"))) ||
                (!HasIgnoreCCRti(target, player) && (HasBreakableCC(target, player) || HasUnBreakableCC(target, player))));
}

bool PossibleAttackTargetsValue::IsPossibleTarget(Unit* target, Player* player, float range, bool ignoreCC)
{
    // If the target is in an attackable distance
    if(!player->IsWithinDistInMap(target, range))
    {
        return false;
    }

    // If the target is CC'ed
    if(!ignoreCC && !HasIgnoreCCRti(target, player) && (HasBreakableCC(target, player) || HasUnBreakableCC(target, player)))
    {
        return false;
    }

    // If the player is a bot
    PlayerbotAI* playerBot = player->GetPlayerbotAI();
    if (playerBot)
    {
        // If the target is a pet and we have an enemy player nearby
        const bool hasEnemyPlayerTarget = PAI_VALUE(Unit*, "enemy player target");
        if(target->GetObjectGuid().IsPet() && hasEnemyPlayerTarget)
        {
            return false;
        }
    }

    // If the target is a NPC
    Player* enemyPlayer = dynamic_cast<Player*>(target);
    if (!enemyPlayer)
    {
        // If the target is a critter (and is not in combat)
        if ((target->GetCreatureType() == CREATURE_TYPE_CRITTER) && !target->IsInCombat())
        {
            return false;
        }

        // If the target is not tapped (the player doesn't have the loot rights (gray name))
        if (!IsTapped(target, player))
        {
            return false;
        }
    }

    return true;
}

bool PossibleAddsValue::Calculate()
{
    PlayerbotAI *ai = bot->GetPlayerbotAI();
    list<ObjectGuid> possible = ai->GetAiObjectContext()->GetValue<list<ObjectGuid>>("possible targets no los")->Get();
    list<ObjectGuid> attackers = ai->GetAiObjectContext()->GetValue<list<ObjectGuid>>("possible attack targets")->Get();

    for (list<ObjectGuid>::iterator i = possible.begin(); i != possible.end(); ++i)
    {
        ObjectGuid guid = *i;
        if (find(attackers.begin(), attackers.end(), guid) != attackers.end()) continue;

        Unit* add = ai->GetUnit(guid);
        if (add && !add->GetGuidValue(UNIT_FIELD_TARGET) && !sServerFacade.GetThreatManager(add).getCurrentVictim() && sServerFacade.IsHostileTo(add, bot))
        {
            for (list<ObjectGuid>::iterator j = attackers.begin(); j != attackers.end(); ++j)
            {
                Unit* attacker = ai->GetUnit(*j);
                if (!attacker) continue;

                float dist = sServerFacade.GetDistance2d(attacker, add);
                if (sServerFacade.IsDistanceLessOrEqualThan(dist, sPlayerbotAIConfig.aoeRadius * 1.5f)) continue;
                if (sServerFacade.IsDistanceLessOrEqualThan(dist, sPlayerbotAIConfig.aggroDistance)) return true;
            }
        }
    }
    return false;
}