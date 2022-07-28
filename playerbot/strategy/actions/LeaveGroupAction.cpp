#pragma once

#include "botpch.h"
#include "../../playerbot.h"
#include "LeaveGroupAction.h"


using namespace ai;

namespace ai
{
	bool LeaveGroupAction::Leave(Player* player) {

        if (!player)
            return false;

        Group* group = bot->GetGroup();

        /*if (!player->GetPlayerbotAI() && !ai->GetSecurity()->CheckLevelFor(PLAYERBOT_SECURITY_INVITE, false, player))
            return false;*/

        bool aiMaster = (ai->GetMaster() && ai->GetMaster()->GetPlayerbotAI());

        ai->TellMaster(BOT_TEXT("goodbye"), PLAYERBOT_SECURITY_TALK);

        bool randomBot = sRandomPlayerbotMgr.IsRandomBot(bot);

        bool shouldStay = randomBot && bot->GetGroup() && player == bot;

        if (!shouldStay)
        {
            WorldPacket p;
            string member = bot->GetName();
            p << uint32(PARTY_OP_LEAVE) << member << uint32(0);
            bot->GetSession()->HandleGroupDisbandOpcode(p);
            if (ai->GetMaster() && ai->GetMaster()->GetObjectGuid() != player->GetObjectGuid())
                bot->Whisper("I left my group", LANG_UNIVERSAL, player->GetObjectGuid());
        }

        if (randomBot)
        {
            bot->GetPlayerbotAI()->SetMaster(NULL);
        }        

        if(!aiMaster)
            ai->ResetStrategies(!randomBot);
        ai->Reset();

        if(group)
            sTravelMgr.logEvent(ai, "LeaveGroupAction", group->GetLeaderName(), to_string(group->GetMembersCount()));

        return true;
	}

    bool LeaveFarAwayAction::isUseful()
    {
        if (!sPlayerbotAIConfig.randomBotGroupNearby)
            return false;

        if (bot->InBattleGround())
            return false;

        if (bot->InBattleGroundQueue())
            return false;

        if (!bot->GetGroup())
            return false;
        
        Player* master = ai->GetGroupMaster();
        Player* trueMaster = ai->GetMaster();

        if (!master || !trueMaster || (bot == master && !ai->IsRealPlayer()))
            return false;

        if (master && !master->GetPlayerbotAI())
           return false;

        if (trueMaster && !trueMaster->GetPlayerbotAI()) 
            return false;

        if (ai->IsAlt() && (!master->GetPlayerbotAI() || master->GetPlayerbotAI()->IsRealPlayer())) //Don't leave group when alt grouped with player master.
            return false;

        if (ai->GetGrouperType() == GrouperType::SOLO)
            return true;

        uint32 dCount = AI_VALUE(uint32, "death count");

        if (dCount > 9)
            return true;

        if (dCount > 4 && !ai->HasRealPlayerMaster())
            return true;

        if (bot->GetGuildId() == master->GetGuildId())
        {
            if (bot->GetLevel() > master->GetLevel() + 5)
            {
                if(AI_VALUE(bool, "should get money"))
                    return false;
            }
        }

        if (abs(int32(master->GetLevel() - bot->GetLevel())) > 4)
            return true;

        return false;
    }
}