#include "botpch.h"
#include "../../playerbot.h"
#include "AttackersValue.h"
#include "PossibleTargetsValue.h"

#include "../../ServerFacade.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"

using namespace ai;
using namespace MaNGOS;

list<ObjectGuid> AttackersValue::Calculate()
{
    set<Unit*> targets;

    list<ObjectGuid> result;

    if (!ai->AllowActivity(ALL_ACTIVITY))
        return result;

    AddAttackersOf(bot, targets);

    Group* group = bot->GetGroup();
    if (group)
        AddAttackersOf(group, targets);

    RemoveNonThreating(targets);
    
	for (set<Unit*>::iterator i = targets.begin(); i != targets.end(); i++)
		result.push_back((*i)->GetObjectGuid());

    if (bot->duel && bot->duel->opponent)
        result.push_back(bot->duel->opponent->GetObjectGuid());

	return result;
}

void AttackersValue::AddAttackersOf(Group* group, set<Unit*>& targets)
{
    Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
    for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
    {
        Player *member = sObjectMgr.GetPlayer(itr->guid);
        if (!member || !sServerFacade.IsAlive(member) || member == bot || member->GetMapId() != bot->GetMapId() || sServerFacade.GetDistance2d(bot, member) > sPlayerbotAIConfig.sightDistance)
            continue;

        AddAttackersOf(member, targets);
    }
}

struct AddGuardiansHelper
{
    explicit AddGuardiansHelper(list<Unit*> &units) : units(units) {}
    void operator()(Unit* target) const
    {
        units.push_back(target);
    }

    list<Unit*> &units;
};

void AttackersValue::AddAttackersOf(Player* player, set<Unit*>& targets)
{
    if (!player || !player->IsInWorld() || player->IsBeingTeleported())
        return;

	list<Unit*> units;
    PossibleTargetsValue::FindPossibleTargets(player, units, sPlayerbotAIConfig.sightDistance);
	for (list<Unit*>::iterator i = units.begin(); i != units.end(); i++)
    {
        Unit* unit = *i;
        if (PossibleTargetsValue::IsValid(player, unit, sPlayerbotAIConfig.sightDistance))
        {
            if (!player->GetGroup())
            {
#ifdef CMANGOS
                if (!unit->getThreatManager().getThreat(player) && (!unit->getThreatManager().getCurrentVictim() || unit->getThreatManager().getCurrentVictim()->getTarget() != player))
#endif
#ifdef MANGOS
                if (!unit->GetThreatManager().getThreat(player))
#endif
                    continue;
            }

            targets.insert(unit);
            unit->CallForAllControlledUnits(AddGuardiansHelper(units), CONTROLLED_PET | CONTROLLED_GUARDIANS | CONTROLLED_CHARM | CONTROLLED_MINIPET | CONTROLLED_TOTEMS);
        }
    }
}

void AttackersValue::RemoveNonThreating(set<Unit*>& targets)
{
    for(set<Unit *>::iterator tIter = targets.begin(); tIter != targets.end();)
    {
        Unit* unit = *tIter;
        if (!IsValidTarget(unit, bot))
        {
            set<Unit *>::iterator tIter2 = tIter;
            ++tIter;
            targets.erase(tIter2);
        }
        else
            ++tIter;
    }
}

bool AttackersValue::IsPossibleTarget(Unit *attacker, Player *player, float range)
{
    if(!attacker)
        return false;

    Creature *c = dynamic_cast<Creature*>(attacker);
    Group* group = player->GetGroup();
    Player* master = nullptr;

    bool rti = false;
    bool leaderHasThreat = false;
    bool isMemberBotGroup = false;
    bool inVehicle = false;
    bool hasEnemyPlayerTarget = false;
    bool inDuel = false;
    bool canSeeAttacker = false;
    bool hasShackleUndeadAura = false;
    bool hasAttackTaggedStrategy = false;
    bool isSapped = false;
    bool isGouged = false;
    bool isStunned = false;
    bool isPolymorphed = false;
    bool isFeared = false;
    bool isFriendly = false;
    bool isDead = false;

    // If the player is a bot
    PlayerbotAI* bot = player->GetPlayerbotAI();
    if(bot)
    {
        master = bot->GetMaster();
        if (master)
        {
            if(group)
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
        isSapped = bot->HasAura("sap", attacker);
        isGouged = bot->HasAura("gouge", attacker);
        hasShackleUndeadAura = bot->HasAura("shackle undead", attacker);
    }

    isStunned = attacker->IsStunned();
    isPolymorphed = attacker->IsPolymorphed();
    isFeared = sServerFacade.IsFeared(attacker);
    isDead = sServerFacade.UnitIsDead(attacker);
    isFriendly = sServerFacade.IsFriendlyTo(attacker, player);
    inDuel = player->duel && player->duel->opponent && (attacker->GetObjectGuid() == player->duel->opponent->GetObjectGuid());
    canSeeAttacker = attacker->IsVisibleForOrDetect(player, player->GetCamera().GetBody(), true);
    rti = group && (group->GetTargetIcon(7) == attacker->GetObjectGuid());

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
        canSeeAttacker &&
#ifdef CMANGOS
        !(isStunned && hasShackleUndeadAura) &&
        !isGouged &&
#endif
#ifdef MANGOS
        //!attacker->hasUnitState(UNIT_STAT_STUNNED) &&
#endif
        !((isPolymorphed ||
        isSapped ||
        //sServerFacade.IsCharmed(attacker) ||
        isFeared) && !rti) &&
        //!sServerFacade.IsInRoots(attacker) &&
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

bool AttackersValue::IsValidTarget(Unit *attacker, Player *bot)
{
    return  IsPossibleTarget(attacker, bot) &&
            (sServerFacade.GetThreatManager(attacker).getCurrentVictim() ||
            attacker->GetGuidValue(UNIT_FIELD_TARGET) || attacker->GetObjectGuid().IsPlayer() ||
            attacker->GetObjectGuid() == bot->GetPlayerbotAI()->GetAiObjectContext()->GetValue<ObjectGuid>("pull target")->Get());
}

bool PossibleAddsValue::Calculate()
{
    PlayerbotAI *ai = bot->GetPlayerbotAI();
    list<ObjectGuid> possible = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("possible targets no los")->Get();
    list<ObjectGuid> attackers = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("attackers")->Get();

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
