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

        if (!player->GetPlayerbotAI() && !ai->GetSecurity()->CheckLevelFor(PLAYERBOT_SECURITY_INVITE, false, player))
            return false;

        WorldPacket p;
        uint32 roles_mask = 0;
        p << player->GetName();
        p << roles_mask;
        bot->GetSession()->HandleGroupInviteOpcode(p);

        return true;
    }

    bool InviteNearbyToGroupAction::Execute(Event event)
    {
        list<ObjectGuid> nearGuids = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest friendly players")->Get();
        for (auto& i : nearGuids)
        {
            Player* player = sObjectMgr.GetPlayer(i);

            if (!player)
                continue;

            if (player->GetGroup())
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

            if (abs(int32(player->getLevel() - bot->getLevel())) > 2)
                continue;

            if (sServerFacade.GetDistance2d(bot, player) > sPlayerbotAIConfig.sightDistance)
                continue;

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

    bool InviteGuildToGroupAction::Execute(Event event)
    {
        for (auto& member : getGuildMembers())
        {
            Player* player = member;

            if (!player)
                continue;

            if (player->GetGroup())
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

            if (player->getLevel() + 2 < bot->getLevel())
                continue;

            if (player->getLevel() > bot->getLevel() + 20)
                continue;

            return Invite(player);
        }

        return false;
    }
}
