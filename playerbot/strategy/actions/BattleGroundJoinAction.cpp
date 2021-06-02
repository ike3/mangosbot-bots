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
//#include "strategy/values/PositionValue.h"
//#include "ServerFacade.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "Object.h"
#include "ObjectMgr.h"
#include "strategy/values/LastMovementValue.h"
#include "strategy/actions/LogLevelAction.h"
#include "strategy/values/LastSpellCastValue.h"
#include "../values/PositionValue.h"
#include "MovementActions.h"
#include "MotionMaster.h"
#include "MovementGenerator.h"
//#include "../values/PositionValue.h"
#include "MotionGenerators/TargetedMovementGenerator.h"
#include "BattleGround.h"
#include "BattleGroundMgr.h"
#include "BattlegroundJoinAction.h"

using namespace ai;


bool BGJoinAction::Execute(Event event)
{
    if (bgList.empty())
        return false;

    BattleGroundQueueTypeId queueTypeId = (BattleGroundQueueTypeId)bgList[urand(0, bgList.size() - 1)];
    BattleGroundTypeId bgTypeId = sServerFacade.BgTemplateId(queueTypeId);
    bool isArena = false;
    bool isRated = false;

    // Find BattleMaster by Entry
    uint32 BmEntry = sRandomPlayerbotMgr.GetBattleMasterEntry(bot, bgTypeId, true);
    if (!BmEntry)
    {
        sLog.outError("Bot #%d <%s> could not find Battlemaster for %d", bot->GetGUIDLow(), bot->GetName(), bgTypeId);
        return false;
    }

    // check bm map
    CreatureDataPair const* dataPair = sRandomPlayerbotMgr.GetCreatureDataByEntry(BmEntry);
    CreatureData const* data = &dataPair->second;
    ObjectGuid BmGuid = ObjectGuid(HIGHGUID_UNIT, BmEntry, dataPair->first);
    if (data->mapid != bot->GetMapId())
    {
        sLog.outError("Bot #%d <%s> : Battlemaster is not in map for BG %d", bot->GetGUIDLow(), bot->GetName(), bgTypeId);
        return false;
    }

#ifndef MANGOSBOT_ZERO
    if (sServerFacade.BgArenaType(queueTypeId))
    {
        isArena = true;

        vector<uint32>::iterator i = find(ratedList.begin(), ratedList.end(), queueTypeId);
        if (i != ratedList.end())
            isRated = true;

        ai->GetAiObjectContext()->GetValue<uint32>("arena type")->Set(isRated);
    }
#endif

    // set bg type and bm guid
    ai->GetAiObjectContext()->GetValue<ObjectGuid>("bg master")->Set(BmGuid);
    ai->GetAiObjectContext()->GetValue<uint32>("bg type")->Set(queueTypeId);


   //uint32 queueType = AI_VALUE(uint32, "bg type");
   return JoinQueue(queueTypeId);
}

bool BGJoinAction::canJoinBg(BattleGroundQueueTypeId queueTypeId, BattleGroundBracketId bracketId)
{
    BattleGroundTypeId bgTypeId = sServerFacade.BgTemplateId(queueTypeId);
    BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
    if (!bg)
        return false;

    // do not try if too low/high level
    if (!bot->GetBGAccessByLevel(bgTypeId))
        return false;

    // do not try if bracket is wrong
#ifdef MANGOSBOT_TWO
    BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
    uint32 mapId = bg->GetMapId();
    PvPDifficultyEntry const* pvpDiff = GetBattlegroundBracketByLevel(mapId, bot->getLevel());
    if (!pvpDiff)
        return false;

    BattleGroundBracketId bracket_temp = pvpDiff->GetBracketId();

    if (bracket_temp != bracketId)
        return false;
#else
    if (bot->GetBattleGroundBracketIdFromLevel(bgTypeId) != bracketId)
        return false;
#endif
    return true;
}

