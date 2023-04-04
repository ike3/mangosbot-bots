#include "botpch.h"
#include "../../playerbot.h"
#include "ResetInstancesAction.h"

using namespace ai;

bool ResetInstancesAction::Execute(Event& event)
{
    WorldPacket packet(CMSG_RESET_INSTANCES, 0);
    bot->GetSession()->HandleResetInstancesOpcode(packet);

    ai->TellMaster("Resetting all instances", PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
    return true;
}

bool ResetRaidsAction::Execute(Event& event)
{
    Player::BoundInstancesMap& binds = bot->GetBoundInstances();
    for (Player::BoundInstancesMap::iterator itr = binds.begin(); itr != binds.end(); ++itr)
    {
        if (itr->first != bot->GetMapId())
        {
            bot->UnbindInstance(itr);
        }
    }

    return true;
}
