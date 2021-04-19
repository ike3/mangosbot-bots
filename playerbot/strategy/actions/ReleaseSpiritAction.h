#pragma once

#include "../../ServerFacade.h"
#include "../Action.h"
#include "MovementActions.h"
#include "../values/LastMovementValue.h"

namespace ai
{
	class ReleaseSpiritAction : public Action {
	public:
		ReleaseSpiritAction(PlayerbotAI* ai, string name = "release") : Action(ai, name) {}

    public:
        virtual bool Execute(Event event)
        {
            if (sServerFacade.IsAlive(bot))
            {
                ai->TellMasterNoFacing("I am not dead, will wait here");
                ai->ChangeStrategy("-follow,+stay", BOT_STATE_NON_COMBAT);
                return false;
            }

            if (bot->GetCorpse())
            {
                ai->TellMasterNoFacing("I am already a spirit");
                return false;
            }

            WorldPacket& p = event.getPacket();
            if (!p.empty() && p.GetOpcode() == CMSG_REPOP_REQUEST)
                ai->TellMasterNoFacing("Releasing...");
            else
                ai->TellMasterNoFacing("Meet me at the graveyard");

            WorldPacket packet(CMSG_REPOP_REQUEST);
            packet << uint8(0);
            bot->GetSession()->HandleRepopRequestOpcode(packet);
            sLog.outDetail("Bot #%d %s:%d <%s> released", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName());
            return true;
        }
    };

    class AutoReleaseSpiritAction : public ReleaseSpiritAction {
    public:
        AutoReleaseSpiritAction(PlayerbotAI* ai, string name = "auto release") : ReleaseSpiritAction(ai, name) {}

        virtual bool Execute(Event event)
        {
            WorldPacket packet(CMSG_REPOP_REQUEST);
            packet << uint8(0);
            bot->GetSession()->HandleRepopRequestOpcode(packet);
            sLog.outDetail("Bot #%d %s:%d <%s> auto released", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName());
            
            // add waiting for ress aura
            if (bot->InBattleGround())
                bot->CastSpell(bot, 2584, TRIGGERED_OLD_TRIGGERED);

            return true;
        }

        virtual bool isUseful()
        {
            if (bot->InBattleGround())
                return sServerFacade.UnitIsDead(bot) && !bot->GetCorpse();

            return ((!bot->GetGroup()) || (bot->GetGroup() && ai->GetGroupMaster() == bot) || (ai->GetGroupMaster() && ai->GetGroupMaster() != bot &&
                sServerFacade.UnitIsDead(ai->GetGroupMaster()) &&
                bot->GetDeathState() != ai->GetGroupMaster()->GetDeathState()))
                && sServerFacade.UnitIsDead(bot) && !bot->GetCorpse();
        }
    };
}