bool BGJoinAction::shouldJoinBg(BattleGroundQueueTypeId queueTypeId, BattleGroundBracketId bracketId)
{
    BattleGroundTypeId bgTypeId = sServerFacade.BgTemplateId(queueTypeId);
    BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
    if (!bg)
        return false;

    if ((sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][0] +
        sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][0] +
        sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][1] +
        sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][1]) == 0/* && urand(0, 100) > 5*/) // can enable auto join here
        return false;

    if ((sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][0] +
        sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][1]) == 0/* && urand(0, 100) > 5*/) // can enable auto join here
        return false;

    bool isArena = false;
    bool isRated = false;

    uint32 BracketSize = bg->GetMaxPlayers();
    uint32 TeamSize = bg->GetMaxPlayersPerTeam();

    uint32 ACount = sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][0] + sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][0];
    uint32 HCount = sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][1] + sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][1];

    uint32 BgCount = ACount + HCount;
    uint32 SCount, RCount;

    uint32 TeamId = GetTeamIndexByTeamId(bot->GetTeam());

    bool needBots = sRandomPlayerbotMgr.NeedBots[queueTypeId][bracketId][GetTeamIndexByTeamId(bot->GetTeam())];

#ifndef MANGOSBOT_ZERO
    ArenaType type = sServerFacade.BgArenaType(queueTypeId);
    if (type != ARENA_TYPE_NONE)
    {
        isArena = true;
        BracketSize = type * 2;
        TeamSize = type;
        ACount = sRandomPlayerbotMgr.ArenaBots[queueTypeId][bracketId][isRated][0];
        HCount = sRandomPlayerbotMgr.ArenaBots[queueTypeId][bracketId][isRated][1];
        BgCount = sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][isRated] + sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][isRated];
        SCount = sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][0] + sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][0];
        RCount = sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][1] + sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][1];
        uint32 rated_players = sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][1];
        if (rated_players)
        {
            isRated = true;
        }
    }

    // do not try if not a captain of arena team
    if (isRated && sObjectMgr.GetArenaTeamByCaptain(bot->GetObjectGuid()))
        return false;

    if (isRated)
        ratedList.push_back(queueTypeId);
#endif

    if (needBots || (BgCount > BracketSize && BgCount % BracketSize != 0))
        return true;

    // do not join if BG queue is full
    if (BgCount >= BracketSize && (ACount >= TeamSize) && (HCount >= TeamSize))
    {
        return false;
    }

    if (!isArena && ((ACount >= TeamSize && TeamId == 0) || (HCount >= TeamSize && TeamId == 1)))
    {
        return false;
    }

    if (isArena && (((ACount >= TeamSize && HCount > 0) && TeamId == 0) || ((HCount >= TeamSize && ACount > 0) && TeamId == 1)))
    {
        return false;
    }

    if (isArena && (((ACount > TeamSize && HCount == 0) && TeamId == 1) || ((HCount > TeamSize && ACount == 0) && TeamId == 0)))
    {
        return false;
    }

    if (isArena && ((!isRated && SCount >= BracketSize) || (isRated && RCount >= BracketSize)))
    {
        return false;
    }

    return true;
}

bool BGJoinAction::isUseful()
{
    // do not try if BG bots disabled
    if (!sPlayerbotAIConfig.randomBotJoinBG)
        return false;

    // do not try right after login
    if ((time(0) - bot->GetInGameTime()) < 30)
        return false;

    // do not try if low level
    if (bot->getLevel() < 10)
        return false;

    // do not try if with player master
    if (bot->GetPlayerbotAI()->GetMaster() && bot->GetPlayerbotAI()->hasRealPlayerMaster())
        return false;

    if (bot->GetGroup())
        return false;

    if (bot->IsInCombat())
        return false;

    if (bot->InBattleGroundQueue())
        return false;

    // check Deserter debuff
    if (!bot->CanJoinToBattleground())
        return false;

    // do not try if in dungeon
    Map* map = bot->GetMap();
    if (map && map->Instanceable())
        return false;

    bgList.clear();
    ratedList.clear();

    for (int i = BG_BRACKET_ID_FIRST; i < MAX_BATTLEGROUND_BRACKETS; ++i)
    {
        for (int j = BATTLEGROUND_QUEUE_AV; j < MAX_BATTLEGROUND_QUEUE_TYPES; ++j)
        {
            BattleGroundQueueTypeId queueTypeId = BattleGroundQueueTypeId(j);
            BattleGroundTypeId bgTypeId = sServerFacade.BgTemplateId(queueTypeId);
            BattleGroundBracketId bracketId = BattleGroundBracketId(i);

            if (!canJoinBg(queueTypeId, bracketId))
                continue;

            if (shouldJoinBg(queueTypeId, bracketId))
                bgList.push_back(queueTypeId);
        }
    }

    if (!bgList.empty())
        return true;

    return false;
}

