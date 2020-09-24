#include "ObjectGuid.h"
#include "botpch.h"
#include "../../playerbot.h"
#include "../../playerbotAI.h"
#include "LfgActions.h"
#include "../../AiFactory.h"
//#include "../../PlayerbotAIConfig.h"
//#include "../ItemVisitors.h"
#include "../../RandomPlayerbotMgr.h"
//#include "../../../../game/LFGMgr.h"
#include "strategy/values/PositionValue.h"
//#include "ServerFacade.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "Object.h"
#include "ObjectMgr.h"
#include "strategy/values/LastMovementValue.h"
#include "strategy/actions/LogLevelAction.h"
#include "strategy/values/LastSpellCastValue.h"
#include "MovementActions.h"
#include "MotionMaster.h"
#include "MovementGenerator.h"
#include "../values/PositionValue.h"
#include "MotionGenerators/TargetedMovementGenerator.h"
#include "BattleGround.h"
#include "BattleGroundMgr.h"
#include "BattlegroundJoinAction.h"

using namespace ai;


bool BGJoinAction::Execute(Event event)
{
   if (!sPlayerbotAIConfig.randomBotJoinBG)
      return false;

   if (bot->IsDead())
      return false;

   if (bot->getLevel() < 15)
      return false;

   if (!sRandomPlayerbotMgr.IsRandomBot(bot))
      return false;

   if (bot->InBattleGroundQueue())
      return false;

   if (bot->IsBeingTeleported())
      return false;

   Map* map = bot->GetMap();
   if (map && map->Instanceable())
      return false;

   uint32 bgType = BATTLEGROUND_WS;

   return JoinProposal(bgType);
}

