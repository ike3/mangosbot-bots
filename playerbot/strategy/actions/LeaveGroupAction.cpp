#pragma once

#include "botpch.h"
#include "../../playerbot.h"
#include "LeaveGroupAction.h"


using namespace ai;

namespace ai
{
	bool LeaveGroupAction::Leave() {

        bool aiMaster = (ai->GetMaster() && ai->GetMaster()->GetPlayerbotAI());

        ai->TellMaster("Goodbye!", PLAYERBOT_SECURITY_TALK);

        WorldPacket p;
        string member = bot->GetName();
        p << uint32(PARTY_OP_LEAVE) << member << uint32(0);
        bot->GetSession()->HandleGroupDisbandOpcode(p);

        bool randomBot = sRandomPlayerbotMgr.IsRandomBot(bot);
        if (randomBot)
        {
            bot->GetPlayerbotAI()->SetMaster(NULL);
            sRandomPlayerbotMgr.ScheduleTeleport(bot->GetObjectGuid());
        }        

        if(!aiMaster)
            ai->ResetStrategies(!randomBot);
        ai->Reset();

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

        if (ai->GetGrouperType() == SOLO)
            return true;

        if (abs(int32(master->getLevel() - bot->getLevel())) > 4)
            return true;

        if (master->GetDistance(bot) > sPlayerbotAIConfig.reactDistance * 4)
            return true;

        return false;
    }
}