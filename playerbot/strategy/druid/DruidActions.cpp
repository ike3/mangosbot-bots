#include "botpch.h"
#include "../../playerbot.h"
#include "DruidActions.h"

using namespace ai;

bool CastCasterFormAction::Execute(Event& event)
{
    ai->RemoveShapeshift();
    return true;
}

Value<Unit*>* CastHibernateCcAction::GetTargetValue()
{
    return context->GetValue<Unit*>("cc target", "hibernate");
}

bool CastHibernateCcAction::Execute(Event& event)
{
    return ai->CastSpell("hibernate", GetTarget());
}
