#include "botpch.h"
#include "../../playerbot.h"
#include "DruidActions.h"

using namespace ai;

bool CastCasterFormAction::Execute(Event& event)
{
    ai->RemoveShapeshift();
    return true;
}