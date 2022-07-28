#include "botpch.h"
#include "../../playerbot.h"
#include "ReviveFromCorpseAction.h"
#include "../../PlayerbotFactory.h"
#include "../../PlayerbotAIConfig.h"
#include "../../FleeManager.h"

using namespace ai;

bool ReviveFromCorpseAction::Execute(Event event)
{
    Player* master = ai->GetGroupMaster();
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

    if (!ai->HasRealPlayerMaster())
    {
        uint32 dCount = AI_VALUE(uint32, "death count");

        if (dCount >= 5)
        {
            return ai->DoSpecificAction("spirit healer", Event(), true);
        }
    }

    sLog.outDetail("Bot #%d %s:%d <%s> revives at body", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->GetLevel(), bot->GetName());

    ai->StopMoving();
    WorldPacket packet(CMSG_RECLAIM_CORPSE);
    packet << bot->GetObjectGuid();
    bot->GetSession()->HandleReclaimCorpseOpcode(packet);

    sTravelMgr.logEvent(ai, "ReviveFromCorpseAction");
   
    return true;
}

bool FindCorpseAction::Execute(Event event)
{
    if (bot->InBattleGround())
        return false;

    Corpse* corpse = bot->GetCorpse();
    if (!corpse)
        return false;

    Player* master = ai->GetGroupMaster();
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
            sLog.outBasic("Bot #%d %s:%d <%s>: died too many times and was sent to an inn", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->GetLevel(), bot->GetName());
            context->GetValue<uint32>("death count")->Set(0);
            sRandomPlayerbotMgr.RandomTeleportForRpg(bot, false);
            return true;
        }
    }

    WorldPosition botPos(bot),corpsePos(corpse), moveToPos = corpsePos, masterPos(master);
    float reclaimDist = CORPSE_RECLAIM_RADIUS - 5.0f;
    float corpseDist = botPos.distance(corpsePos);
    int64 deadTime = time(nullptr) - corpse->GetGhostTime();

    bool moveToMaster = master && master != bot && masterPos.fDist(corpsePos) < reclaimDist;

    //Should we ressurect? If so, return false.
    if (corpseDist < reclaimDist)
    {
        if (moveToMaster) //We are near master.
        {
            if (botPos.fDist(masterPos) < sPlayerbotAIConfig.spellDistance)
                return false;
        }
        else if (deadTime > 8 * MINUTE) //We have walked too long already.
            return false;
        else 
        {
            list<ObjectGuid> units = AI_VALUE(list<ObjectGuid>, "possible targets no los");
            
            if (botPos.getUnitsAggro(units, bot) == 0) //There are no mobs near.
                return false;
        }
    }

    //If we are getting close move to a save ressurrection spot instead of just the corpse.
    if (corpseDist < sPlayerbotAIConfig.reactDistance)
    {
        if (moveToMaster)
            moveToPos = masterPos;
        else
        {
            FleeManager manager(bot, reclaimDist, 0.0, urand(0, 1), moveToPos);

            if (manager.isUseful())
            {
                float rx, ry, rz;
                if (manager.CalculateDestination(&rx, &ry, &rz))
                    moveToPos = WorldPosition(moveToPos.getMapId(), rx, ry, rz, 0.0);
                else if (!moveToPos.GetReachableRandomPointOnGround(bot, reclaimDist, urand(0, 1)))
                    moveToPos = corpsePos;
            }
        }
    }

    //Actual mobing part.
    bool moved = false;

    if (!ai->AllowActivity(ALL_ACTIVITY))
    {
        uint32 delay = sServerFacade.GetDistance2d(bot, corpse) / bot->GetSpeed(MOVE_RUN); //Time a bot would take to travel to it's corpse.
        delay = min(delay, uint32(10 * MINUTE)); //Cap time to get to corpse at 10 minutes.

        if (deadTime > delay)
        {
            bot->GetMotionMaster()->Clear();
            bot->TeleportTo(moveToPos.getMapId(), moveToPos.getX(), moveToPos.getY(), moveToPos.getZ(), 0);
        }

        moved = true;
    }
    else
    {
        if (bot->IsMoving())
            moved = true;
        else
        {
            if (deadTime < 10 * MINUTE && dCount < 5) //Look for corpse up to 30 minutes.
            {
                moved = MoveTo(moveToPos.getMapId(), moveToPos.getX(), moveToPos.getY(), moveToPos.getZ(), false, false);
            }

            if (!moved)
            {
                moved = ai->DoSpecificAction("spirit healer", Event(), true);
            }
        }
    }   

    return moved;
}

