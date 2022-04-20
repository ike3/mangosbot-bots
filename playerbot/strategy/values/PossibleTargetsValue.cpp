#include "botpch.h"
#include "../../playerbot.h"
#include "PossibleTargetsValue.h"
#include "AttackersValue.h"

#include "../../ServerFacade.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"

using namespace ai;
using namespace MaNGOS;

void PossibleTargetsValue::FindUnits(list<Unit*> &targets)
{
    MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck u_check(bot, range);
    MaNGOS::UnitListSearcher<MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck> searcher(targets, u_check);
    Cell::VisitAllObjects(bot, searcher, range);
}

bool PossibleTargetsValue::AcceptUnit(Unit* unit)
{
    // check for CC-ed targets
    bool isCCtarget = false;
    if (ai->HasAura("polymorph", unit, false, false, -1, true))
        isCCtarget = true;
    if (ai->HasAura("sap", unit, false, false, -1, true))
        isCCtarget = true;
    if (ai->HasAura("fear", unit, false, false, -1, true))
        isCCtarget = true;
    if (ai->HasAura("banish", unit, false, false, -1, true))
        isCCtarget = true;

    return isCCtarget || AttackersValue::IsPossibleTarget(unit, bot, range);
}
