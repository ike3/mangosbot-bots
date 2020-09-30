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

   if (bot->getLevel() < 10)
      return false;

   //if (!sRandomPlayerbotMgr.IsRandomBot(bot))
      //return false;

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
    // ignore if player is already in BG
    if (bot->InBattleGround())
        return false;

    // get BG TypeId
    BattleGroundTypeId bgTypeId = BattleGroundTypeId(type);

    BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
    if (!bg)
        return false;

    // check Deserter debuff
    if (!bot->CanJoinToBattleground())
        return false;

    // check if already in queue
    if (bot->InBattleGroundQueue())
        return false;

   if (bot->getLevel() < 10)
      return false;

   // check if has free queue slots
   if (!bot->HasFreeBattleGroundQueueId())
   {
       return false;
   }

   // get BattleMaster Entry
   uint32 BmEntry = bot->GetPlayerbotAI()->GetBattleMasterEntryByRace(bot->getRace());
   uint32 BmGuid = bot->GetPlayerbotAI()->GetBattleMasterGuidByRace(bot->getRace());
   // get BattleMaster GUID
   ObjectGuid guid = ObjectGuid(HIGHGUID_UNIT, BmEntry, BmGuid);
   // get BG MapId
   uint32 mapId = GetBattleGrounMapIdByTypeId(bgTypeId);
   uint32 instanceId = 0; // 0 = First Available
   bool joinAsGroup = bot->GetGroup() && bot->GetGroup()->GetLeaderGuid() == bot->GetObjectGuid() ? true : false;
   bool isPremade = false;

   sLog.outBasic("Bot %u (%d %s) queued for BG (%s)", bot->GetGUIDLow(), bot->getLevel(), bot->GetTeamId() == 0 ? "A" : "H", mapId == 489 ? "WSG" : (mapId == 529 ? "AB" : "AV"));
   
   BattleGroundBracketId bracketId = bot->GetBattleGroundBracketIdFromLevel(BATTLEGROUND_WS);
   sRandomPlayerbotMgr.BracketBots[bgTypeId][bracketId][bot->GetTeamId()]++;

   WorldPacket packet(CMSG_BATTLEMASTER_JOIN, 20);
   packet << guid << mapId << instanceId << joinAsGroup;
   bot->GetSession()->HandleBattlemasterJoinOpcode(packet);
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
    //BattleGroundTypeId bgTypeId = BattleGroundTypeId(instanceId);
    BattleGroundTypeId bgTypeId = GetBattleGroundTypeIdByMapId(mapId);
    BattleGroundBracketId bracketId = bot->GetBattleGroundBracketIdFromLevel(BATTLEGROUND_WS);
    string bgname = mapId == 489 ? "WSG" : (mapId == 529 ? "AB" : "AV");

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
        sRandomPlayerbotMgr.BracketBots[bgTypeId][bracketId][bot->GetTeamId()] = 0;
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
        bot->Unmount();

        WorldPacket packet(CMSG_BATTLEFIELD_PORT, 20);
        //packet << type << unk2 << bgTypeId_ << unk << action;
        packet << mapId << action;
        bot->GetSession()->HandleBattleFieldPortOpcode(packet);
        
        ai->ResetStrategies(!IsRandomBot);
        ai->ChangeStrategy("-bg", BOT_STATE_NON_COMBAT);
        ai->ChangeStrategy("-bg", BOT_STATE_COMBAT);
        
        return true;
    }
    return true;
}