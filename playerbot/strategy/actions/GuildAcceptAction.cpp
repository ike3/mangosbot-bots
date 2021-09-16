#include "botpch.h"
#include "../../playerbot.h"
#include "GuildAcceptAction.h"

using namespace std;
using namespace ai;

bool GuildAcceptAction::Execute(Event event)
{
    WorldPacket p(event.getPacket());
    p.rpos(0);
    Player* inviter = nullptr;
    std::string Invitedname;
    p >> Invitedname;

    if (normalizePlayerName(Invitedname))
        inviter = ObjectAccessor::FindPlayerByName(Invitedname.c_str());

    if (!inviter)
        return false;

    bool accept = true;
    uint32 guildId = inviter->GetGuildId();
    if (!guildId)
    {
        ai->TellError("You are not in a guild!");
        accept = false;
    }
    else if (bot->GetGuildId())
    {
        ai->TellError("Sorry, I am in a guild already");
        accept = false;
    }
    else if (!ai->GetSecurity()->CheckLevelFor(PLAYERBOT_SECURITY_INVITE, false, inviter, true))
    {
        ai->TellError("Sorry, I don't want to join your guild :(");
        accept = false;
    }

    WorldPacket packet;
    if (accept)
    {
        bot->GetSession()->HandleGuildAcceptOpcode(packet);
    }
    else
    {
        bot->GetSession()->HandleGuildDeclineOpcode(packet);
    }
    return true;
}

bool GuidInviteAction::Execute(Event event)
{
    ObjectGuid guid = event.getObject();

    Player* master = GetMaster();
    if (!master)
    {
        if (!guid)
            guid = bot->GetSelectionGuid();
    }
    else {
        if (!guid)
            guid = master->GetSelectionGuid();
    }

    if (!guid)
        return false;

    Player* player = sObjectMgr.GetPlayer(guid);

    if (!player)
        return false;

    WorldPacket data(CMSG_GUILD_INVITE, 8);

    data << player->GetName();

    bot->GetSession()->HandleGuildInviteOpcode(data);

    return true;
};

bool GuidInviteNearbyAction::Execute(Event event)
{
    uint32 found = 0;

    Guild* guild = sGuildMgr.GetGuildById(bot->GetGuildId());
    MemberSlot* botMember = guild->GetMemberSlot(bot->GetObjectGuid());

    list<ObjectGuid> nearGuids = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest friendly players")->Get();
    for (auto& i : nearGuids)
    {
        Player* player = sObjectMgr.GetPlayer(i);

        if (!player || bot == player)
            continue;

        if (player->GetGuildId() != bot->GetGuildId())
            continue;

        if(player->GetGuildId()) //Promote or demote nearby members based on chance.
        {          
            MemberSlot* member = guild->GetMemberSlot(player->GetObjectGuid());
            uint32 dCount = AI_VALUE(uint32, "death count");

            if(dCount < 2 || !urand(0,10))
            if (botMember->RankId > member->RankId + 1 && guild->GetRankRights(botMember->RankId) & GR_RIGHT_PROMOTE && !urand(0, 10))
            {
                WorldPacket data(CMSG_GUILD_PROMOTE, 8);
                data << player->GetName();
                bot->GetSession()->HandleGuildPromoteOpcode(data);

                continue;
            }

            if (dCount > 3 || !urand(0, 10))
            if (botMember->RankId > member->RankId && botMember->RankId < guild->GetLowestRank() - 1 && guild->GetRankRights(botMember->RankId) & GR_RIGHT_DEMOTE && !urand(0, 10))
            {
                WorldPacket data(CMSG_GUILD_DEMOTE, 8);
                data << player->GetName();
                bot->GetSession()->HandleGuildDemoteOpcode(data);

                continue;
            }

            continue;
        }

        if (!(guild->GetRankRights(botMember->RankId) & GR_RIGHT_INVITE))
            continue;

        if (player->GetGuildIdInvited())
            continue;

        PlayerbotAI* botAi = player->GetPlayerbotAI();

        if (botAi)
        {
            
            if (botAi->GetGuilderType() == GuilderType::SOLO && !botAi->HasRealPlayerMaster()) //Do not invite solo players.
                continue;

            
            if (botAi->HasActivePlayerMaster()) //Do not invite alts of active players. 
                continue;
        }
        else
        {
            if (!sPlayerbotAIConfig.randomBotGroupNearby)
                return false;
        }

        if (sServerFacade.GetDistance2d(bot, player) > sPlayerbotAIConfig.sightDistance)
            continue;

        //Parse rpg target to quest action.
        WorldPacket p(SMSG_GUILD_INVITE);
        p << i;
        p.rpos(0);

        if (GuidInviteAction::Execute(Event("guild invite nearby", p)))
            found++;
    }

    return found > 0;
};

bool GuidInviteNearbyAction::isUseful()
{
    if (!bot->GetGuildId())
        return false;

    Guild* guild = sGuildMgr.GetGuildById(bot->GetGuildId());
    MemberSlot* botMember = guild->GetMemberSlot(bot->GetObjectGuid());

    return  guild->GetRankRights(botMember->RankId) & (GR_RIGHT_DEMOTE | GR_RIGHT_PROMOTE | GR_RIGHT_INVITE);
}

bool GuildLeaveAction::Execute(Event event)
{
    if (event.getSource() != "trigger")
    {
        Player* inviter = event.getOwner();

        if (inviter && !ai->GetSecurity()->CheckLevelFor(PLAYERBOT_SECURITY_INVITE, false, inviter, true))
        {
            ai->TellError("Sorry, I am happy in my guild :)");
        }
    }

    WorldPacket packet;
    bot->GetSession()->HandleGuildLeaveOpcode(packet);
    return true;
}