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

            if (!botAi) //Only invite bots. Maybe change later.
                continue;

            if (botAi->GetGrouperType() == SOLO)
                return false;

            if (botAi->GetMaster())
                if (!botAi->GetMaster()->GetPlayerbotAI() || botAi->GetMaster()->GetPlayerbotAI()->IsRealPlayer()) //Do not invite bots with a player master.
                    if (!botAi->IsRealPlayer()) //Unless the bot is really a player
                        continue;

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
        if (!sPlayerbotAIConfig.randomBotGroupNearby)
            return false;

        if (bot->InBattleGround())
            return false;
        
        if (bot->InBattleGroundQueue())
            return false;

        GrouperType grouperType = ai->GetGrouperType();

        if (grouperType == SOLO || grouperType == MEMBER)
            return false;

        Group* group = bot->GetGroup();

        if (group)
        {
            if (group->IsFull())
                return false;

            if (ai->GetGroupMaster() != bot)
                return false;

            uint32 memberCount = group->GetMembersCount();

            if (memberCount > 1 && grouperType == LEADER_2)
                return false;

            if (memberCount > 2 && grouperType == LEADER_3)
                return false;

            if (memberCount > 3 && grouperType == LEADER_4)
                return false;
        }

        if (ai->GetMaster())
            if (!ai->GetMaster()->GetPlayerbotAI() || ai->GetMaster()->GetPlayerbotAI()->IsRealPlayer()) //Alts do not invite.
                if (!ai->IsRealPlayer()) //Unless the bot is really a player
                    return false;

        return true;
    }
}
