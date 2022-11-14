#include "botpch.h"
#include "../../playerbot.h"
#include "PossibleAttackTargetsValue.h"
#include "PossibleTargetsValue.h"

#include "../../ServerFacade.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"

using namespace ai;
using namespace MaNGOS;

list<ObjectGuid> PossibleAttackTargetsValue::Calculate()
{
    list<ObjectGuid> result;
    if (ai->AllowActivity(ALL_ACTIVITY))
    {
        result = AI_VALUE(list<ObjectGuid>, "attackers");
        RemoveNonThreating(result);

        // Add the duel opponent
        if (bot->duel && bot->duel->opponent)
        {
            result.push_back(bot->duel->opponent->GetObjectGuid());
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
        if (!IsValidTarget(target, bot, true))
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

bool PossibleAttackTargetsValue::HasIgnoreCCRti(Unit* attacker, Player* player)
{
    Group* group = player->GetGroup();

    if (group && (group->GetTargetIcon(7) == attacker->GetObjectGuid()))
        return true;

    return false;
}

bool PossibleAttackTargetsValue::HasBreakableCC(Unit* attacker, Player* player)
{
    if (attacker->IsPolymorphed())
        return true;

    if (sServerFacade.IsFrozen(attacker))
        return true;

    PlayerbotAI* ai = player->GetPlayerbotAI();
    if (ai)
    {
        if (ai->HasAura("sap", attacker))
            return true;

        if (ai->HasAura("gouge", attacker))
            return true;

        if (ai->HasAura("shackle undead", attacker))
            return true;
    }

    return false;
}

bool PossibleAttackTargetsValue::HasUnBreakableCC(Unit* attacker, Player* player)
{
    if (attacker->IsStunned())
        return true;

#ifdef MANGOS
    if (attacker->hasUnitState(UNIT_STAT_STUNNED))
        return true;
#endif

    if (sServerFacade.IsFeared(attacker))
        return true;

    if (sServerFacade.IsInRoots(attacker))
        return true;

    if (sServerFacade.IsCharmed(attacker) && attacker->IsInTeam(player, true))
        return true;

    PlayerbotAI* ai = player->GetPlayerbotAI();
    if (ai)
    {
        if (ai->HasAura("banish", attacker))
            return true;
    }

    return false;
}

bool PossibleAttackTargetsValue::IsPossibleTarget(Unit* attacker, Player* player, float range, bool ignoreCC)
{
    if (!attacker)
        return false;

    Creature* c = dynamic_cast<Creature*>(attacker);
    Group* group = player->GetGroup();
    Player* master = nullptr;

    bool hasCC = false;
    bool leaderHasThreat = false;
    bool isMemberBotGroup = false;
    bool inVehicle = false;
    bool hasEnemyPlayerTarget = false;
    bool inDuel = false;
    bool canSeeAttacker = false;
    bool hasAttackTaggedStrategy = false;
    bool isFriendly = false;
    bool isDead = false;

    // If the player is a bot
    PlayerbotAI* bot = player->GetPlayerbotAI();
    if (bot)
    {
        master = bot->GetMaster();
        if (master)
        {
            if (group)
            {
                leaderHasThreat = attacker->getThreatManager().getThreat(master);

                // If the master is a bot
                if (master->GetPlayerbotAI() && !master->GetPlayerbotAI()->IsRealPlayer())
                    isMemberBotGroup = true;
            }
        }

        inVehicle = bot->IsInVehicle(false, true);
        hasEnemyPlayerTarget = bot->GetAiObjectContext()->GetValue<Unit*>("enemy player target")->Get();
        hasAttackTaggedStrategy = bot->HasStrategy("attack tagged", BotState::BOT_STATE_NON_COMBAT);
    }


    isDead = sServerFacade.UnitIsDead(attacker);
    isFriendly = sServerFacade.IsFriendlyTo(attacker, player);
    inDuel = player->duel && player->duel->opponent && (attacker->GetObjectGuid() == player->duel->opponent->GetObjectGuid());
    canSeeAttacker = attacker->IsVisibleForOrDetect(player, player->GetCamera().GetBody(), true);
    hasCC = !ignoreCC && !HasIgnoreCCRti(attacker, player) && (HasBreakableCC(attacker, player) || HasUnBreakableCC(attacker, player));

#ifndef MANGOSBOT_ZERO
    Player* arenaEnemy = dynamic_cast<Player*>(attacker);
    if (arenaEnemy)
    {
        if (arenaEnemy->InArena() && player->InArena() && (arenaEnemy->GetBGTeam() != player->GetBGTeam()))
            inDuel = true;
    }
#endif

    return attacker->IsInWorld() &&
        (attacker->GetMapId() == player->GetMapId()) &&
        !isDead &&
        !attacker->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1) &&
        !attacker->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNTARGETABLE) &&
        (inVehicle || !attacker->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE)) &&
        canSeeAttacker && !hasCC &&
        (!isFriendly || inDuel) &&
        player->IsWithinDistInMap(attacker, range) &&
        !attacker->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION) &&
        !(attacker->GetObjectGuid().IsPet() && hasEnemyPlayerTarget) &&
        !(attacker->GetCreatureType() == CREATURE_TYPE_CRITTER && !attacker->IsInCombat()) &&
        !(sPlayerbotAIConfig.IsInPvpProhibitedZone(sServerFacade.GetAreaId(attacker)) && (attacker->GetObjectGuid().IsPlayer() || attacker->GetObjectGuid().IsPet())) &&
        (!c || (
#ifdef MANGOS
            !c->IsInEvadeMode() &&
#endif
#ifdef CMANGOS
            !c->GetCombatManager().IsInEvadeMode() &&
#endif
            (
#ifdef CMANGOS
            (!isMemberBotGroup && hasAttackTaggedStrategy) || leaderHasThreat ||
                (!c->HasLootRecipient() &&
                    (!c->GetVictim() ||
                        c->GetVictim() &&
                        ((player->IsInGroup(c->GetVictim())) ||
                            (master && c->GetVictim() == master)))) ||
                c->IsTappedBy(player)
#endif
#ifndef MANGOSBOT_TWO
#ifdef MANGOS
                !attacker->HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_TAPPED) || player->IsTappedByMeOrMyGroup(c)
#endif
#endif
                )
            )
        );
}

bool PossibleAttackTargetsValue::IsValidTarget(Unit* attacker, Player* bot, bool ignoreCC)
{
    return  IsPossibleTarget(attacker, bot, sPlayerbotAIConfig.sightDistance, ignoreCC) &&
                (sServerFacade.GetThreatManager(attacker).getCurrentVictim() ||
                attacker->GetGuidValue(UNIT_FIELD_TARGET) ||
                attacker->GetObjectGuid().IsPlayer() ||
                (attacker->GetObjectGuid() == bot->GetPlayerbotAI()->GetAiObjectContext()->GetValue<ObjectGuid>("attack target")->Get()) ||
                (!HasIgnoreCCRti(attacker, bot) && (HasBreakableCC(attacker, bot) || HasUnBreakableCC(attacker, bot))));
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