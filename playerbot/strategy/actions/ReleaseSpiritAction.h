#pragma once

#include "../../ServerFacade.h"
#include "../Action.h"
#include "MovementActions.h"
#include "../values/LastMovementValue.h"
#include "ReviveFromCorpseAction.h"

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

            sLog.outDetail("Bot #%d %s:%d <%s> released", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->GetLevel(), bot->GetName());

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
            sLog.outDetail("Bot #%d %s:%d <%s> auto released", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->GetLevel(), bot->GetName());

            WorldPacket packet(CMSG_REPOP_REQUEST);
            packet << uint8(0);
            bot->GetSession()->HandleRepopRequestOpcode(packet);

            // add waiting for ress aura
            if (bot->InBattleGround() && !ai->HasAura(2584, bot))
            {
                // cast Waiting for Resurrect
                bot->CastSpell(bot, 2584, TRIGGERED_OLD_TRIGGERED);
            }

            sTravelMgr.logEvent(ai, "AutoReleaseSpiritAction");           

            return true;
        }

        virtual bool isUseful()
        {
            if (!sServerFacade.UnitIsDead(bot))
                return false;

#ifndef MANGOSBOT_ZERO
            if (bot->InArena())
                return false;
#endif

            if (bot->InBattleGround())
                return !bot->GetCorpse() || !bot->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST);

            if (bot->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
                return false;

            if (!bot->GetGroup())
                return true;

            if (!ai->GetGroupMaster())
                return true;

            if (ai->GetGroupMaster() == bot)
                return true;

            if (!ai->HasActivePlayerMaster())
                return true;

            if (ai->HasActivePlayerMaster() && ai->GetGroupMaster()->GetMapId() == bot->GetMapId() && bot->GetMap() && (bot->GetMap()->IsRaid() || bot->GetMap()->IsDungeon()))
                return false;

            if(sServerFacade.UnitIsDead(ai->GetGroupMaster()))
                return true;

            if (sServerFacade.IsDistanceGreaterThan(AI_VALUE2(float, "distance", "master target"), sPlayerbotAIConfig.sightDistance))
                return true;

            return false;
        }
    };

    class RepopAction : public SpiritHealerAction {
    public:
        RepopAction(PlayerbotAI* ai, string name = "repop") : SpiritHealerAction(ai, name) {}

    public:
        virtual bool Execute(Event event)
        {
            sLog.outBasic("Bot #%d %s:%d <%s> repops at graveyard", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->GetLevel(), bot->GetName());

            int64 deadTime;

            Corpse* corpse = bot->GetCorpse();
            if (corpse)
                deadTime = time(nullptr) - corpse->GetGhostTime();
            else if (bot->IsDead())
                deadTime = 0;
            else
                deadTime = 60 * MINUTE;

            uint32 dCount = AI_VALUE(uint32, "death count");

            WorldSafeLocsEntry const* ClosestGrave = GetGrave(dCount > 10 || deadTime > 30 * MINUTE);

            if (!ClosestGrave)
                return false;

            bot->TeleportTo(ClosestGrave->map_id, ClosestGrave->x, ClosestGrave->y, ClosestGrave->z, ClosestGrave->o);

            RESET_AI_VALUE(bool,"combat::self target");
            RESET_AI_VALUE(WorldPosition, "current position");

            return true;
        }

        virtual bool isUseful()
        {
            if (bot->InBattleGround())
                return false;

            return true;
        }
    };
}
