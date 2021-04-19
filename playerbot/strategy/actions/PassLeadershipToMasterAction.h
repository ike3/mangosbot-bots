#pragma once

#include "../Action.h"

namespace ai
{
    class PassLeadershipToMasterAction : public Action {
    public:
        PassLeadershipToMasterAction(PlayerbotAI* ai) : Action(ai, "leader") {}

        virtual bool Execute(Event event)
        {
            Player* master = GetMaster();
            if (master && !master->GetPlayerbotAI() && bot->GetGroup() && bot->GetGroup()->IsMember(master->GetObjectGuid()))
            {
                WorldPacket p(SMSG_GROUP_SET_LEADER, 8);
                p << master->GetObjectGuid();
                bot->GetSession()->HandleGroupSetLeaderOpcode(p);
                ai->TellMasterNoFacing("Passing leader to you!");
                return true;
            }

            return false;
        }

        virtual bool isUseful()
        {
            return bot->GetGroup() && bot->GetGroup()->IsLeader(bot->GetObjectGuid()) && !bot->GetPlayerbotAI()->isRealPlayer();
        }
    };

}
