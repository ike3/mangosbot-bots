#include "botpch.h"
#include "../../playerbot.h"
#include "PaladinActions.h"

using namespace ai;

string GetActualBlessingOfMight(Unit* target, bool greater = false)
{
    switch (target->getClass())
    {
    case CLASS_MAGE:
    case CLASS_PRIEST:
    case CLASS_WARLOCK:
        return greater ? "greater blessing of wisdom" : "blessing of wisdom";
    }
    return greater ? "greater blessing of might" : "blessing of might";
}

string GetActualBlessingOfWisdom(Unit* target, bool greater = false)
{
    switch (target->getClass())
    {
    case CLASS_WARRIOR:
    case CLASS_ROGUE:
        return greater ? "greater blessing of might" : "blessing of might";
    }
    return greater ? "greater blessing of wisdom" : "blessing of wisdom";
}

Value<Unit*>* CastBlessingOnPartyAction::GetTargetValue()
{
    Value<Unit*>* smallBless = context->GetValue<Unit*>("party member without my aura",
        "blessing of kings,blessing of might,blessing of wisdom");

    Value<Unit*>* bigBless = context->GetValue<Unit*>("party member without my aura",
        "greater blessing of kings,greater blessing of might,greater blessing of wisdom");

    Value<Unit*>* actualBless = context->GetValue<Unit*>("party member without my aura",
        spell);

    if (!actualBless && !bigBless && !smallBless)
        return NULL;

    if (bigBless && actualBless == smallBless)
        return NULL;

    if (bigBless && smallBless == bigBless && smallBless == actualBless)
        return bigBless;

    if (smallBless && bigBless && actualBless && actualBless != bigBless)
        return smallBless;

    if (smallBless && bigBless && actualBless != bigBless && smallBless == actualBless)
        return bigBless;

    if (smallBless && !bigBless && smallBless == actualBless)
        return smallBless;

    if (smallBless && bigBless && actualBless == bigBless)
        return bigBless;

    return actualBless;
}

bool CastBlessingOfMightAction::Execute(Event event)
{
    Unit* target = GetTarget();
    if (!target) return false;

    return ai->CastSpell(GetActualBlessingOfMight(target), target);
}

bool CastGreaterBlessingOfMightAction::Execute(Event event)
{
    Unit* target = GetTarget();
    if (!target) return false;

    return ai->CastSpell(GetActualBlessingOfMight(target, true), target);
}

bool CastBlessingOfMightOnPartyAction::Execute(Event event)
{
    Unit* target = GetTarget();
    if (!target) return false;

    return ai->CastSpell(GetActualBlessingOfMight(target), target);
}

bool CastGreaterBlessingOfMightOnPartyAction::Execute(Event event)
{
    Unit* target = GetTarget();
    if (!target) return false;

    return ai->CastSpell(GetActualBlessingOfMight(target, true), target);
}

bool CastBlessingOfWisdomAction::Execute(Event event)
{
    Unit* target = GetTarget();
    if (!target) return false;

    return ai->CastSpell(GetActualBlessingOfWisdom(target), target);
}

bool CastGreaterBlessingOfWisdomAction::Execute(Event event)
{
    Unit* target = GetTarget();
    if (!target) return false;

    return ai->CastSpell(GetActualBlessingOfWisdom(target, true), target);
}

bool CastBlessingOfWisdomOnPartyAction::Execute(Event event)
{
    Unit* target = GetTarget();
    if (!target) return false;

    return ai->CastSpell(GetActualBlessingOfWisdom(target), target);
}

bool CastGreaterBlessingOfWisdomOnPartyAction::Execute(Event event)
{
    Unit* target = GetTarget();
    if (!target) return false;

    return ai->CastSpell(GetActualBlessingOfWisdom(target, true), target);
}
