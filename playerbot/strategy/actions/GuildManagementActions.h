#pragma once

#include "../Action.h"

namespace ai
{
    class GuidManageAction : public Action {
    public:
        GuidManageAction(PlayerbotAI* ai, string name = "guild manage", uint16 opcode = CMSG_GUILD_INVITE) : Action(ai, name), opcode(opcode) {}
        virtual bool Execute(Event event);
        virtual bool isUseful() { return false; }
    protected:
        virtual void SendPacket(WorldPacket data) {};
        virtual Player* GetPlayer(Event event);
        virtual bool PlayerIsValid(Player* member) { return !member->GetGuildId(); };
        virtual uint8 GetRankId(Player* member) { return sGuildMgr.GetGuildById(member->GetGuildId())->GetMemberSlot(member->GetObjectGuid())->RankId; }

        uint16 opcode;
    };

    class GuildInviteAction : public GuidManageAction {
    public:
        GuildInviteAction(PlayerbotAI* ai, string name = "guild invite", uint16 opcode = CMSG_GUILD_INVITE) : GuidManageAction(ai, name, opcode) {}
        virtual bool isUseful() { return bot->GetGuildId() && sGuildMgr.GetGuildById(bot->GetGuildId())->HasRankRight(bot->GetRank(), GR_RIGHT_INVITE); }
    protected:
        virtual void SendPacket(WorldPacket data) { bot->GetSession()->HandleGuildInviteOpcode(data); };
        virtual bool PlayerIsValid(Player* member) { return !member->GetGuildId(); };
    };

    class GuildPromoteAction : public GuidManageAction {
    public:
        GuildPromoteAction(PlayerbotAI* ai, string name = "guild promote", uint16 opcode = CMSG_GUILD_PROMOTE) : GuidManageAction(ai, name, opcode) {}
        virtual bool isUseful() { return bot->GetGuildId() && sGuildMgr.GetGuildById(bot->GetGuildId())->HasRankRight(bot->GetRank(), GR_RIGHT_PROMOTE); }
    protected:
        virtual void SendPacket(WorldPacket data) { bot->GetSession()->HandleGuildPromoteOpcode(data); };
        virtual bool PlayerIsValid(Player* member) { return member->GetGuildId() == bot->GetGuildId() && GetRankId(bot) < GetRankId(member) - 1; };
    };

    class GuildDemoteAction : public GuidManageAction {
    public:
        GuildDemoteAction(PlayerbotAI* ai, string name = "guild demote", uint16 opcode = CMSG_GUILD_DEMOTE) : GuidManageAction(ai, name, opcode) {}
        virtual bool isUseful() { return bot->GetGuildId() && sGuildMgr.GetGuildById(bot->GetGuildId())->HasRankRight(bot->GetRank(), GR_RIGHT_DEMOTE); }
    protected:
        virtual void SendPacket(WorldPacket data) { bot->GetSession()->HandleGuildDemoteOpcode(data); };
        virtual bool PlayerIsValid(Player* member) { return member->GetGuildId() == bot->GetGuildId() && GetRankId(bot) < GetRankId(member); };
    };
    
    class GuildRemoveAction : public GuidManageAction {
    public:
        GuildRemoveAction(PlayerbotAI* ai, string name = "guild remove", uint16 opcode = CMSG_GUILD_REMOVE) : GuidManageAction(ai, name, opcode) {}
        virtual bool isUseful() { return bot->GetGuildId() && sGuildMgr.GetGuildById(bot->GetGuildId())->HasRankRight(bot->GetRank(), GR_RIGHT_REMOVE); }
    protected:
        virtual void SendPacket(WorldPacket data) { bot->GetSession()->HandleGuildRemoveOpcode(data); };
        virtual bool PlayerIsValid(Player* member) { return member->GetGuildId() == bot->GetGuildId() && GetRankId(bot) < GetRankId(member); };
    };

    class GuildManageNearbyAction : public Action {
    public:
        GuildManageNearbyAction(PlayerbotAI* ai) : Action(ai, "guild manage nearby") {}
        virtual bool Execute(Event event);
        virtual bool isUseful();
    };

    class GuildLeaveAction : public Action {
    public:
        GuildLeaveAction(PlayerbotAI* ai) : Action(ai, "guild leave") {}
        virtual bool Execute(Event event);
        virtual bool isUseful() { return bot->GetGuildId(); }
    };
}
