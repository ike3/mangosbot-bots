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
            else
                sPlayerbotDbStore.Save(ai);
            
            //if (!inviter->GetPlayerbotAI() && !bot->GetPlayerbotAI()->isRealPlayer())
            //{
                ai->ResetStrategies();
                //ai->ChangeStrategy("-rpg", BOT_STATE_NON_COMBAT);
                //ai->ChangeStrategy("-grind", BOT_STATE_NON_COMBAT);
                //ai->ChangeStrategy("-travel", BOT_STATE_NON_COMBAT);
                ai->ChangeStrategy("+follow", BOT_STATE_NON_COMBAT);
            //}

            ai->ChangeStrategy("-lfg", BOT_STATE_NON_COMBAT);
            ai->ChangeStrategy("-bg", BOT_STATE_NON_COMBAT);

            ai->Reset();

            ai->TellMaster("Hello");
            return true;
        }
    };

}