bool BGJoinAction::JoinQueue(uint32 type)
{
    // ignore if player is already in BG
    if (bot->InBattleGround())
        return false;

    // get BG TypeId
    BattleGroundQueueTypeId queueTypeId = BattleGroundQueueTypeId(type);
    BattleGroundTypeId bgTypeId = sServerFacade.BgTemplateId(queueTypeId);
    BattleGroundBracketId bracketId;

    BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
    if (!bg)
        return false;

#ifdef MANGOSBOT_TWO
    BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
    uint32 mapId = bg->GetMapId();
    PvPDifficultyEntry const* pvpDiff = GetBattlegroundBracketByLevel(mapId, bot->getLevel());
    if (!pvpDiff)
        return false;

    bracketId = pvpDiff->GetBracketId();
#else
    bracketId = bot->GetBattleGroundBracketIdFromLevel(bgTypeId);
#endif

    uint32 BracketSize = bg->GetMaxPlayers();
    uint32 TeamSize = bg->GetMaxPlayersPerTeam();
    uint32 TeamId = GetTeamIndexByTeamId(bot->GetTeam());

   // check if has free queue slots
   if (!bot->HasFreeBattleGroundQueueId())
   {
       return false;
   }

   // get BattleMaster GUID
   ObjectGuid guid = AI_VALUE(ObjectGuid, "bg master");
   if (!guid)
   {
       sLog.outError("Bot %d could not find Battlemaster to join", bot->GetGUIDLow());
       return false;
   }
   // get BG MapId
#ifdef MANGOSBOT_ZERO
   uint32 mapId = GetBattleGrounMapIdByTypeId(bgTypeId);
#else
   uint32 bgTypeId_ = bgTypeId;
#endif
   uint32 instanceId = 0; // 0 = First Available
   bool joinAsGroup = bot->GetGroup() && bot->GetGroup()->GetLeaderGuid() == bot->GetObjectGuid() ? true : false;
   bool isPremade = false;
   bool isArena = false;
   bool isRated = false;
   uint8 arenaslot = 0;
   uint8 asGroup = false;
   string _bgType;

   switch (bgTypeId)
   {
   case BATTLEGROUND_AV:
       _bgType = "AV";
       break;
   case BATTLEGROUND_WS:
       _bgType = "WSG";
       break;
   case BATTLEGROUND_AB:
       _bgType = "AB";
       break;
#ifndef MANGOSBOT_ZERO
   case BATTLEGROUND_EY:
       _bgType = "EotS";
       break;
#endif
   default:
       break;
}

#ifndef MANGOSBOT_ZERO
   ArenaType arenaType = sServerFacade.BgArenaType(queueTypeId);
   if (arenaType != ARENA_TYPE_NONE)
   {
       isArena = true;
       BracketSize = type * 2;
       TeamSize = type;
       isRated = ai->GetAiObjectContext()->GetValue<uint32>("arena type")->Get();

       if (joinAsGroup)
           asGroup = true;

       switch (arenaType)
       {
       case ARENA_TYPE_2v2:
           arenaslot = 0;
           _bgType = "2v2";
           break;
       case ARENA_TYPE_3v3:
           arenaslot = 1;
           _bgType = "3v3";
           break;
       case ARENA_TYPE_5v5:
           arenaslot = 2;
           _bgType = "5v5";
           break;
       default:
           break;
       }
   }
#endif

   // refresh food/regs
   sRandomPlayerbotMgr.Refresh(bot);

   if (isArena)
   {
       if (isRated)
       {
           sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][isRated] += TeamSize;
           sRandomPlayerbotMgr.ArenaBots[queueTypeId][bracketId][isRated][TeamId] += TeamSize;
       }
       else
       {
           sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][isRated]++;
           sRandomPlayerbotMgr.ArenaBots[queueTypeId][bracketId][isRated][TeamId]++;
       }
   }
   else
       sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][TeamId]++;

   WorldPacket packet(CMSG_BATTLEMASTER_JOIN, 20);
