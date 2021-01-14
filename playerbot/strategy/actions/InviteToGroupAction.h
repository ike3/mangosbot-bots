#pragma once

#include "../Action.h"

namespace ai
{
    class InviteToGroupAction : public Action
    {
    public:
        InviteToGroupAction(PlayerbotAI* ai, string name = "invite") : Action(ai, name) {}

        virtual bool Execute(Event event)
        {
            Player* master = event.getOwner();
            return Invite(master);
        }

        virtual bool Invite(Player* player);
    };


    class InviteNearbyToGroupAction : public InviteToGroupAction
    {
    public:
        InviteNearbyToGroupAction(PlayerbotAI* ai, string name = "invite nearby") : InviteToGroupAction(ai, name) {}

        virtual bool Execute(Event event);
        virtual bool isUseful();
    };
}
