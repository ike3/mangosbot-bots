#pragma once

#include "botpch.h"
#include "../../playerbot.h"
#include "InviteToGroupAction.h"
#include "../../ServerFacade.h"
#include "../values/Formations.h"


using namespace ai;

namespace ai
{
    bool InviteToGroupAction::Invite(Player* inviter, Player* player)
    {
        if (!player)
            return false;

        if (inviter == player)
            return false;

        if (!player->GetPlayerbotAI() && !ai->GetSecurity()->CheckLevelFor(PlayerbotSecurityLevel::PLAYERBOT_SECURITY_INVITE, true, player))
            return false;

        if (Group* group = inviter->GetGroup())
        {
            if(player->GetPlayerbotAI() && !player->GetPlayerbotAI()->IsRealPlayer())
                if (!group->IsRaidGroup() && group->GetMembersCount() > 4)
                    group->ConvertToRaid();
        }

        WorldPacket p;
        uint32 roles_mask = 0;
        p << player->GetName();
        p << roles_mask;
        inviter->GetSession()->HandleGroupInviteOpcode(p);

        return true;
    }

    bool JoinGroupAction::Execute(Event& event)
    {
        if (bot->InBattleGround())
            return false;

        if (bot->InBattleGroundQueue())
            return false;

        Player* master = event.getOwner();

        Group* group = master->GetGroup();

        if (group)
        {
            if (group->IsFull())
                return false;

            if (bot->GetGroup() == group)
                return false;
        }

        if (bot->GetGroup())
            if (!ai->DoSpecificAction("leave", event, true))
                return false;

        return Invite(master, bot);
    }

    bool InviteNearbyToGroupAction::Execute(Event& event)
    {
        if (!bot->GetGroup())  //Select a random formation to copy.
        {
            vector<string> formations = { "melee","queue","chaos","circle","line","shield","arrow","near","far"};
            FormationValue* value = (FormationValue*)context->GetValue<Formation*>("formation");
            string newFormation = formations[urand(0, formations.size() - 1)];
            value->Load(newFormation);
        }

        list<ObjectGuid> nearGuids = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest friendly players")->Get();
        for (auto& i : nearGuids)
        {
            Player* player = sObjectMgr.GetPlayer(i);

            if (!player)
                continue;

            if (player == bot)
                continue;

            if (player->GetMapId() != bot->GetMapId())
                continue;

            if (player->GetGroup())
                continue;

            if (!sPlayerbotAIConfig.randomBotInvitePlayer && player->isRealPlayer())
                continue;

            Group* group = bot->GetGroup();

            if (player->isDND())
                continue;

            if (player->IsBeingTeleported())
                continue;

            PlayerbotAI* botAi = player->GetPlayerbotAI();

            if (botAi)
            {
                if (botAi->GetGrouperType() == GrouperType::SOLO && !botAi->HasRealPlayerMaster()) //Do not invite solo players. 
                    continue;

                if (botAi->HasActivePlayerMaster()) //Do not invite alts of active players. 
                    continue;
            }

            if (abs(int32(player->GetLevel() - bot->GetLevel())) > 2)
                continue;

            if (sServerFacade.GetDistance2d(bot, player) > sPlayerbotAIConfig.spellDistance)
                continue;

            //When inviting the 5th member of the group convert to raid for future invites.
            if (group && ai->GetGrouperType() > GrouperType::LEADER_5 && !group->IsRaidGroup() && bot->GetGroup()->GetMembersCount() > 3)
                group->ConvertToRaid();

            if (sPlayerbotAIConfig.inviteChat && sRandomPlayerbotMgr.IsFreeBot(bot))
            {
                map<string, string> placeholders;
                placeholders["%name"] = player->GetName();

                if(group && group->IsRaidGroup())
                    bot->Say(BOT_TEXT2("Hey %name do you want join my raid?", placeholders), (bot->GetTeam() == ALLIANCE ? LANG_COMMON : LANG_ORCISH));                    
                else
                    bot->Say(BOT_TEXT2("Hey %name do you want join my group?", placeholders), (bot->GetTeam() == ALLIANCE ? LANG_COMMON : LANG_ORCISH));
            }

            return Invite(bot, player);
        }

        return false;
    }

