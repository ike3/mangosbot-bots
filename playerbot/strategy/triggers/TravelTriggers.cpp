#include "botpch.h"
#include "../../playerbot.h"
#include "TravelTriggers.h"

#include "../../PlayerbotAIConfig.h"
#include "../../Travelmgr.h"
using namespace ai;

bool NoTravelTargetTrigger::IsActive()
{
    return !context->GetValue<TravelTarget *>("travel target")->Get()->isActive();
}

bool FarFromTravelTargetTrigger::IsActive()
{
    return context->GetValue<TravelTarget*>("travel target")->Get()->isTraveling();
}
