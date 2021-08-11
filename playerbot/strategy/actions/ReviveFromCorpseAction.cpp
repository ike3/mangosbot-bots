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

    sLog.outDetail("Bot #%d %s:%d <%s> revives at body", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName());
    
    WorldPacket packet(CMSG_RECLAIM_CORPSE);
    packet << bot->GetObjectGuid();
    bot->GetSession()->HandleReclaimCorpseOpcode(packet);

    return true;
}

bool FindCorpseAction::Execute(Event event)
{
    if (bot->InBattleGround())
        return false;

    Corpse* corpse = bot->GetCorpse();
    if (!corpse)
        return false;

    //if (corpse->GetMapId() != bot->GetMapId())
    //    return false;

    Player* master = GetMaster();
    if (master)
    {
        if (!master->GetPlayerbotAI() &&
            sServerFacade.IsDistanceLessThan(AI_VALUE2(float, "distance", "master target"), sPlayerbotAIConfig.farDistance))
            return false;
    }

    uint32 dCount = AI_VALUE(uint32, "death count");

    if (!ai->HasRealPlayerMaster())
    {
        if (dCount >= 5)
        {
            sLog.outBasic("Bot #%d %s:%d <%s>: died too many times and was sent to an inn", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName());
            context->GetValue<uint32>("death count")->Set(0);
            sRandomPlayerbotMgr.RandomTeleportForRpg(bot);
            return true;
        }
    }

    if (!corpse->IsWithinDistInMap(bot, CORPSE_RECLAIM_RADIUS - 5.0f, true))
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
       
        if (!ai->AllowActivity(ALL_ACTIVITY))
        {
            uint32 delay = sServerFacade.GetDistance2d(bot, corpse) / bot->GetSpeed(MOVE_RUN); //Time a bot would take to travel to it's corpse.
            delay = min(delay, uint32(10 * MINUTE)); //Cap time to get to corpse at 10 minutes.

            if (deadTime > delay)
            {
                bot->GetMotionMaster()->Clear();
                bot->TeleportTo(corpse->GetMapId(), x, y, z, 0);
            }
        }
        else
        {
            if (bot->IsMoving())
                return false;

            bool moved = false;

            if (deadTime < 30 * MINUTE && dCount < 5 && corpse->GetMapId() == bot->GetMapId()) //Look for corpse up to 30 minutes.
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
                    sLog.outBasic("Bot #%d %s:%d <%s> moves to graveyard", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName());
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

WorldSafeLocsEntry const* SpiritHealerAction::GetGrave(bool startZone)
{
    WorldSafeLocsEntry const* ClosestGrave = nullptr;
    WorldSafeLocsEntry const* NewGrave = nullptr;

    ClosestGrave = sObjectMgr.GetClosestGraveYard(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(), bot->GetMapId(), bot->GetTeam());

    if (!startZone && ClosestGrave)
        return ClosestGrave;

    vector<uint32> races;

    if (bot->GetTeam() == ALLIANCE)
        races = { RACE_HUMAN, RACE_DWARF,RACE_GNOME,RACE_NIGHTELF };
    else
        races = { RACE_ORC, RACE_TROLL,RACE_TAUREN,RACE_UNDEAD };

    float graveDistance = -1;

    WorldPosition botPos(bot);

    for (auto race : races)
    {
        for (uint32 cls = 0; cls < MAX_CLASSES; cls++)
        {
            PlayerInfo const* info = sObjectMgr.GetPlayerInfo(race, cls);

            if (!info)
                continue;

            NewGrave = sObjectMgr.GetClosestGraveYard(info->positionX, info->positionY, info->positionZ, info->mapId, bot->GetTeam());

            if (!NewGrave)
                continue;

            WorldPosition gravePos(NewGrave->map_id, NewGrave->x, NewGrave->y, NewGrave->z);

            float newDist = botPos.fDist(gravePos);;

            if (graveDistance < 0 || newDist < graveDistance)
            {
                ClosestGrave = NewGrave;
                graveDistance = newDist;
            }
        }
    }

    return ClosestGrave;
}

bool SpiritHealerAction::Execute(Event event)
{
    Corpse* corpse = bot->GetCorpse();
    if (!corpse)
    {
        ai->TellError("I am not a spirit");
        return false;
    }

    uint32 dCount = AI_VALUE(uint32, "death count");

    WorldSafeLocsEntry const* ClosestGrave = GetGrave(dCount > 10);

    if (bot->GetDistance2d(ClosestGrave->x, ClosestGrave->y) < sPlayerbotAIConfig.sightDistance)
    {
        list<ObjectGuid> npcs = AI_VALUE(list<ObjectGuid>, "nearest npcs");
        for (list<ObjectGuid>::iterator i = npcs.begin(); i != npcs.end(); i++)
        {
            Unit* unit = ai->GetUnit(*i);
            if (unit && unit->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPIRITHEALER))
            {
                sLog.outBasic("Bot #%d %s:%d <%s> revives at spirit healer", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName());
                PlayerbotChatHandler ch(bot);
                bot->ResurrectPlayer(0.5f);
                bot->SpawnCorpseBones();
                bot->SaveToDB();
                context->GetValue<Unit*>("current target")->Set(NULL);
                bot->SetSelectionGuid(ObjectGuid());
                ai->TellMaster("Hello");

                if (dCount > 20)
                    context->GetValue<uint32>("death count")->Set(0);

                return true;
            }
        }
    }

    if (!ClosestGrave)
    {
        return false;
    }

    bool moved = false;
    
    if (bot->IsWithinLOS(ClosestGrave->x, ClosestGrave->y, ClosestGrave->z))
        moved = MoveNear(ClosestGrave->map_id, ClosestGrave->x, ClosestGrave->y, ClosestGrave->z, 0.0);
    else
        moved = MoveTo(ClosestGrave->map_id, ClosestGrave->x, ClosestGrave->y, ClosestGrave->z, false, false);

    if (moved)
        return true;

    if (!ai->HasActivePlayerMaster())
    {
        bot->TeleportTo(ClosestGrave->map_id, ClosestGrave->x, ClosestGrave->y, ClosestGrave->z, ClosestGrave->o);
    }

    sLog.outBasic("Bot #%d %s:%d <%s> can't find a spirit healer", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName());
    ai->TellError("Cannot find any spirit healer nearby");
    return false;
}