#pragma once

#include "../Action.h"

namespace ai
{
    class AcceptInvitationAction : public Action {
    public:
        AcceptInvitationAction(PlayerbotAI* ai) : Action(ai, "accept invitation") {}

        virtual bool Execute(Event event)
        {
            Player* master = GetMaster();

            Group* grp = bot->GetGroupInvite();
            if (!grp)
                return false;

            Player* inviter = sObjectMgr.GetPlayer(grp->GetLeaderGuid());
            if (!inviter)
                return false;

			if (!ai->GetSecurity()->CheckLevelFor(PLAYERBOT_SECURITY_INVITE, false, inviter))
            {
                WorldPacket data(SMSG_GROUP_DECLINE, 10);
                data << bot->GetName();
                sServerFacade.SendPacket(inviter, data);
                bot->UninviteFromGroup();
                return false;
            }

            WorldPacket p;
            uint32 roles_mask = 0;
            p << roles_mask;
            bot->GetSession()->HandleGroupAcceptOpcode(p);

            if (sRandomPlayerbotMgr.IsRandomBot(bot))
                ai->SetMaster(inviter);
            //else
            //    sPlayerbotDbStore.Save(ai);
            
            ai->ResetStrategies();
            ai->ChangeStrategy("+follow,-lfg,-bg", BOT_STATE_NON_COMBAT);
            if (!inviter->GetPlayerbotAI() && !bot->GetPlayerbotAI()->IsRealPlayer())
            {
                ai->ChangeStrategy("-rpg,-travel,-grind", BOT_STATE_NON_COMBAT);
            }
            ai->Reset();

            ai->TellMaster("Hello");
            return true;
        }
    };

}
