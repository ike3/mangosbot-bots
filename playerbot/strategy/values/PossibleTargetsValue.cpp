#include "botpch.h"
#include "../../playerbot.h"
#include "PossibleTargetsValue.h"
#include "PossibleAttackTargetsValue.h"

#include "../../ServerFacade.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"

using namespace ai;
using namespace MaNGOS;

list<ObjectGuid> PossibleTargetsValue::Calculate()
{
    // Check if the target name has been overridden
    bool shouldIgnoreLos = ignoreLos;
    float rangeCheck = range;
    bool shouldIgnoreValidate = false;
    if (!qualifier.empty())
    {
        shouldIgnoreLos = Qualified::getMultiQualifierInt(qualifier, 0, ":");
        rangeCheck = Qualified::getMultiQualifierInt(qualifier, 1, ":");
        shouldIgnoreValidate = Qualified::getMultiQualifierInt(qualifier, 2, ":");
    }

    list<Unit*> targets;
    FindPossibleTargets(bot, targets, rangeCheck);

    list<ObjectGuid> results;
    for (list<Unit*>::iterator i = targets.begin(); i != targets.end(); ++i)
    {
        Unit* unit = *i;
        if ((shouldIgnoreLos || sServerFacade.IsWithinLOSInMap(bot, unit)) && 
            (shouldIgnoreValidate || AcceptUnit(unit)))
        {
            results.push_back(unit->GetObjectGuid());
        }
    }

    return results;
}

void PossibleTargetsValue::FindUnits(list<Unit*> &targets)
{
    FindPossibleTargets(bot, targets, range);
}

bool PossibleTargetsValue::AcceptUnit(Unit* unit)
{
    // check for CC-ed targets
    bool isCCtarget = false;
    if (bot->getClass() == CLASS_MAGE && ai->HasMyAura("polymorph", unit))
        isCCtarget = true;
    if (bot->getClass() == CLASS_ROGUE && ai->HasMyAura("sap", unit))
        isCCtarget = true;
    if (!isCCtarget && bot->getClass() == CLASS_WARLOCK && ai->HasMyAura("fear", unit))
        isCCtarget = true;
    if (!isCCtarget && bot->getClass() == CLASS_WARLOCK && ai->HasMyAura("banish", unit))
        isCCtarget = true;

    return isCCtarget || PossibleTargetsValue::IsValid(bot, unit, range);
}

void PossibleTargetsValue::FindPossibleTargets(Player* player, list<Unit*>& targets, float range)
{
    MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck u_check(player, range);
    MaNGOS::UnitListSearcher<MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck> searcher(targets, u_check);
    Cell::VisitAllObjects(player, searcher, range);
}

bool PossibleTargetsValue::IsValid(Player* player, Unit* target, float range)
{
    return PossibleAttackTargetsValue::IsValid(target, player, range, true);
}