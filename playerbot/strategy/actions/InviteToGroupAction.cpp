#pragma once

#include "botpch.h"
#include "../../playerbot.h"
#include "InviteToGroupAction.h"
#include "../../ServerFacade.h"


using namespace ai;

namespace ai
{
    bool InviteToGroupAction::Invite(Player* player)
    {
        if (!player)
            return false;

        if (!player->GetPlayerbotAI() && !ai->GetSecurity()->CheckLevelFor(PlayerbotSecurityLevel::PLAYERBOT_SECURITY_INVITE, true, player))
            return false;

        WorldPacket p;
        uint32 roles_mask = 0;
        p << player->GetName();
        p << roles_mask;
        bot->GetSession()->HandleGroupInviteOpcode(p);

        return true;
    }

    bool InviteNearbyToGroupAction::Execute(Event& event)
    {
        list<ObjectGuid> nearGuids = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest friendly players")->Get();
        for (auto& i : nearGuids)
        {
            Player* player = sObjectMgr.GetPlayer(i);

            if (!player)
                continue;

            if (player->GetGroup())
                continue;

            Group* group = bot->GetGroup();

            if (player->isDND())
                continue;

            PlayerbotAI* botAi = player->GetPlayerbotAI();

            if (botAi)
            {
                if (botAi->GetGrouperType() == GrouperType::SOLO && !botAi->HasRealPlayerMaster()) //Do not invite solo players. 
                    continue;

                if (botAi->HasActivePlayerMaster()) //Do not invite alts of active players. 
                    continue;
            }
            else
            {
                if (!sPlayerbotAIConfig.randomBotGroupNearby)
                    return false;
            }

            if (abs(int32(player->GetLevel() - bot->GetLevel())) > 2)
                continue;

            if (sServerFacade.GetDistance2d(bot, player) > sPlayerbotAIConfig.sightDistance)
                continue;
            
            //When inviting the 5th member of the group convert to raid for future invites.
            if (group && botAi->GetGrouperType() > GrouperType::LEADER_5 && !group->IsRaidGroup() && bot->GetGroup()->GetMembersCount() > 3)
                bot->GetGroup()->ConvertToRaid();

            return Invite(player);
        }

        return false;
    }

    bool InviteNearbyToGroupAction::isUseful()
    {
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
            if (group->IsFull())
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
        for (auto& member : getGuildMembers())
        {
            Player* player = member;

            if (!player)
                continue;

            if (player->GetGroup())
                continue;

            if (player->isDND())
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
                    if (!PAI_VALUE(bool,"should get money"))
                        continue;
                }
            }
            else
            {
                if (!sPlayerbotAIConfig.randomBotGroupNearby)
                    return false;
            }

            if (bot->GetLevel() > player->GetLevel() + 5) //Do not invite members that too low level or risk dragging them to deadly places.
                continue;

            if (!playerAi && sServerFacade.GetDistance2d(bot, player) > sPlayerbotAIConfig.sightDistance)
                continue;

            return Invite(player);
        }

        return false;
    }
}