#ifdef MANGOSBOT_ZERO
   packet << guid << mapId << instanceId << joinAsGroup;
   sLog.outBasic("Bot #%d %s:%d <%s> queued %s", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), _bgType);
#else
   sLog.outBasic("Bot #%d %s:%d <%s> queued %s %s", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), _bgType, isRated ? "Rated Arena" : isArena ? "Arena" : "");
   if (!isArena)
   {
       packet << guid << bgTypeId_ << instanceId << joinAsGroup;
   }
   else
   {
       WorldPacket arena_packet(CMSG_BATTLEMASTER_JOIN_ARENA, 20);
       arena_packet << guid << arenaslot << asGroup << isRated;
       bot->GetSession()->HandleBattlemasterJoinArena(arena_packet);
       return true;
   }
#endif
   // refresh food/regs
   sRandomPlayerbotMgr.Refresh(bot);

   bot->GetSession()->HandleBattlemasterJoinOpcode(packet);
   return true;
}

bool BGLeaveAction::Execute(Event event)
{
    if (!bot->InBattleGroundQueue())
        return false;

    uint32 queueType = AI_VALUE(uint32, "bg type");
    if (!queueType)
        return false;

    //ai->ChangeStrategy("-bg", BOT_STATE_NON_COMBAT);

    BattleGroundQueueTypeId queueTypeId = bot->GetBattleGroundQueueTypeId(0);
    BattleGroundTypeId _bgTypeId = sServerFacade.BgTemplateId(queueTypeId);
    uint8 type = false;
    uint16 unk = 0x1F90;
    uint8 unk2 = 0x0;
    bool isArena = false;
    bool IsRandomBot = sRandomPlayerbotMgr.IsRandomBot(bot->GetGUIDLow());

#ifndef MANGOSBOT_ZERO
    ArenaType arenaType = sServerFacade.BgArenaType(queueTypeId);
    if (arenaType != ARENA_TYPE_NONE)
    {
        isArena = true;
        type = arenaType;
    }
#endif
    sLog.outDetail("Bot #%d %s:%d <%s> leaves %s queue", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName(), isArena ? "Arena" : "BG");

    WorldPacket packet(CMSG_BATTLEFIELD_PORT, 20);
#ifdef MANGOSBOT_ZERO
    uint32 mapId = GetBattleGrounMapIdByTypeId(_bgTypeId);
    packet << mapId << uint8(0);
#else
    packet << type << unk2 << (uint32)_bgTypeId << unk << uint8(0);
#endif
#ifdef MANGOS
    bot->GetSession()->HandleBattleFieldPortOpcode(packet);
#endif
#ifdef CMANGOS
    bot->GetSession()->HandleBattlefieldPortOpcode(packet);
#endif
    if (IsRandomBot)
        ai->SetMaster(NULL);

    ai->ResetStrategies(!IsRandomBot);
    ai->GetAiObjectContext()->GetValue<uint32>("bg type")->Set(NULL);
    ai->GetAiObjectContext()->GetValue<uint32>("bg role")->Set(NULL);
    ai->GetAiObjectContext()->GetValue<uint32>("arena type")->Set(NULL);
    ai->GetAiObjectContext()->GetValue<ObjectGuid>("bg master")->Set(ObjectGuid());
    return true;
}

bool BGStatusAction::isUseful()
{
    return bot->InBattleGroundQueue();
}

