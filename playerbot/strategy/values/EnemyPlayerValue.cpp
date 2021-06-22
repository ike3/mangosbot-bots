#include "botpch.h"
#include "../../playerbot.h"
#include "EnemyPlayerValue.h"
#include "TargetValue.h"

using namespace ai;
using namespace std;

/*class FindEnemyPlayerStrategy : public FindTargetStrategy
{
public:
    FindEnemyPlayerStrategy(PlayerbotAI* ai) : FindTargetStrategy(ai)
    {
    }

public:
    virtual void CheckAttacker(Unit* attacker, ThreatManager* threatManager)
    {
        if (!result)
        {
            Player* enemy = dynamic_cast<Player*>(attacker);
            if (enemy &&
                    ai->IsOpposing(enemy) &&
                    enemy->IsPvP() &&
					!sPlayerbotAIConfig.IsInPvpProhibitedZone(enemy->GetAreaId()) &&
                    !enemy->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE) &&
                    !enemy->HasStealthAura() &&
                    !enemy->HasInvisibilityAura() &&
                    !(enemy->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION)))
                result = attacker;
        }
    }

};

Unit* EnemyPlayerValue::Calculate()
{
    FindEnemyPlayerStrategy strategy(ai);
    return FindTarget(&strategy);
}*/

bool NearestEnemyPlayersValue::AcceptUnit(Unit* unit)
{
    Player* enemy = dynamic_cast<Player*>(unit);
    if (enemy &&
        ai->IsOpposing(enemy) &&
        enemy->IsPvP() &&
        !sPlayerbotAIConfig.IsInPvpProhibitedZone(enemy->GetAreaId()) &&
        !enemy->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE) &&
        //!enemy->HasStealthAura() &&
        //!enemy->HasInvisibilityAura() &&
        enemy->IsVisibleForOrDetect(bot, enemy, false) &&
        !(enemy->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION))
        )
        return true;
}

Unit* EnemyPlayerValue::Calculate()
{
    // from VMaNGOS
    // 1. Check units we are currently in combat with.

    std::list<Unit*> targets;
    Unit* pVictim = bot->GetVictim();
    HostileReference* pReference = bot->getHostileRefManager().getFirst();

    while (pReference)
    {
        ThreatManager *threatManager = pReference->getSource();
        if (Unit* pTarget = threatManager->getOwner())
        {
            if (pTarget != pVictim &&
                pTarget->IsPlayer() &&
                pTarget->IsVisibleForOrDetect(bot, pTarget, false) &&
                bot->IsWithinDist(pTarget, VISIBILITY_DISTANCE_NORMAL))
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
        targets.sort([this](Unit* pUnit1, const Unit* pUnit2)
        {
            return bot->GetDistance(pUnit1) < bot->GetDistance(pUnit2);
        });

        return *targets.begin();
    }

    // 2. Find enemy player in range.

    list<ObjectGuid> players = AI_VALUE(list<ObjectGuid>, "nearest enemy players");
    float const maxAggroDistance = GetMaxAttackDistance();

    for (const auto& gTarget : players)
    {
        Unit* pUnit = ai->GetUnit(gTarget);
        if (!pUnit)
            continue;

        Player* pTarget = dynamic_cast<Player*>(pUnit);
        if (!pTarget)
            continue;

        if (pTarget == pVictim)
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

        // Aggro weak enemies from further away.
        uint32 const aggroDistance = bot->GetHealth() > pTarget->GetHealth() ? maxAggroDistance : 20.0f;
        if (!bot->IsWithinDist(pTarget, aggroDistance))
            continue;

        if (bot->IsWithinLOSInMap(pTarget) && (fabs(bot->GetPositionZ() - pTarget->GetPositionZ()) < 30.0f))
            return pTarget;
    }

    // 3. Check party attackers.

    if (Group* pGroup = bot->GetGroup())
    {
        for (GroupReference* itr = pGroup->GetFirstMember(); itr != nullptr; itr = itr->next())
        {
            if (Unit* pMember = itr->getSource())
            {
                if (pMember == bot)
                    continue;

                if (sServerFacade.GetDistance2d(bot, pMember) > 30.0f)
                    continue;

                if (Unit* pAttacker = pMember->getAttackerForHelper())
                    if (bot->IsWithinDist(pAttacker, maxAggroDistance * 2.0f) &&
                        bot->IsWithinLOSInMap(pAttacker) &&
                        pAttacker != pVictim &&
                        pAttacker->IsVisibleForOrDetect(bot, pAttacker, false) &&
                        pAttacker->IsPlayer())
                        return pAttacker;
            }
        }
    }

    return nullptr;
}
