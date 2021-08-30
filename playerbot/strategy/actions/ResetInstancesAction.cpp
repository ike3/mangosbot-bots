#include "botpch.h"
#include "../../playerbot.h"
#include "ResetInstancesAction.h"


using namespace ai;

bool ResetInstancesAction::Execute(Event event)
{
    WorldPacket packet(CMSG_RESET_INSTANCES, 0);
    bot->GetSession()->HandleResetInstancesOpcode(packet);

    ai->TellMaster("Resetting all instances");
    return true;
}