bool BGStatusAction::Execute(Event event)
{
    uint32 QueueSlot;
    uint32 instanceId;
    uint32 mapId;
    uint32 statusid;
    uint32 Time1;
    uint32 Time2;
    uint8 unk1;
    string _bgType;

#ifndef MANGOSBOT_ZERO
    uint64 arenatype;
    uint64 arenaByte;
    uint8 arenaTeam;
    uint8 isRated;
    uint64 unk0;
    uint64 x1f90;
    uint8 minlevel;
    uint8 maxlevel;
    uint64 bgTypeId;
    uint32 battleId;
#endif

    WorldPacket p(event.getPacket());
    statusid = 0;
#ifndef MANGOSBOT_ZERO
    p >> QueueSlot; // queue id (0...2) - player can be in 3 queues in time
    p >> arenaByte;
    if (arenaByte == 0)
        return false;
#ifdef MANGOSBOT_TWO
    p >> minlevel;
    p >> maxlevel;
#endif
    p >> instanceId;
    p >> isRated;
    p >> statusid;

    // check status
    switch (statusid)
    {
    case STATUS_WAIT_QUEUE:                  // status_in_queue
        p >> Time1;                         // average wait time, milliseconds
        p >> Time2;                        // time in queue, updated every minute!, milliseconds
        break;
    case STATUS_WAIT_JOIN:                   // status_invite
        p >> mapId;    //sLog.outBasic("mapId %d!", mapId);                    // map id
#ifdef MANGOSBOT_TWO
        p >> unk0;
#endif
        p >> Time1;   //sLog.outBasic("Time1 %d!", Time1);                      // time to remove from queue, milliseconds
        break;
    case STATUS_IN_PROGRESS:                  // status_in_progress
        p >> mapId;                          // map id
#ifdef MANGOSBOT_TWO
        p >> unk0;
#endif
        p >> Time1;                         // time to bg auto leave, 0 at bg start, 120000 after bg end, milliseconds
        p >> Time2;                        // time from bg start, milliseconds
        p >> arenaTeam;
        break;
    default:
        sLog.outError("Unknown BG status!");
        break;
    }
#else
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
        break;
    }
#endif

    bool IsRandomBot = sRandomPlayerbotMgr.IsRandomBot(bot->GetGUIDLow());
    BattleGroundQueueTypeId queueTypeId = bot->GetBattleGroundQueueTypeId(QueueSlot);
    BattleGroundTypeId _bgTypeId = sServerFacade.BgTemplateId(queueTypeId);
    bool isArena = false;

    uint8 type = false;                                             // arenatype if arena
    //uint32 bgTypeId_ = _bgTypeId;                                       // type id from dbc
    uint16 unk = 0x1F90;
    uint8 unk2 = 0x0;
    uint8 action = 0x1;

#ifndef MANGOSBOT_ZERO
    ArenaType arenaType = sServerFacade.BgArenaType(queueTypeId);
    if (arenaType != ARENA_TYPE_NONE)
    {
        isArena = true;
        type = arenaType;
    }
#endif

    switch (_bgTypeId)
    {
    case BATTLEGROUND_AV:
        _bgType = "AV";
        break;
    case BATTLEGROUND_WS:
        _bgType = "WSG";
        break;
    case BATTLEGROUND_AB:
        _bgType = "AB";
        break;
#ifndef MANGOSBOT_ZERO
    case BATTLEGROUND_EY:
        _bgType = "EotS";
        break;
#endif
    default:
        break;
    }

#ifndef MANGOSBOT_ZERO
    switch (arenaType)
    {
    case ARENA_TYPE_2v2:
        _bgType = "2v2";
        break;
    case ARENA_TYPE_3v3:
        _bgType = "3v3";
        break;
    case ARENA_TYPE_5v5:
        _bgType = "5v5";
        break;
    default:
        break;
    }
