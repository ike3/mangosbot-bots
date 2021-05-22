#include "botpch.h"
#include "../../playerbot.h"
#include "GenericTriggers.h"
#include "../../LootObjectStack.h"
#include "../../PlayerbotAIConfig.h"

using namespace ai;

bool LowManaTrigger::IsActive()
{
    return AI_VALUE2(bool, "has mana", "self target") && AI_VALUE2(uint8, "mana", "self target") < sPlayerbotAIConfig.lowMana;
}

bool MediumManaTrigger::IsActive()
{
    return AI_VALUE2(bool, "has mana", "self target") && AI_VALUE2(uint8, "mana", "self target") < sPlayerbotAIConfig.mediumMana;
}


bool RageAvailable::IsActive()
{
    return AI_VALUE2(uint8, "rage", "self target") >= amount;
}

bool EnergyAvailable::IsActive()
{
	return AI_VALUE2(uint8, "energy", "self target") >= amount;
}

bool ComboPointsAvailableTrigger::IsActive()
{
    return AI_VALUE2(uint8, "combo", "current target") >= amount;
}

bool LoseAggroTrigger::IsActive()
{
    return !AI_VALUE2(bool, "has aggro", "current target");
}

bool HasAggroTrigger::IsActive()
{
    return AI_VALUE2(bool, "has aggro", "current target");
}

bool PanicTrigger::IsActive()
{
    return AI_VALUE2(uint8, "health", "self target") < sPlayerbotAIConfig.criticalHealth &&
		(!AI_VALUE2(bool, "has mana", "self target") || AI_VALUE2(uint8, "mana", "self target") < sPlayerbotAIConfig.lowMana);
}

bool OutNumberedTrigger::IsActive()
{
    int32 botLevel = bot->getLevel();
    uint32 friendPower = 200, foePower = 0;
    for (auto &attacker : ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("attackers")->Get())
    {
     
        Creature* creature = ai->GetCreature(attacker);
        if (!creature)
            continue;

        int32 dLevel = creature->getLevel() - botLevel;

        if(dLevel > -10)
            foePower = std::max(100 + 10 * dLevel, dLevel * 200);
    }

    if (!foePower)
        return false;

    for (auto & helper : ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest friendly players")->Get())
    {
        Unit* player = ai->GetUnit(helper);

        if (!player || player == bot)
            continue;

        int32 dLevel = player->getLevel() - botLevel;

        if (dLevel > -10 && bot->GetDistance(player) < 10.0f)
            friendPower += std::max(200 + 20 * dLevel, dLevel * 200);
    }

    return friendPower < foePower;
}

bool BuffTrigger::IsActive()
{
    Unit* target = GetTarget();
	return SpellTrigger::IsActive() &&
        !ai->HasAura(spell, target, true)/* &&
        (!AI_VALUE2(bool, "has mana", "self target") || AI_VALUE2(uint8, "mana", "self target") > sPlayerbotAIConfig.mediumMana)*/
        ;
}

Value<Unit*>* BuffOnPartyTrigger::GetTargetValue()
{
	return context->GetValue<Unit*>("party member without aura", spell);
}

Value<Unit*>* DebuffOnAttackerTrigger::GetTargetValue()
{
	return context->GetValue<Unit*>("attacker without aura", spell);
}

bool NoAttackersTrigger::IsActive()
{
    return !AI_VALUE(Unit*, "current target") && AI_VALUE(uint8, "attacker count") > 0;
}

bool InvalidTargetTrigger::IsActive()
{
    return AI_VALUE2(bool, "invalid target", "current target");
}

bool NoTargetTrigger::IsActive()
{
	return !AI_VALUE(Unit*, "current target");
}

bool MyAttackerCountTrigger::IsActive()
{
    return AI_VALUE(uint8, "my attacker count") >= amount;
}

bool AoeTrigger::IsActive()
{
    return AI_VALUE(uint8, "aoe count") >= amount && AI_VALUE(uint8, "attacker count") >= amount;
}

bool DebuffTrigger::IsActive()
{
    return BuffTrigger::IsActive() && AI_VALUE2(uint8, "health", GetTargetName()) > 15;
}

bool SpellTrigger::IsActive()
{
	return GetTarget();
}

bool SpellCanBeCastTrigger::IsActive()
{
	Unit* target = GetTarget();
	return target && ai->CanCastSpell(spell, target, true);
}

bool RandomTrigger::IsActive()
{
    if (time(0) - lastCheck < sPlayerbotAIConfig.repeatDelay / 1000)
        return false;

    lastCheck = time(0);
    int k = (int)(probability / sPlayerbotAIConfig.randomChangeMultiplier);
    if (k < 1) k = 1;
    return (rand() % k) == 0;
}

bool AndTrigger::IsActive()
{
    return ls->IsActive() && rs->IsActive();
}

string AndTrigger::getName()
{
    std::string name(ls->getName());
    name = name + " and ";
    name = name + rs->getName();
    return name;
}

bool BoostTrigger::IsActive()
{
	return BuffTrigger::IsActive() && AI_VALUE(uint8, "balance") <= balance;
}

bool ItemCountTrigger::IsActive()
{
	return AI_VALUE2(uint8, "item count", item) < count;
}

bool InterruptSpellTrigger::IsActive()
{
	return SpellTrigger::IsActive() && ai->IsInterruptableSpellCasting(GetTarget(), getName(), true);
}

bool HasAuraTrigger::IsActive()
{
	return ai->HasAura(getName(), GetTarget());
}

bool HasNoAuraTrigger::IsActive()
{
    return !ai->HasAura(getName(), GetTarget());
}

bool TankAoeTrigger::IsActive()
{
    if (!AI_VALUE(uint8, "attacker count"))
        return false;

    Unit* currentTarget = AI_VALUE(Unit*, "current target");
    if (!currentTarget)
        return true;

    Unit* tankTarget = AI_VALUE(Unit*, "tank target");
    if (!tankTarget || currentTarget == tankTarget)
        return false;
#ifdef CMANGOS
    return currentTarget->GetVictim() == AI_VALUE(Unit*, "self target");
#endif
#ifdef MANGOS
    return currentTarget->getVictim() == AI_VALUE(Unit*, "self target");
#endif
}

bool IsBehindTargetTrigger::IsActive()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    return target && AI_VALUE2(bool, "behind", "current target");
}