    bool InviteNearbyToGroupAction::isUseful()
    {
        if (!sPlayerbotAIConfig.randomBotGroupNearby)
            return false;

        if (bot->InBattleGround())
            return false;
        
        if (bot->InBattleGroundQueue())
            return false;

        GrouperType grouperType = ai->GetGrouperType();

        if (grouperType == GrouperType::SOLO || grouperType == GrouperType::MEMBER)
            return false;

        Group* group = bot->GetGroup();

        if (group)
        {
            if (group->IsRaidGroup() && group->IsFull())
                return false;

            if (ai->GetGroupMaster() != bot)
                return false;

            uint32 memberCount = group->GetMembersCount();

            if (memberCount >= uint8(grouperType))
                return false;
        }

        if (ai->HasActivePlayerMaster()) //Alts do not invite randomly          
           return false;

        return true;
    }

    vector<Player*> InviteGuildToGroupAction::getGuildMembers()
    {
        Guild* guild = sGuildMgr.GetGuildById(bot->GetGuildId());

        FindGuildMembers worker;
        guild->BroadcastWorker(worker);

        return worker.GetResult();
    }

    bool InviteGuildToGroupAction::Execute(Event& event)
    {
        Guild* guild = sGuildMgr.GetGuildById(bot->GetGuildId());

        for (auto& member : getGuildMembers())
        {
            Player* player = member;

            if (!player)
                continue;

            if (player->GetGroup())
                continue;

            if (player->isDND())
                continue;

            if (!sPlayerbotAIConfig.randomBotInvitePlayer && player->isRealPlayer())
                continue;

            if (player->IsBeingTeleported())
                continue;

            if (player->GetMapId() != bot->GetMapId() && player->GetLevel() < 30)
                continue;

            if (WorldPosition(player).distance(bot) > 1000 && player->GetLevel() < 15)
                continue;

            PlayerbotAI* playerAi = player->GetPlayerbotAI();

            if (playerAi)
            {
                if (playerAi->GetGrouperType() == GrouperType::SOLO && !playerAi->HasRealPlayerMaster()) //Do not invite solo players. 
                    continue;

                if (playerAi->HasActivePlayerMaster()) //Do not invite alts of active players. 
                    continue;

                if (player->GetLevel() > bot->GetLevel() + 5) //Invite higher levels that need money so they can grind money and help out.
                {
                    if (!PAI_VALUE(bool, "should get money"))
                        continue;
                }
            }

            if (bot->GetLevel() > player->GetLevel() + 5) //Do not invite members that too low level or risk dragging them to deadly places.
                continue;

            if (!playerAi && sServerFacade.GetDistance2d(bot, player) > sPlayerbotAIConfig.sightDistance)
                continue;

            Group* group = bot->GetGroup();
            //When inviting the 5th member of the group convert to raid for future invites.
            if (group && ai->GetGrouperType() > GrouperType::LEADER_5 && !group->IsRaidGroup() && bot->GetGroup()->GetMembersCount() > 3)
            {
                group->ConvertToRaid();
            }

            if (sPlayerbotAIConfig.inviteChat && sRandomPlayerbotMgr.IsFreeBot(bot))
            {
                map<string, string> placeholders;
                placeholders["%name"] = player->GetName();
                placeholders["%place"] = WorldPosition(player).getAreaName(false, false);

                if (group && group->IsRaidGroup())
                {
                    if (urand(0, 3))
                        guild->BroadcastToGuild(bot->GetSession(), BOT_TEXT2("Hey anyone want to raid in %place", placeholders), LANG_UNIVERSAL);
                    else
                        guild->BroadcastToGuild(bot->GetSession(), BOT_TEXT2("Hey %name I'm raiding in %place do you wan to join me?", placeholders), LANG_UNIVERSAL);
                }
                else
                {
                    if (urand(0, 3))
                        guild->BroadcastToGuild(bot->GetSession(), BOT_TEXT2("Hey anyone wanna group up in %place?", placeholders), (bot->GetTeam() == ALLIANCE ? LANG_COMMON : LANG_ORCISH));
                    else
                        guild->BroadcastToGuild(bot->GetSession(), BOT_TEXT2("Hey %name do you want join my group? I'm heading for %place", placeholders), (bot->GetTeam() == ALLIANCE ? LANG_COMMON : LANG_ORCISH));
                }
            }

            return Invite(bot, player);
        }

        return false;
    }
}