#endif

    if (Time1 == TIME_TO_AUTOREMOVE) //battleground is over, bot needs to leave
    {
        BattleGround* bg = bot->GetBattleGround();
        if (bg)
        {
            BattleGroundBracketId bracketId = bot->GetBattleGround()->GetBracketId();
            uint32 TeamId = bot->GetTeam() == ALLIANCE ? 0 : 1;
#ifndef MANGOSBOT_ZERO
            if (isArena)
            {
                sRandomPlayerbotMgr.ArenaBots[queueTypeId][bracketId][isRated][TeamId]--;
                TeamId = isRated ? 1 : 0;
            }
#endif
            sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][TeamId]--;
            sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][TeamId] = 0;
        }

        // remove warsong strategy
        if (IsRandomBot)
            ai->SetMaster(NULL);
        ai->ChangeStrategy("-warsong", BOT_STATE_COMBAT);
        ai->ChangeStrategy("-warsong", BOT_STATE_NON_COMBAT);
        ai->ChangeStrategy("-arathi", BOT_STATE_COMBAT);
        ai->ChangeStrategy("-arathi", BOT_STATE_NON_COMBAT);
        ai->ChangeStrategy("-battleground", BOT_STATE_COMBAT);
        ai->ChangeStrategy("-battleground", BOT_STATE_NON_COMBAT);
        ai->ChangeStrategy("-arena", BOT_STATE_COMBAT);
        ai->ChangeStrategy("-arena", BOT_STATE_NON_COMBAT);
        sLog.outBasic("Bot #%d <%s> leaves %s (%s).", bot->GetGUIDLow(), bot->GetName(), isArena ? "Arena" : "BG", _bgType);

        WorldPacket packet(CMSG_LEAVE_BATTLEFIELD);
        packet << uint8(0);
        packet << uint8(0);                           // BattleGroundTypeId-1 ?
#ifdef MANGOSBOT_ZERO
        packet << uint16(0);
#else
        packet << uint32(0);
        packet << uint16(0);