bool IsNotBehindTargetTrigger::IsActive()
{
    Unit* target = AI_VALUE(Unit*, "current target");
    return target && !AI_VALUE2(bool, "behind", "current target");
}

bool IsNotFacingTargetTrigger::IsActive()
{
    return !AI_VALUE2(bool, "facing", "current target");
}

bool HasCcTargetTrigger::IsActive()
{
    return AI_VALUE2(Unit*, "cc target", getName()) && !AI_VALUE2(Unit*, "current cc target", getName());
}

bool NoMovementTrigger::IsActive()
{
	return !AI_VALUE2(bool, "moving", "self target");
}

bool NoPossibleTargetsTrigger::IsActive()
{
    list<ObjectGuid> targets = AI_VALUE(list<ObjectGuid>, "possible targets");
    return !targets.size();
}

bool PossibleAddsTrigger::IsActive()
{
    return AI_VALUE(bool, "possible adds") && !AI_VALUE(ObjectGuid, "pull target");
}

bool NotDpsTargetActiveTrigger::IsActive()
{
    Unit* dps = AI_VALUE(Unit*, "dps target");
    Unit* target = AI_VALUE(Unit*, "current target");
    Unit* enemy = AI_VALUE(Unit*, "enemy player target");
    
    // do not switch if enemy target
    if (target && target == enemy && sServerFacade.IsAlive(target))
        return false;

    return dps && target != dps;
}

bool NotDpsAoeTargetActiveTrigger::IsActive()
{
    Unit* dps = AI_VALUE(Unit*, "dps aoe target");
    Unit* target = AI_VALUE(Unit*, "current target");
    Unit* enemy = AI_VALUE(Unit*, "enemy player target");

    // do not switch if enemy target
    if (target && target == enemy && sServerFacade.IsAlive(target))
        return false;

    return dps && target != dps;
}

bool IsSwimmingTrigger::IsActive()
{
    return AI_VALUE2(bool, "swimming", "self target");
}

bool HasNearestAddsTrigger::IsActive()
{
    list<ObjectGuid> targets = AI_VALUE(list<ObjectGuid>, "nearest adds");
    return targets.size();
}

bool HasItemForSpellTrigger::IsActive()
{
	string spell = getName();
    uint32 spellId = AI_VALUE2(uint32, "spell id", spell);
    return spellId && AI_VALUE2(Item*, "item for spell", spellId);
}


bool TargetChangedTrigger::IsActive()
{
    Unit* oldTarget = context->GetValue<Unit*>("old target")->Get();
    Unit* target = context->GetValue<Unit*>("current target")->Get();
    return target && oldTarget != target;
}

Value<Unit*>* InterruptEnemyHealerTrigger::GetTargetValue()
{
    return context->GetValue<Unit*>("enemy healer target", spell);
}

Value<Unit*>* SnareTargetTrigger::GetTargetValue()
{
    return context->GetValue<Unit*>("snare target", spell);
}

bool StayTimeTrigger::IsActive()
{
    time_t stayTime = AI_VALUE(time_t, "stay time");
    time_t now = time(0);
    return delay && stayTime && now > stayTime + 2 * delay / 1000;
}

bool IsMountedTrigger::IsActive()
{
    return AI_VALUE2(bool, "mounted", "self target");
}

bool CorpseNearTrigger::IsActive()
{
    return bot->GetCorpse() && bot->GetCorpse()->IsWithinDistInMap(bot, CORPSE_RECLAIM_RADIUS, true);
}

bool IsFallingTrigger::IsActive()
{
    return bot->HasMovementFlag(MOVEFLAG_FALLING);
}

bool IsFallingFarTrigger::IsActive()
{
    return bot->HasMovementFlag(MOVEFLAG_FALLINGFAR);
}