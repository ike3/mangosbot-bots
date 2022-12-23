#include "botpch.h"
#include "EngineValues.h"


using namespace ai;

bool ActionPossibleValue::Calculate()
{
    Action* action = context->GetAction(getQualifier());
    if (!action)
        return false;
    bool isPossible = action->isPossible();
    return isPossible;
}