#endif
        bot->GetSession()->HandleLeaveBattlefieldOpcode(packet);
        //bot->GetSession()->QueuePacket(packet);
        ai->ResetStrategies(!IsRandomBot);
        ai->GetAiObjectContext()->GetValue<uint32>("bg type")->Set(NULL);
        ai->GetAiObjectContext()->GetValue<uint32>("bg role")->Set(NULL);
        ai->GetAiObjectContext()->GetValue<uint32>("arena type")->Set(NULL);
        ai->GetAiObjectContext()->GetValue<ObjectGuid>("bg master")->Set(ObjectGuid());
        ai::PositionMap& posMap = context->GetValue<ai::PositionMap&>("position")->Get();
        ai::PositionEntry pos = context->GetValue<ai::PositionMap&>("position")->Get()["bg objective"];
        pos.Reset();
        posMap["bg objective"] = pos;
    }
    if (statusid == STATUS_WAIT_QUEUE) //bot is in queue
    {
        BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(_bgTypeId);
        if (!bg)
            return false;

        bool leaveQ = false;
        uint32 timer;
        if (isArena)
            timer = TIME_TO_AUTOREMOVE + 30 * 1000;
        else
            timer = TIME_TO_AUTOREMOVE + 1000 * (bg->GetMaxPlayers() * 4);

        if (Time2 > timer)
            leaveQ = true;

        if (leaveQ && !(bot->GetGroup() || ai->GetMaster()))
        {
            uint32 TeamId = bot->GetTeam() == ALLIANCE ? 0 : 1;
            BattleGroundBracketId bracketId = BG_BRACKET_ID_TEMPLATE;
#ifdef CMANGOS
#ifdef MANGOSBOT_TWO
            BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(_bgTypeId);
            uint32 mapId = bg->GetMapId();
            PvPDifficultyEntry const* pvpDiff = GetBattlegroundBracketByLevel(mapId, bot->getLevel());
            if (pvpDiff)
                bracketId = pvpDiff->GetBracketId();

#else
            bracketId = bot->GetBattleGroundBracketIdFromLevel(_bgTypeId);
#endif
#endif
            bool realPlayers = sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][TeamId];
            if (realPlayers)
                return false;
            //ai->ChangeStrategy("-bg", BOT_STATE_NON_COMBAT);
            sLog.outBasic("Bot #%u <%s> (%u %s) waited too long and leaves queue (%s %s).", bot->GetGUIDLow(), bot->GetName(), bot->getLevel(), bot->GetTeam() == ALLIANCE ? "A" : "H", isArena ? "Arena" : "BG", _bgType);
            WorldPacket packet(CMSG_BATTLEFIELD_PORT, 20);
            action = 0;
#ifdef MANGOSBOT_ZERO
            packet << mapId << action;
#else
            packet << type << unk2 << (uint32)_bgTypeId << unk << action;
#endif
#ifdef MANGOS
            bot->GetSession()->HandleBattleFieldPortOpcode(packet);
#endif
#ifdef CMANGOS
            bot->GetSession()->HandleBattlefieldPortOpcode(packet);
#endif
            ai->ResetStrategies(!IsRandomBot);
            ai->GetAiObjectContext()->GetValue<uint32>("bg type")->Set(NULL);
            ai->GetAiObjectContext()->GetValue<uint32>("bg role")->Set(NULL);
            ai->GetAiObjectContext()->GetValue<uint32>("arena type")->Set(NULL);
            ai->GetAiObjectContext()->GetValue<ObjectGuid>("bg master")->Set(ObjectGuid());
            sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][TeamId]--;
            return true;
        }
    }
    if (statusid == STATUS_WAIT_JOIN) //bot may join
    {
#ifndef MANGOSBOT_ZERO
        if (isArena)
        {
            isArena = true;
            BattleGroundQueue& bgQueue = sServerFacade.bgQueue(queueTypeId);
            GroupQueueInfo ginfo;
            if (!bgQueue.GetPlayerGroupInfoData(bot->GetObjectGuid(), &ginfo))
            {
                return false;
            }
#ifdef MANGOS
            if (ginfo.IsInvitedToBGInstanceGUID)
            {
                BattleGround* bg = sBattleGroundMgr.GetBattleGround(ginfo.IsInvitedToBGInstanceGUID, BATTLEGROUND_TYPE_NONE);
                if (!bg)
                {
                    return false;
                }

                _bgTypeId = bg->GetTypeID();
            }
#endif
#ifdef CMANGOS
            if (ginfo.isInvitedToBgInstanceGuid)
            {
                BattleGround* bg = sBattleGroundMgr.GetBattleGround(ginfo.isInvitedToBgInstanceGuid, BATTLEGROUND_TYPE_NONE);
                if (!bg)
                {
                    return false;
                }

                _bgTypeId = bg->GetTypeId();
            }
        }
#endif
#endif

#ifdef MANGOSBOT_ZERO
        sLog.outBasic("Bot #%d <%s> (%u %s) joined BG (%s)", bot->GetGUIDLow(), bot->GetName(), bot->getLevel(), bot->GetTeam() == ALLIANCE ? "A" : "H", _bgType);
#else
        sLog.outBasic("Bot #%d <%s> (%u %s) joined %s (%s)", bot->GetGUIDLow(), bot->GetName(), bot->getLevel(), bot->GetTeam() == ALLIANCE ? "A" : "H", isArena ? "Arena" : "BG", _bgType);
#endif
        bot->Unmount();

        WorldPacket packet(CMSG_BATTLEFIELD_PORT, 20);
#ifdef MANGOSBOT_ZERO
        packet << mapId << action;
#else
        packet << type << unk2 << (uint32)_bgTypeId << unk << action;
#endif
#ifdef MANGOS
        bot->GetSession()->HandleBattleFieldPortOpcode(packet);
#endif
#ifdef CMANGOS
        bot->GetSession()->HandleBattlefieldPortOpcode(packet);
#endif

        ai->ResetStrategies(false);
        //ai->ChangeStrategy("-bg,-rpg,-travel,-grind", BOT_STATE_NON_COMBAT);
        ai::PositionMap& posMap = context->GetValue<ai::PositionMap&>("position")->Get();
        ai::PositionEntry pos = context->GetValue<ai::PositionMap&>("position")->Get()["bg objective"];
        pos.Reset();
        posMap["bg objective"] = pos;

        return true;
    }

    if (statusid == STATUS_IN_PROGRESS) // placeholder for Leave BG if it takes too long
    {
        return true;
    }
    return true;
}

bool BGStatusCheckAction::Execute(Event event)
{
    if (bot->IsBeingTeleported())
        return false;

    if (!bot->InBattleGroundQueue())
        return false;

    if (bot->InBattleGround())
        return false;

    WorldPacket packet(CMSG_BATTLEFIELD_STATUS);
    bot->GetSession()->HandleBattlefieldStatusOpcode(packet);
    return true;
}
