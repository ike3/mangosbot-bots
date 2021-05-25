#include "botpch.h"
#include "../../playerbot.h"
#include "ReviveFromCorpseAction.h"
#include "../../PlayerbotFactory.h"
#include "../../PlayerbotAIConfig.h"

using namespace ai;

bool ReviveFromCorpseAction::Execute(Event event)
{
    Player* master = GetMaster();
    Corpse* corpse = bot->GetCorpse();

    // follow master when master revives
    WorldPacket& p = event.getPacket();
    if (!p.empty() && p.GetOpcode() == CMSG_RECLAIM_CORPSE && master && !corpse && sServerFacade.IsAlive(bot))
    {
        if (sServerFacade.IsDistanceLessThan(AI_VALUE2(float, "distance", "master target"), sPlayerbotAIConfig.farDistance))
        {
            if (!ai->HasStrategy("follow", BOT_STATE_NON_COMBAT))
            {
                ai->TellMasterNoFacing("Welcome back!");
                ai->ChangeStrategy("+follow,-stay", BOT_STATE_NON_COMBAT);
                return true;
            }
        }
    }

    if (!corpse)
        return false;

    if (corpse->GetGhostTime() + bot->GetCorpseReclaimDelay(corpse->GetType() == CORPSE_RESURRECTABLE_PVP) > time(nullptr))
        return false;

    if (master)
    {
        if (!master->GetPlayerbotAI() && sServerFacade.UnitIsDead(master) && master->GetCorpse()
            && sServerFacade.IsDistanceLessThan(AI_VALUE2(float, "distance", "master target"), sPlayerbotAIConfig.farDistance))
            return false;
    }
    
    WorldPacket packet(CMSG_RECLAIM_CORPSE);
    packet << bot->GetObjectGuid();
    bot->GetSession()->HandleReclaimCorpseOpcode(packet);

    sLog.outDetail("Bot #%d %s:%d <%s> revived", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName());

    context->GetValue<Unit*>("current target")->Set(NULL);
    bot->SetSelectionGuid(ObjectGuid());
    ai->ChangeEngine(BOT_STATE_NON_COMBAT);
    return true;
}

bool FindCorpseAction::Execute(Event event)
{
    if (bot->InBattleGround())
        return false;

    Corpse* corpse = bot->GetCorpse();
    if (!corpse)
        return false;

    if (corpse->GetMapId() != bot->GetMapId())
        return false;

    Player* master = GetMaster();
    if (master)
    {
        if (!master->GetPlayerbotAI() &&
            sServerFacade.IsDistanceLessThan(AI_VALUE2(float, "distance", "master target"), sPlayerbotAIConfig.farDistance))
            return false;
    }

    if (!corpse->IsWithinDistInMap(bot, CORPSE_RECLAIM_RADIUS - 5, true))
    {
        float x = corpse->GetPositionX();
        float y = corpse->GetPositionY();
        float z = corpse->GetPositionZ();


#ifndef MANGOSBOT_TWO         
        bot->GetMap()->GetReachableRandomPointOnGround(x, y, z, CORPSE_RECLAIM_RADIUS - 5.0f, true);
#else
        bot->GetMap()->GetReachableRandomPointOnGround(bot->GetPhaseMask(), x, y, z, CORPSE_RECLAIM_RADIUS - 5.0f, true);
#endif

        int64 deadTime = time(nullptr) - corpse->GetGhostTime();

        sLog.outDetail("Bot #%d %s:%d <%s> looks for body", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName());
       
        if (!ai->AllowActive(ALL_ACTIVITY))
        {
            uint32 delay = bot->GetDistance(corpse) / bot->GetSpeed(MOVE_RUN) * IN_MILLISECONDS + sPlayerbotAIConfig.reactDelay; //Time a bot would take to travel to it's corpse.
            delay = std::min(delay, uint32(10 * MINUTE * IN_MILLISECONDS)); //Cap time to get to corpse at 10 minutes.

            if (deadTime > delay)
            {
                bot->GetMotionMaster()->Clear();
                bot->TeleportTo(corpse->GetMapId(), x, y, z, 0);
            }
        }
        else
        {
            bool moved = false;

            if (deadTime < 30 * MINUTE * IN_MILLISECONDS) //Look for corpse up to 30 minutes.
            {
                if (bot->IsWithinLOS(x, y, z))
                    moved = MoveNear(bot->GetMapId(), x, y, z, 0);
                else
                    moved = MoveTo(bot->GetMapId(), x, y, z, false, false);
            }

            if (!moved)
            {
                moved = ai->DoSpecificAction("spirit healer");
                if (moved)
                    sLog.outDetail("Bot #%d %s:%d <%s> revived at graveyard", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName());
            }

            return moved;
        }
        return false;
    }
    return false;
}

bool FindCorpseAction::isUseful()
{
    if (bot->InBattleGround())
        return false;

    return bot->GetCorpse() && !bot->GetCorpse()->IsWithinDistInMap(bot, CORPSE_RECLAIM_RADIUS - 5, true);
}

bool SpiritHealerAction::Execute(Event event)
{
    Corpse* corpse = bot->GetCorpse();
    if (!corpse)
    {
        ai->TellError("I am not a spirit");
        return false;
    }

    list<ObjectGuid> npcs = AI_VALUE(list<ObjectGuid>, "nearest npcs");
    for (list<ObjectGuid>::iterator i = npcs.begin(); i != npcs.end(); i++)
    {
        Unit* unit = ai->GetUnit(*i);
        if (unit && unit->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPIRITHEALER))
        {
            PlayerbotChatHandler ch(bot);
            bot->ResurrectPlayer(0.5f);
            bot->SpawnCorpseBones();
            bot->SaveToDB();
            context->GetValue<Unit*>("current target")->Set(NULL);
            bot->SetSelectionGuid(ObjectGuid());
            ai->TellMaster("Hello");
            return true;
        }
    }

    AreaTableEntry const* zone = GetAreaEntryByAreaID(bot->GetAreaId());

    WorldSafeLocsEntry const* ClosestGrave = nullptr;
    ClosestGrave = sObjectMgr.GetClosestGraveYard(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(), bot->GetMapId(), bot->GetTeam());

    if (!ClosestGrave)
    {
        return false;
    }

    float x = ClosestGrave->x;
    float y = ClosestGrave->y;
    float z = ClosestGrave->z;

    bool moved = false;

    if (bot->IsWithinLOS(x, y, z))
        moved = MoveNear(bot->GetMapId(), x, y, z, 0);
    else
        moved = MoveTo(bot->GetMapId(), x, y, z, false, false);

    if (moved)
        return true;

    ai->TellError("Cannot find any spirit healer nearby");
    return false;
}