bool BGJoinAction::JoinProposal(uint32 type)
{
   //WorldSession *m_session = bot->GetSession();

   //Player* bot = m_session->GetPlayer();

   if (bot->getLevel() < 15)
      return false;

   if (bot->InBattleGroundQueue())
       return false;

   // get BattleMaster Entry
   uint32 BmEntry = bot->GetPlayerbotAI()->GetBattleMasterEntryByRace(bot->getRace());
   uint32 BmGuid = bot->GetPlayerbotAI()->GetBattleMasterGuidByRace(bot->getRace());
   // get BattleMaster GUID
   ObjectGuid guid = ObjectGuid(HIGHGUID_UNIT, BmEntry, BmGuid);
   // get BG TypeId
   BattleGroundTypeId bgTypeId = BattleGroundTypeId(type);
   // get BG MapId
   uint32 mapId = GetBattleGrounMapIdByTypeId(bgTypeId);
   uint32 instanceId = 0; // 0 = First Available
   bool joinAsGroup = bot->GetGroup() && bot->GetGroup()->GetLeaderGuid() == guid ? true : false;
   bool isPremade = false;
   Group* grp = nullptr;

   sLog.outBasic("Bot %u (%d %s) queued for BG (%s)", bot->GetGUIDLow(), bot->getLevel(), bot->GetTeamId() == 0 ? "A" : "H", mapId == 489 ? "WSG" : (mapId == 529 ? "AB" : "AV"));
   
   uint32 bracket = ai->GetBracketByLevel(bot->getLevel());
   sRandomPlayerbotMgr.BracketBots[bracket][bot->GetTeamId()]++;

   WorldPacket packet(CMSG_BATTLEMASTER_JOIN, 20);
   packet << guid << mapId << instanceId << joinAsGroup;
   bot->GetSession()->HandleBattlemasterJoinOpcode(packet);

   //ObjectGuid guid;
   //uint32 instanceId;
   //uint32 mapId;
   //uint8 joinAsGroup;
   //bool isPremade = false;
   //Group* grp;

   //recv_data >> guid;                                      // battlemaster guid
   //recv_data >> mapId;
   //recv_data >> instanceId;                                // instance id, 0 if First Available selected
   //recv_data >> joinAsGroup;                               // join as group

   /*BattleGroundTypeId bgTypeId = BattleGroundTypeId(bgTypeId_);

   //sLog.outDetail("WORLD: Received opcode CMSG_BATTLEMASTER_JOIN from %s", guid.GetString().c_str());

   // can do this, since it's battleground, not arena
   BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(bgTypeId
#ifndef MANGOSBOT_ZERO
       , ARENA_TYPE_NONE
#endif
   );

   // ignore if player is already in BG
   if (bot->InBattleGround())
      return false;

   // get bg instance or bg template if instance not found
   BattleGround* bg = nullptr;
   if (instanceId)
      bg = sBattleGroundMgr.GetBattleGroundThroughClientInstance(instanceId, bgTypeId);
   if (!bg)
      bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
   if (!bg)
   {
      sLog.outError("Battleground: no available bg / template found");
      return false;
   }

   BattleGroundBracketId bgBracketId = bot->GetBattleGroundBracketIdFromLevel(bgTypeId);

   // check queue conditions
   if (!joinAsGroup)
   {
      // check Deserter debuff
      if (!bot->CanJoinToBattleground())
      {
         WorldPacket data(SMSG_GROUP_JOINED_BATTLEGROUND, 4);
         data << uint32(0xFFFFFFFE);
         bot->GetSession()->SendPacket(&data);
         return false;
      }
      // check if already in queue
      if (bot->GetBattleGroundQueueIndex(bgQueueTypeId) < PLAYER_MAX_BATTLEGROUND_QUEUES)
         // player is already in this queue
         return false;
      // check if has free queue slots
      if (!bot->HasFreeBattleGroundQueueId())
         return false;
   }
   else
   {
      grp = bot->GetGroup();
      // no group found, error
      if (!grp)
         return false;
      uint32 err = grp->CanJoinBattleGroundQueue(bgTypeId, bgQueueTypeId, 0, bg->GetMaxPlayersPerTeam()
#ifndef MANGOSBOT_ZERO
          , false, 0
#endif
          );
      isPremade = sWorld.getConfig(CONFIG_UINT32_BATTLEGROUND_PREMADE_GROUP_WAIT_FOR_MATCH) &&
         (grp->GetMembersCount() >= bg->GetMinPlayersPerTeam());
      if (err != BG_JOIN_ERR_OK)
      {
#ifdef MANGOSBOT_ZERO
         m_session->SendBattleGroundJoinError(err);
#else
         m_session->SendBattleGroundOrArenaJoinError(err);
#endif
         return false;
      }
   }
   // if we're here, then the conditions to join a bg are met. We can proceed in joining.

   // _player->GetGroup() was already checked, grp is already initialized
   BattleGroundQueue& bgQueue = sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId];
   if (joinAsGroup)
   {
      //sLog.outDetail("Battleground: the following players are joining as group:");
      //sLog.outDetail("Battleground: the following players are joining as group:");
      GroupQueueInfo* ginfo = bgQueue.AddGroup(bot, grp, bgTypeId, bgBracketId
#ifdef MANGOSBOT_ZERO
          , isPremade
#else
          , ARENA_TYPE_NONE, false, isPremade, 0
#endif
      );
      uint32 avgTime = bgQueue.GetAverageQueueWaitTime(ginfo, bot->GetBattleGroundBracketIdFromLevel(bgTypeId));
      for (GroupReference* itr = grp->GetFirstMember(); itr != nullptr; itr = itr->next())
      {
         Player* member = itr->getSource();
         if (!member)
            continue;                                   // this should never happen

         uint32 queueSlot = member->AddBattleGroundQueueId(bgQueueTypeId);           // add to queue

         // store entry point coords (same as leader entry point)
         member->SetBattleGroundEntryPoint(bot);

         // send status packet (in queue)
         WorldPacket data;
         sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, queueSlot, STATUS_WAIT_QUEUE, avgTime, 0
#ifndef MANGOSBOT_ZERO
             , ginfo->arenaType, TEAM_NONE
#endif
         );
         member->GetSession()->SendPacket(&data);
         sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(&data, bgTypeId);
         member->GetSession()->SendPacket(&data);
         //sLog.outDetail("Battleground: player joined queue for bg queue type %u bg type %u: GUID %u, NAME %s", bgQueueTypeId, bgTypeId, member->GetGUIDLow(), member->GetName());
      }
      sLog.outBasic("Battleground: group end");
   }
   else
   {
      GroupQueueInfo* ginfo = bgQueue.AddGroup(bot, NULL, bgTypeId, bgBracketId
#ifdef MANGOSBOT_ZERO
          , isPremade
#else
          , ARENA_TYPE_NONE, false, isPremade, 0
#endif
      );
      uint32 avgTime = bgQueue.GetAverageQueueWaitTime(ginfo, bot->GetBattleGroundBracketIdFromLevel(bgTypeId));
      // already checked if queueSlot is valid, now just get it
      uint32 queueSlot = bot->AddBattleGroundQueueId(bgQueueTypeId);
      // store entry point coords
      bot->SetBattleGroundEntryPoint();

      WorldPacket data;
      // send status packet (in queue)
      sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, queueSlot, STATUS_WAIT_QUEUE, avgTime, 0
#ifndef MANGOSBOT_ZERO
          , ginfo->arenaType, TEAM_NONE
#endif
      );
      m_session->SendPacket(&data);
      //sLog.outDetail("Battleground: player joined queue for bg queue type %u bg type %u: GUID %u, NAME %s", bgQueueTypeId, bgTypeId, bot->GetGUIDLow(), bot->GetName());
   }
   sBattleGroundMgr.ScheduleQueueUpdate(
#ifndef MANGOSBOT_ZERO
       0, ARENA_TYPE_NONE, 
#endif
       bgQueueTypeId, bgTypeId, bot->GetBattleGroundBracketIdFromLevel(bgTypeId));*/
   return true;
}

