using namespace ai;
#include "StrategyValues.h"

bool IsUsefulValue::Calculate()
{
    Action* action = context->GetAction(getQualifier());

    if (!action)
        return false;

    return action->isUseful();
}