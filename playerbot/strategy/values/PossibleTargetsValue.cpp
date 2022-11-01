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
    if (bot->getClass() == CLASS_MAGE && ai->HasMyAura("polymorph", unit))
        isCCtarget = true;
    if (bot->getClass() == CLASS_ROGUE && ai->HasMyAura("sap", unit))
        isCCtarget = true;
    if (!isCCtarget && bot->getClass() == CLASS_WARLOCK && ai->HasMyAura("fear", unit))
        isCCtarget = true;
    if (!isCCtarget && bot->getClass() == CLASS_WARLOCK && ai->HasMyAura("banish", unit))
        isCCtarget = true;

    return isCCtarget || AttackersValue::IsPossibleTarget(unit, bot, range);
}