bool BGStatusAction::Execute(Event event)
{
    uint32 QueueSlot;
    uint8 arenatype;
    uint8 arenaByte;
    uint32 instanceId;
    uint16 x1f90;
    uint8 minlevel;
    uint8 maxlevel;
    uint32 mapId;
    uint8 isRated;
    uint32 statusid;
    uint8 unk1;
    uint32 Time1;
    uint32 Time2;

    WorldPacket p(event.getPacket());
    statusid = 0;

    //on status 0, the packet has a uint64 as payload only. Took me two suicide tries and a gnomen porn video to find out
#ifndef MANGOSBOT_ZERO
    p >> QueueSlot >> arenatype >> arenaByte >> battleId >> x1f90;
    if (x1f90 != 0)
        p >> minlevel >> maxlevel >> instanceId >> isRated >> statusid;
#else
    //p >> QueueSlot >> battleId;
    //if (battleId != 0)
    //    p >> arenatype >> instanceId >> statusid;
    //p >> QueueSlot >> mapId >> minlevel >> instanceId >> statusid;
    p >> QueueSlot; // queue id (0...2) - player can be in 3 queues in time
    p >> mapId;     // MapID
    if (mapId == 0)
        return false;
    p >> unk1;      // Unknown
    p >> instanceId;
    p >> statusid;  // status

    // check status
    switch (statusid)
    {
    case STATUS_WAIT_QUEUE:                  // status_in_queue
        p >> Time1;                         // average wait time, milliseconds
        p >> Time2;                        // time in queue, updated every minute!, milliseconds
        break;
    case STATUS_WAIT_JOIN:                   // status_invite
        p >> Time1;                         // time to remove from queue, milliseconds
        break;
    case STATUS_IN_PROGRESS:                 // status_in_progress
        p >> Time1;                         // time to bg auto leave, 0 at bg start, 120000 after bg end, milliseconds
        p >> Time2;                        // time from bg start, milliseconds
        break;
    default:
        sLog.outError("Unknown BG status!");
        //return false;
        break;
    }
                    //if (instanceId != 0)
        //p >> minlevel >> battleId >> statusid;
#endif
    bool IsRandomBot = sRandomPlayerbotMgr.IsRandomBot(bot->GetGUIDLow());
    BattleGroundTypeId bgTypeId = BattleGroundTypeId(instanceId);
    //mapId = GetBattleGrounMapIdByTypeId(bgTypeId);
    string bgname = mapId == 489 ? "WSG" : (mapId == 529 ? "AB" : "AV");
    //uint32 GetBattleGrounMapIdByTypeId(BattleGroundTypeId bgTypeId);
    /*if (battleId != 0)
    {
        //battleId = 2;
        sLog.outBasic("Battleground: player (bot) %s battleId is %u", bot->GetName(), battleId);
        sLog.outBasic("Battleground: player (bot) %s QueueSlot is %u", bot->GetName(), QueueSlot);
        sLog.outBasic("Battleground: player (bot) %s x1f90 is %u", bot->GetName(), x1f90);
        sLog.outBasic("Battleground: player (bot) %s instanceId is %u", bot->GetName(), instanceId);
        sLog.outBasic("Battleground: player (bot) %s STATUS is %u", bot->GetName(), statusid);
    }*/

    if (Time1 == TIME_TO_AUTOREMOVE) //battleground is over, bot needs to leave
    {
        sLog.outBasic("Bot %u leaves BG (%s).", bot->GetGUIDLow(), bgname);
        //bot->LeaveBattleground(true);
        WorldPacket packet(CMSG_LEAVE_BATTLEFIELD);
        packet << uint8(0);
        packet << uint8(0);                           // BattleGroundTypeId-1 ?
        packet << uint16(0);
        bot->GetSession()->HandleLeaveBattlefieldOpcode(packet);
        ai->ResetStrategies(!IsRandomBot);
        uint32 bracket = ai->GetBracketByLevel(bot->getLevel());
        sRandomPlayerbotMgr.BracketBots[bracket][bot->GetTeamId()]--;
        if (urand(0, 100) > 50)
        {
            if (IsRandomBot)
            {
                //sRandomPlayerbotMgr.SetValue(bot->GetGUIDLow(), "bg", 10);
            }
                
        }
        else
        {
            if (IsRandomBot)
            {
                //sRandomPlayerbotMgr.AddBgBot(bot);
            }
                
        }
    }
    if (statusid == STATUS_WAIT_QUEUE) //bot is in queue
    {
        sLog.outDetail("Bot %u (%u %s) is in BG queue (%s).", bot->GetGUIDLow(), bot->getLevel(), bot->GetTeamId() == 0 ? "A" : "H", bgname);
    }
    if (statusid == STATUS_WAIT_JOIN) //bot may join
    {
        uint8 type = 0;                                             // arenatype if arena
        uint8 unk2;                                             // unk, can be 0x0 (may be if was invited?) and 0x1
        uint32 bgTypeId_ = bgTypeId;                                       // type id from dbc
        uint16 unk = 0x1F90;                                              // 0x1F90 constant?*/
        uint8 action = 1;

        sLog.outBasic("Bot %u (%u %s) joined BG (%s)", bot->GetGUIDLow(), bot->getLevel(), bot->GetTeamId() == 0 ? "A" : "H", bgname);

        WorldPacket packet(CMSG_BATTLEFIELD_PORT, 20);
        //packet << type << unk2 << bgTypeId_ << unk << action;
        packet << mapId << action;
        bot->GetSession()->HandleBattleFieldPortOpcode(packet);
        
        ai->ResetStrategies(!IsRandomBot);
        ai->ChangeStrategy("-bg", BOT_STATE_NON_COMBAT);
        return true;
    }
        /*if (bot->InBattleGround())
        {
            //sLog.outDebug
            sLog.outDetail("Battleground: player (bot) %s (%u) is in battleground already. (%u, %u).", bot->GetName(), bot->GetGUIDLow(), battleId, instanceId);
            return false;
        }

        sLog.outDetail("Battleground: player (bot) %s (%u) received a invite to a battleground (%u, %u).", bot->GetName(), bot->GetGUIDLow(), battleId, instanceId);

        bot->CombatStop(true);

        sLog.outDetail("Bot %s is going to join BG %d", bot->GetName(), battleId);

        bot->clearUnitState(UNIT_STAT_ALL_STATE);

        //get GroupQueueInfo from BattlegroundQueue
        BattleGroundTypeId bgTypeId = BattleGroundTypeId(battleId);
        BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(bgTypeId
#ifndef MANGOSBOT_ZERO
            , 0
#endif
        );
        BattleGroundQueue& bgQueue = sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId];
        //we must use temporary variable, because GroupQueueInfo pointer can be deleted in BattlegroundQueue::RemovePlayer() function
        GroupQueueInfo ginfo;
        //================================================================== TODO
        if (!bgQueue.GetPlayerGroupInfoData(bot->GetObjectGuid(), &ginfo))
        {
            sLog.outError("Bot %s was not in queue to join BG %d", bot->GetName(), battleId);
            return false;
        }
        //================================================================== TODO
        // if action == 1, then instanceId is required
        //================================================================== TODO
        if (!ginfo.IsInvitedToBGInstanceGUID)
        {
            sLog.outError("Bot %s had no instanceID to join BG %d", bot->GetName(), battleId);
            return false;
        }
        //================================================================== TODO

        BattleGround* bg = sBattleGroundMgr.GetBattleGround(ginfo.IsInvitedToBGInstanceGUID, bgTypeId);
        if (!bg)
        {
            bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
            if (!bg)
            {
                sLog.outError("Bot %s could find no template for Battleground %d", bot->GetName(), battleId);
                return false;
            }
        }

        // expected bracket entry
        BattleGroundBracketId bracketEntry = bot->GetBattleGroundBracketIdFromLevel(bg->GetTypeID());
        if (!bracketEntry)
        {
            sLog.outError("Bot %s has no bracket (%d) available to join BG %d", bot->GetName(), bot->getLevel(), battleId);
            return false;
        }

        //some checks if player isn't cheating - it is not exactly cheating, but we cannot allow it
#ifndef MANGOSBOT_ZERO
        if (ginfo.ArenaType == 0)
        {
#endif
            //if player is trying to enter battleground (not arena!) and he has deserter debuff, we must just remove him from queue
            if (!bot->CanJoinToBattleground())
            {
                sLog.outError("Bot %s cant join BG %d because it has the deserter debuf", bot->GetName(), battleId);
                return false;
            }
            //if player don't match battleground max level, then do not allow him to enter! (this might happen when player leveled up during his waiting in queue
            if (bot->getLevel() > bg->GetMaxLevel())
            {
                sLog.outError("Bot %s cant join BG %d because its level does not fit", bot->GetName(), battleId);
                return false;
            }
#ifndef MANGOSBOT_ZERO
        }
#endif
        //uint32 queueSlot = bot->GetBattlegroundQueueIndex(bgQueueTypeId);
        // check Freeze debuff
        if (bot->HasAura(9454))
        {
            sLog.outError("Bot %s cant join BG %d because it is freezed", bot->GetName(), battleId);
            return false;
        }

        if (!bot->IsInvitedForBattleGroundQueueType(bgQueueTypeId))
        {
            sLog.outError("Bot %s cant join BG %d because it was not invited to join", bot->GetName(), battleId);
            return false;                                 // cheating? No, bots dont cheat. They just try very hard.
        }

        if (!bot->InBattleGround())
            bot->SetBattleGroundEntryPoint();

        // resurrect the player
        if (!bot->IsAlive())
        {
            bot->ResurrectPlayer(1.0f);
            bot->SpawnCorpseBones();
        }
        // stop taxi flight at port
        bot->GetMotionMaster()->MovementExpired();
        if (bot->IsTaxiFlying())
        {
            bot->CleanupAfterTaxiFlight();
        }

        // remove battleground queue status from BGmgr
        bgQueue.RemovePlayer(bot->GetObjectGuid(), false);
        // this is still needed here if battleground "jumping" shouldn't add deserter debuff
        // also this is required to prevent stuck at old battleground after SetBattlegroundId set to new
        if (BattleGround* currentBg = bot->GetBattleGround())
            currentBg->RemovePlayerAtLeave(bot->GetObjectGuid(), false, true);

        // set the destination instance id
        bot->SetBattleGroundId(bg->GetInstanceID(), bgTypeId);
        // set the destination team
        bot->SetBGTeam(ginfo.GroupTeam);

        // bg->HandleBeforeTeleportToBattleground(_player);
        sBattleGroundMgr.SendToBattleGround(bot, ginfo.IsInvitedToBGInstanceGUID, bgTypeId);
        ai->ResetStrategies(false);
        // add only in HandleMoveWorldPortAck()
        // bg->AddPlayer(_player, team);
        sLog.outDetail("Battleground: player (bot) %s (%u) joined battle for bg %u, bgtype %u, queue type %u.", bot->GetName(), bot->GetGUIDLow(), bg->GetInstanceID(), bg->GetTypeID(), bgQueueTypeId);
        sLog.outBasic("Bot %s joined BG %d", bot->GetName(), battleId);
    }*/
    return true;
}