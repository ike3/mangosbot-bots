#include "botpch.h"
#include "../../playerbot.h"
#include "EnemyPlayerValue.h"
#include "TargetValue.h"

using namespace ai;
using namespace std;

bool NearestEnemyPlayersValue::AcceptUnit(Unit* unit)
{
    Player* enemy = dynamic_cast<Player*>(unit);

    bool inCannon = ai->IsInVehicle(false, true);

    return (enemy &&
        enemy->IsWithinDist(bot, EnemyPlayerValue::GetMaxAttackDistance(bot), false) &&
        enemy->GetMapId() == bot->GetMapId() &&
#ifdef MANGOSBOT_ZERO
        ai->IsOpposing(enemy) &&
#else
        (ai->IsOpposing(enemy) || (bot->InArena() && enemy->InArena() && bot->GetBGTeam() != enemy->GetBGTeam())) &&
#endif
        enemy->IsPvP() &&
        !enemy->IsPolymorphed() &&
        !ai->HasAura("sap", enemy) &&
        !ai->HasAura("gouge", enemy) &&
        !sServerFacade.IsFeared(enemy) &&
        !sPlayerbotAIConfig.IsInPvpProhibitedZone(sServerFacade.GetAreaId(enemy)) &&
        !enemy->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1) &&
        !enemy->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNTARGETABLE) &&
        ((inCannon || !enemy->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNINTERACTIBLE))) &&
        //!enemy->HasStealthAura() &&
        //!enemy->HasInvisibilityAura() &&
        enemy->IsVisibleForOrDetect(bot, bot->GetCamera().GetBody(), false) &&
        !enemy->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION)
        );
}

Unit* EnemyPlayerValue::Calculate()
{
    // from VMaNGOS

    Unit* duel = AI_VALUE(Unit*, "duel target");
    if (duel)
        return duel;

    bool inCannon = ai->IsInVehicle(false, true);
    float const maxAggroDistance = GetMaxAttackDistance(bot);
    Unit* pVictim = bot->GetVictim();
    bool isMelee = !ai->IsRanged(bot);

    // chance to not change target
    /*if (pVictim && pVictim->IsPlayer() && bot->IsWithinDist(pVictim, maxAggroDistance)&&)*/

    // 1. Check units we are currently in combat with.
    std::list<Unit*> targets;
    HostileReference* pReference = bot->getHostileRefManager().getFirst();
    while (pReference)
    {
        ThreatManager* threatManager = pReference->getSource();
        if (Unit* pTarget = threatManager->getOwner())
        {
            if (/*pTarget != pVictim &&*/
                pTarget->IsPlayer() &&
                pTarget->IsVisibleForOrDetect(bot, bot->GetCamera().GetBody(), false) &&
                /*bot->IsWithinLOSInMap(pTarget) &&*/
                bot->IsWithinDist(pTarget, maxAggroDistance))
            {
                if (bot->GetTeam() == HORDE)
                {
                    if (pTarget->HasAura(23333))
                        return pTarget;
                }
                else
                {
                    if (pTarget->HasAura(23335))
                        return pTarget;
                }

                targets.push_back(pTarget);
            }
        }
        pReference = pReference->next();
    }

    if (!targets.empty())
    {
        if (isMelee)
        {
            targets.sort([this](Unit* pUnit1, const Unit* pUnit2)
                {
                    return bot->GetDistance(pUnit1, false) < bot->GetDistance(pUnit2, false);
                });
        }
        else
        {
            targets.sort([this](Unit* pUnit1, const Unit* pUnit2)
                {
                    return pUnit1->GetHealth() < pUnit2->GetHealth();
                });
        }

        for (auto enemy : targets)
        {
            // TODO some logic
            return enemy;
        }
    }

    // 2. Find enemy player in range.

    list<ObjectGuid> players = AI_VALUE(list<ObjectGuid>, "nearest enemy players");
    std::list<Player*> targetsList;

    for (const auto& gTarget : players)
    {
        Unit* pUnit = ai->GetUnit(gTarget);
        if (!pUnit)
            continue;

        Player* pTarget = dynamic_cast<Player*>(pUnit);
        if (!pTarget)
            continue;

        /*if (pTarget == pVictim)
            continue;*/

        uint32 const aggroDistance = (inCannon || bot->GetHealth() > pTarget->GetHealth()) ? maxAggroDistance : 20.0f;
        if (!bot->IsWithinDist(pTarget, aggroDistance, false))
            continue;

        if (bot->GetTeam() == HORDE)
        {
            if (pTarget->HasAura(23333))
                return pTarget;
        }
        else
        {
            if (pTarget->HasAura(23335))
                return pTarget;
        }

        // TODO choose proper targets
        if (/*bot->IsWithinLOSInMap(pTarget, true) && (inCannon || */fabs(bot->GetPositionZ() - pTarget->GetPositionZ()) < 30.0f)
            targetsList.push_back(pTarget);
    }

    if (!targetsList.empty())
    {
        if (isMelee)
        {
            targetsList.sort([this](Unit* pUnit1, const Unit* pUnit2)
                {
                    return bot->GetDistance(pUnit1, false) < bot->GetDistance(pUnit2, false);
                });
        }
        else
        {
            targetsList.sort([this](Unit* pUnit1, const Unit* pUnit2)
                {
                    return pUnit1->GetHealth() < pUnit2->GetHealth();
                });
        }

        return *targetsList.begin();
    }

    // 3. Check party attackers.

    if (Group* pGroup = bot->GetGroup())
    {
        for (GroupReference* itr = pGroup->GetFirstMember(); itr != nullptr; itr = itr->next())
        {
            if (Unit* pMember = itr->getSource())
            {
                if (pMember == bot || pMember->GetMapId() != bot->GetMapId())
                    continue;

                if (sServerFacade.GetDistance2d(bot, pMember) > 30.0f)
                    continue;

                if (Unit* pAttacker = pMember->getAttackerForHelper())
                    if (bot->IsWithinDist(pAttacker, maxAggroDistance, false) &&
                        /*bot->IsWithinLOSInMap(pAttacker, true) &&*/
                        /*pAttacker != pVictim &&*/
                        pAttacker->IsVisibleForOrDetect(bot, bot->GetCamera().GetBody(), false) &&
                        pAttacker->IsPlayer())
                        return pAttacker;
            }
        }
    }

    return nullptr;
}
