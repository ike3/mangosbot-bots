#pragma once

#include "../Action.h"
#include "InventoryAction.h"

namespace ai
{
    class GuildAcceptAction : public Action {
    public:
        GuildAcceptAction(PlayerbotAI* ai) : Action(ai, "guild accept") {}
        virtual bool Execute(Event event);
    };

    class GuidInviteAction : public Action {
    public:
        GuidInviteAction(PlayerbotAI* ai, string name = "guild invite") : Action(ai, name) {}
        virtual bool Execute(Event event);
        virtual bool isUseful() { return bot->GetGuildId() && sGuildMgr.GetGuildById(bot->GetGuildId())->HasRankRight(bot->GetRank(), GR_RIGHT_INVITE); }
    };

    class GuidInviteNearbyAction : public GuidInviteAction {
    public:
        GuidInviteNearbyAction(PlayerbotAI* ai) : GuidInviteAction(ai, "guild invite nearby") {}
        virtual bool Execute(Event event);
        virtual bool isUseful();
    };

    class GuildLeaveAction : public GuidInviteAction {
    public:
        GuildLeaveAction(PlayerbotAI* ai) : GuidInviteAction(ai, "guild leave") {}
        virtual bool Execute(Event event);
        virtual bool isUseful() { return bot->GetGuildId(); }
    };
}