bool FindCorpseAction::isUseful()
{
    if (bot->InBattleGround())
        return false;

    return bot->GetCorpse();
}

WorldSafeLocsEntry const* SpiritHealerAction::GetGrave(bool startZone)
{
    WorldSafeLocsEntry const* ClosestGrave = nullptr;
    WorldSafeLocsEntry const* NewGrave = nullptr;

    ClosestGrave = bot->GetMap()->GetGraveyardManager().GetClosestGraveYard(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(), bot->GetMapId(), bot->GetTeam());

    if (!startZone && ClosestGrave)
        return ClosestGrave;

    if (ai->HasStrategy("follow", BOT_STATE_NON_COMBAT)&& ai->GetGroupMaster() && ai->GetGroupMaster() != bot)
    {
        Player* master = ai->GetGroupMaster();

        if (master && master != bot)
        {
            ClosestGrave = bot->GetMap()->GetGraveyardManager().GetClosestGraveYard(master->GetPositionX(), master->GetPositionY(), master->GetPositionZ(), master->GetMapId(), bot->GetTeam());

            if (ClosestGrave)
                return ClosestGrave;
        }
    }
    else if(startZone && AI_VALUE(uint8, "durability"))
    {
        TravelTarget* travelTarget = AI_VALUE(TravelTarget*, "travel target");

        if (travelTarget->getPosition())
        {
            WorldPosition travelPos = *travelTarget->getPosition();
            ClosestGrave = bot->GetMap()->GetGraveyardManager().GetClosestGraveYard(travelPos.getX(), travelPos.getY(), travelPos.getZ(), travelPos.getMapId(), bot->GetTeam());

            if (ClosestGrave)
                return ClosestGrave;
        }
    }


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

            NewGrave = bot->GetMap()->GetGraveyardManager().GetClosestGraveYard(info->positionX, info->positionY, info->positionZ, info->mapId, bot->GetTeam());

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
    int64 deadTime = time(nullptr) - corpse->GetGhostTime();

    WorldSafeLocsEntry const* ClosestGrave = GetGrave(dCount > 10 || deadTime > 15 * MINUTE || AI_VALUE(uint8, "durability") < 10);

    if (bot->GetDistance2d(ClosestGrave->x, ClosestGrave->y) < sPlayerbotAIConfig.sightDistance)
    {
        list<ObjectGuid> npcs = AI_VALUE(list<ObjectGuid>, "nearest npcs");
        for (list<ObjectGuid>::iterator i = npcs.begin(); i != npcs.end(); i++)
        {
            Unit* unit = ai->GetUnit(*i);
            if (unit && unit->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPIRITHEALER))
            {
                sLog.outBasic("Bot #%d %s:%d <%s> revives at spirit healer", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->GetLevel(), bot->GetName());
                PlayerbotChatHandler ch(bot);
                bot->ResurrectPlayer(0.5f);
                bot->SpawnCorpseBones();
                bot->SaveToDB();
                context->GetValue<Unit*>("current target")->Set(NULL);
                bot->SetSelectionGuid(ObjectGuid());
                ai->TellMaster(BOT_TEXT("hello"));

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
        context->GetValue<uint32>("death count")->Set(dCount + 1);
        return bot->TeleportTo(ClosestGrave->map_id, ClosestGrave->x, ClosestGrave->y, ClosestGrave->z, ClosestGrave->o);
    }

    sLog.outBasic("Bot #%d %s:%d <%s> can't find a spirit healer", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->GetLevel(), bot->GetName());
    ai->TellError("Cannot find any spirit healer nearby");
    return false;
}

bool SpiritHealerAction::isUseful()
{
    return bot->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST);
}
