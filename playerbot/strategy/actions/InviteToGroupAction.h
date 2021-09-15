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

    //Generic guid member finder
    class FindGuildMembers
    {
    public:
        FindGuildMembers() {};

        void operator()(Player* player) { data.push_back(player); };
        vector<Player*> const GetResult() { return data; };
    private:
        vector<Player*> data;
    };

    class InviteGuildToGroupAction : public InviteNearbyToGroupAction
    {
    public:
        InviteGuildToGroupAction(PlayerbotAI* ai, string name = "invite guild") : InviteNearbyToGroupAction(ai, name) {}

        virtual bool Execute(Event event);
        virtual bool isUseful() { return bot->GetGuildId() && InviteNearbyToGroupAction::isUseful(); };
    private:
        vector<Player*> getGuildMembers();
    };
}
