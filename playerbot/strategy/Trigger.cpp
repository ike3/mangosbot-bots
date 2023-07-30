#include "../../botpch.h"
#include "../playerbot.h"
#include "Trigger.h"
#include "Action.h"

using namespace ai;

Event Trigger::Check()
{
	if (IsActive())
	{
		Event event(getName());
		return event;
	}
	Event event;
	return event;
}

Value<Unit*>* Trigger::GetTargetValue()
{
    return context->GetValue<Unit*>(GetTargetName());
}

Unit* Trigger::GetTarget()
{
    return GetTargetValue()->Get();
}

bool Trigger::needCheck(uint32 diff)
{
    if (!checkInterval) return true;

    if ((int32)checkAfter <= (int32)diff)
        return true;

    checkAfter -= diff;
    return false;

}

void Trigger::DelayNextCheck()
{
    checkAfter = checkInterval * 1000 + urand(0, sPlayerbotAIConfig.lagInterval);
}