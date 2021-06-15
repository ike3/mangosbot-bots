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

            if (bot->GetCorpse() && bot->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
            {
                ai->TellMasterNoFacing("I am already a spirit");
                return false;
            }

            WorldPacket& p = event.getPacket();
            if (!p.empty() && p.GetOpcode() == CMSG_REPOP_REQUEST)
                ai->TellMasterNoFacing("Releasing...");
            else
                ai->TellMasterNoFacing("Meet me at the graveyard");

            sLog.outDetail("Bot #%d %s:%d <%s> released", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName());

            WorldPacket packet(CMSG_REPOP_REQUEST);
            packet << uint8(0);
            bot->GetSession()->HandleRepopRequestOpcode(packet);

            // add waiting for ress aura
            if (bot->InBattleGround() && !ai->HasAura(2584, bot))
            {
                // cast Waiting for Resurrect
                bot->CastSpell(bot, 2584, TRIGGERED_OLD_TRIGGERED);
            }

            return true;
        }
    };

    class AutoReleaseSpiritAction : public ReleaseSpiritAction {
    public:
        AutoReleaseSpiritAction(PlayerbotAI* ai, string name = "auto release") : ReleaseSpiritAction(ai, name) {}

        virtual bool Execute(Event event)
        {
            sLog.outDetail("Bot #%d %s:%d <%s> auto released", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName());

            WorldPacket packet(CMSG_REPOP_REQUEST);
            packet << uint8(0);
            bot->GetSession()->HandleRepopRequestOpcode(packet);

            // add waiting for ress aura
            if (bot->InBattleGround() && !ai->HasAura(2584, bot))
            {
                // cast Waiting for Resurrect
                bot->CastSpell(bot, 2584, TRIGGERED_OLD_TRIGGERED);
            }

            return true;
        }

        virtual bool isUseful()
        {
            if (bot->InBattleGround())
                return sServerFacade.UnitIsDead(bot) && !bot->GetCorpse();

            return ((!bot->GetGroup()) || (bot->GetGroup() && ai->GetGroupMaster() == bot) || (ai->GetGroupMaster() && ai->GetGroupMaster() != bot &&
                sServerFacade.UnitIsDead(ai->GetGroupMaster()) &&
                bot->GetDeathState() != ai->GetGroupMaster()->GetDeathState()))
                && sServerFacade.UnitIsDead(bot) && !bot->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST);
        }
    };

    class RepopAction : public Action {
    public:
        RepopAction(PlayerbotAI* ai, string name = "repop") : Action(ai, name) {}

    public:
        virtual bool Execute(Event event)
        {
            sLog.outBasic("Bot #%d %s:%d <%s> repops at graveyard", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName());

            bot->RepopAtGraveyard();

            return true;
        }

        virtual bool isUsefull()
        {
            return true;
        }
    };
}
