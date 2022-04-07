#include "botpch.h"
#include "../../playerbot.h"
#include "TravelTriggers.h"

#include "../../PlayerbotAIConfig.h"
#include "../../Travelmgr.h"
#include "ServerFacade.h"
using namespace ai;

bool NoTravelTargetTrigger::IsActive()
{
    return !context->GetValue<TravelTarget *>("travel target")->Get()->isActive();
}

bool FarFromTravelTargetTrigger::IsActive()
{
    return context->GetValue<TravelTarget*>("travel target")->Get()->isTraveling();
}

bool NearDarkPortalTrigger::IsActive()
{
    return sServerFacade.GetAreaId(bot) == 72;
}

bool AtDarkPortalAzerothTrigger::IsActive()
{
    if (sServerFacade.GetAreaId(bot) == 72)
    {
        if (sServerFacade.GetDistance2d(bot, -11906.9f, -3208.53f) < 20.0f)
        {
            return true;
        }
    }
    return false;
}

bool AtDarkPortalOutlandTrigger::IsActive()
{
    if (sServerFacade.GetAreaId(bot) == 3539)
    {
        if (sServerFacade.GetDistance2d(bot, -248.1939f, 921.919f) < 10.0f)
        {
            return true;
        }
    }
    return false;
}
