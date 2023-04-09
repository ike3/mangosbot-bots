#pragma once
#include "GenericActions.h"

namespace ai
{
    class InviteToGroupAction : public ChatCommandAction
    {
    public:
        InviteToGroupAction(PlayerbotAI* ai, string name = "invite") : ChatCommandAction(ai, name) {}

        virtual bool Execute(Event& event) override
        {
            Player* master = event.getOwner();
            return Invite(bot, master);
        }

        virtual bool Invite(Player* inviter, Player* player);
    };

    class JoinGroupAction : public InviteToGroupAction
    {
    public:
        JoinGroupAction(PlayerbotAI* ai, string name = "join") : InviteToGroupAction(ai, name) {}
        virtual bool Execute(Event& event) override;
    };

    class LfgAction : public InviteToGroupAction
    {
    public:
        LfgAction(PlayerbotAI* ai, string name = "lfg") : InviteToGroupAction(ai, name) {}
        virtual bool Execute(Event& event) override;
    };

    class InviteNearbyToGroupAction : public InviteToGroupAction
    {
    public:
        InviteNearbyToGroupAction(PlayerbotAI* ai, string name = "invite nearby") : InviteToGroupAction(ai, name) {}
        virtual bool Execute(Event& event) override;
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
        virtual bool Execute(Event& event) override;
        virtual bool isUseful() { return bot->GetGuildId() && InviteNearbyToGroupAction::isUseful(); };

    private:
        vector<Player*> getGuildMembers();
    };
}
