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
#ifndef MANGOSBOT_ZERO
#ifdef CMANGOS
#include "Arena/ArenaTeam.h"
#endif
#ifdef MANGOS
#include "ArenaTeam.h"
#endif
#endif

using namespace ai;


bool BGJoinAction::Execute(Event event)
{
    uint32 queueType = AI_VALUE(uint32, "bg type");
    if (!queueType) // force join to fill bg
    {
        if (bgList.empty())
            return false;

        BattleGroundQueueTypeId queueTypeId = (BattleGroundQueueTypeId)bgList[urand(0, bgList.size() - 1)];
        BattleGroundTypeId bgTypeId = sServerFacade.BgTemplateId(queueTypeId);
        BattleGroundBracketId bracketId;
        bool isArena = false;
        bool isRated = false;

        BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
        if (!bg)
            return false;

#ifdef MANGOSBOT_TWO
        uint32 mapId = bg->GetMapId();
        PvPDifficultyEntry const* pvpDiff = GetBattlegroundBracketByLevel(mapId, bot->getLevel());
        if (!pvpDiff)
            return false;

        bracketId = pvpDiff->GetBracketId();
#else
        bracketId = bot->GetBattleGroundBracketIdFromLevel(bgTypeId);
#endif

        // Find BattleMaster by Entry
        //uint32 BmEntry = sRandomPlayerbotMgr.GetBattleMasterEntry(bot, bgTypeId, true);
        //if (!BmEntry)
        //{
        //    sLog.outError("Bot #%d <%s> could not find Battlemaster for %d", bot->GetGUIDLow(), bot->GetName(), bgTypeId);
        //    return false;
        //}

#ifndef MANGOSBOT_ZERO
        if (ArenaType type = sServerFacade.BgArenaType(queueTypeId))
        {
            isArena = true;

            vector<uint32>::iterator i = find(ratedList.begin(), ratedList.end(), queueTypeId);
            if (i != ratedList.end())
                isRated = true;

            if (isRated && !gatherArenaTeam(type))
                return false;

            ai->GetAiObjectContext()->GetValue<uint32>("arena type")->Set(isRated);
        }
#endif

        // set bg type and bm guid
        //ai->GetAiObjectContext()->GetValue<ObjectGuid>("bg master")->Set(BmGuid);
        ai->GetAiObjectContext()->GetValue<uint32>("bg type")->Set(queueTypeId);
        queueType = queueTypeId;
    }

   return JoinQueue(queueType);
}

#ifndef MANGOSBOT_ZERO
bool BGJoinAction::gatherArenaTeam(ArenaType type)
{
    ArenaTeam* arenateam = nullptr;
    for (uint32 arena_slot = 0; arena_slot < MAX_ARENA_SLOT; ++arena_slot)
    {
        ArenaTeam* temp = sObjectMgr.GetArenaTeamById(bot->GetArenaTeamId(arena_slot));
        if (!temp)
            continue;

        if (temp->GetCaptainGuid() != bot->GetObjectGuid())
            continue;

        if (temp->GetType() != type)
            continue;

        arenateam = temp;
    }
    if (!arenateam)
        return false;

    vector<uint32> members;

    // search for arena team members and make them online
    for (ArenaTeam::MemberList::iterator itr = arenateam->GetMembers().begin(); itr != arenateam->GetMembers().end(); ++itr)
    {
        bool offline = false;
        Player* member = sObjectMgr.GetPlayer(itr->guid);
        if (!member)
        {
            offline = true;
        }
        //if (!member && !sObjectMgr.GetPlayerAccountIdByGUID(itr->guid))
        //    continue;

        if (offline)
            sRandomPlayerbotMgr.AddPlayerBot(itr->guid.GetRawValue(), 0);

        if (member)
        {
            if (member->GetGroup() && !member->GetGroup()->IsLeader(bot->GetObjectGuid()))
                continue;

            if (member->IsInCombat())
                continue;

            if (member->GetObjectGuid() == bot->GetObjectGuid())
                continue;

            if (!member->GetPlayerbotAI())
                continue;

            member->GetPlayerbotAI()->Reset();
        }

        if (member)
            members.push_back(member->GetGUIDLow());
    }

    if (!members.size() || (int)members.size() < (int)(arenateam->GetType() - 1))
    {
        sLog.outBasic("Team #%d <%s> has not enough members for match", arenateam->GetId(), arenateam->GetName());
        return false;
    }

#ifndef MANGOSBOT_TWO
    Group* group = new Group();
#else
    Group* group = new Group(GROUPTYPE_NORMAL);
#endif
    uint32 count = 1;
    group->Create(bot->GetObjectGuid(), bot->GetName());
    for (auto i = begin(members); i != end(members); ++i)
    {
        if (*i == bot->GetGUIDLow())
            continue;

        if (count >= (int)arenateam->GetType())
            break;

        Player* member = sObjectMgr.GetPlayer(ObjectGuid(HIGHGUID_PLAYER, *i));
        if (!member)
            continue;

        if (member->getLevel() < 70)
            continue;

        if (!group->AddMember(ObjectGuid(HIGHGUID_PLAYER, *i), member->GetName()))
            continue;

        if (!member->GetPlayerbotAI())
            continue;

        member->GetPlayerbotAI()->Reset();

        member->TeleportTo(bot->GetMapId(), bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(), 0);

        sLog.outBasic("Bot #%d <%s>: member of <%s>", member->GetGUIDLow(), member->GetName(), arenateam->GetName());

        count++;
    }

    if (group && group->GetMembersCount() == (uint32)arenateam->GetType())
    {
        sLog.outBasic("Team #%d <%s> is ready for match", arenateam->GetId(), arenateam->GetName());
        return true;
    }
    else
    {
        sLog.outBasic("Team #%d <%s> is not ready for match", arenateam->GetId(), arenateam->GetName());
        group->Disband();
    }
    return false;
}
#endif

bool BGJoinAction::canJoinBg(BattleGroundQueueTypeId queueTypeId, BattleGroundBracketId bracketId)
{
    // check if bot can join this bg/bracket

    BattleGroundTypeId bgTypeId = sServerFacade.BgTemplateId(queueTypeId);

    // check if already in queue
    if (bot->InBattleGroundQueueForBattleGroundQueueType(queueTypeId))
        return false;

    // check too low/high level
    if (!bot->GetBGAccessByLevel(bgTypeId))
        return false;

    // check bracket
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
    // check if bot should join (queue has real players)

    BattleGroundTypeId bgTypeId = sServerFacade.BgTemplateId(queueTypeId);
    BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
    if (!bg)
        return false;

    bool isArena = false;
    bool isRated = false;

#ifndef MANGOSBOT_ZERO
    ArenaType type = sServerFacade.BgArenaType(queueTypeId);
    if (type != ARENA_TYPE_NONE)
        isArena = true;
#endif
    bool hasPlayers = (sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][0] + sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][1]) > 0;
    if (!hasPlayers)
        return false;

    uint32 BracketSize = bg->GetMaxPlayers();
    uint32 TeamSize = bg->GetMaxPlayersPerTeam();

    uint32 ACount = sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][0] + sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][0];
    uint32 HCount = sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][1] + sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][1];

    uint32 BgCount = ACount + HCount;
    uint32 SCount, RCount = 0;

    uint32 TeamId = bot->GetTeam() == ALLIANCE ? 0 : 1;

#ifndef MANGOSBOT_ZERO
    if (isArena)
    {
        uint32 rated_players = sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][1];
        if (rated_players)
        {
            isRated = true;
        }
        isArena = true;
        BracketSize = (uint32)(type * 2);
        TeamSize = type;
        ACount = sRandomPlayerbotMgr.ArenaBots[queueTypeId][bracketId][isRated][0];
        HCount = sRandomPlayerbotMgr.ArenaBots[queueTypeId][bracketId][isRated][1];
        BgCount = sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][isRated] + sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][isRated];
        SCount = sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][0] + sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][0];
        RCount = sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][1] + sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][1];
    }

    // do not try if not a captain of arena team

    if (isRated)
    {
        if (!sObjectMgr.GetArenaTeamByCaptain(bot->GetObjectGuid()))
            return false;

        // check if bot has correct team
        ArenaTeam* arenateam = nullptr;
        for (uint32 arena_slot = 0; arena_slot < MAX_ARENA_SLOT; ++arena_slot)
        {
            ArenaTeam* temp = sObjectMgr.GetArenaTeamById(bot->GetArenaTeamId(arena_slot));
            if (!temp)
                continue;

            if (temp->GetType() != type)
                continue;

            arenateam = temp;
        }

        if (!arenateam)
            return false;

        ratedList.push_back(queueTypeId);
    }
#endif

    // hack fix crash in queue remove event
    if (!isRated && bot->GetGroup())
        return false;

    bool needBots = sRandomPlayerbotMgr.NeedBots[queueTypeId][bracketId][isArena ? isRated : GetTeamIndexByTeamId(bot->GetTeam())];

    // add more bots if players are not invited or 1st BG instance is full
    if (needBots/* || (hasPlayers && BgCount > BracketSize && (BgCount % BracketSize) != 0)*/)
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

    // can't queue in BG
    if (bot->InBattleGround())
        return false;

    // do not try right after login
    if ((time(0) - bot->GetInGameTime()) < 30)
        return false;

    // check level
    if (bot->getLevel() < 10)
        return false;

    // do not try if with player master or in combat/group
    if (bot->GetPlayerbotAI()->HasActivePlayerMaster())
        return false;

    //if (bot->GetGroup())
    //    return false;

    if (bot->IsInCombat())
        return false;

    // check Deserter debuff
    if (!bot->CanJoinToBattleground())
        return false;

    // check if has free queue slots
    if (!bot->HasFreeBattleGroundQueueId())
        return false;

    // do not try if in dungeon
    //Map* map = bot->GetMap();
    //if (map && map->Instanceable())
    //    return false;

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

    // check if already in queue
    if (bot->InBattleGroundQueueForBattleGroundQueueType(queueTypeId))
        return false;

    // check bg req level
    if (!bot->GetBGAccessByLevel(bgTypeId))
        return false;

    // get BattleMaster unit
    // Find BattleMaster by Entry
    /*uint32 BmEntry = sRandomPlayerbotMgr.GetBattleMasterEntry(bot, bgTypeId, true);
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
    }*/

   // get BG MapId
#ifdef MANGOSBOT_ZERO
   uint32 mapId = GetBattleGrounMapIdByTypeId(bgTypeId);
#else
   uint32 bgTypeId_ = bgTypeId;
#endif
   uint32 instanceId = 0; // 0 = First Available
   uint8 joinAsGroup = bot->GetGroup() && bot->GetGroup()->IsLeader(bot->GetObjectGuid());
   bool isPremade = false;
   bool isArena = false;
   bool isRated = false;
   uint8 arenaslot = 0;
   uint8 asGroup = false;
   string _bgType;

// check if arena
#ifndef MANGOSBOT_ZERO
   ArenaType arenaType = sServerFacade.BgArenaType(queueTypeId);
   if (arenaType != ARENA_TYPE_NONE)
       isArena = true;
#endif

   // get battlemaster
   Unit* unit = ai->GetUnit(AI_VALUE2(CreatureDataPair const*, "bg master", bgTypeId));
#ifndef MANGOSBOT_TWO
   if (!unit)
#else
   if (!unit && isArena)
#endif
   {
       sLog.outError("Bot %d could not find Battlemaster to join", bot->GetGUIDLow());
       return false;
   }
// in wotlk only arena requires battlemaster guid
#ifndef MANGOSBOT_TWO
   ObjectGuid guid = unit->GetObjectGuid();
#else
   ObjectGuid guid = isArena ? unit->GetObjectGuid() : bot->GetObjectGuid();
#endif

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
   if (isArena)
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
   else if (!joinAsGroup)
       sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][TeamId]++;
   else
       sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][TeamId] += bot->GetGroup()->GetMembersCount();

   ai->GetAiObjectContext()->GetValue<uint32>("bg type")->Set(0);

   WorldPacket packet(CMSG_BATTLEMASTER_JOIN, 20);
#ifdef MANGOSBOT_ZERO
   packet << unit->GetObjectGuid() << mapId << instanceId << joinAsGroup;
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

   bot->GetSession()->HandleBattlemasterJoinOpcode(packet);
   return true;
}

bool FreeBGJoinAction::shouldJoinBg(BattleGroundQueueTypeId queueTypeId, BattleGroundBracketId bracketId)
{
    BattleGroundTypeId bgTypeId = sServerFacade.BgTemplateId(queueTypeId);
    BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
    if (!bg)
        return false;

    bool isArena = false;
    bool isRated = false;

#ifndef MANGOSBOT_ZERO
    ArenaType type = sServerFacade.BgArenaType(queueTypeId);
    if (type != ARENA_TYPE_NONE)
        isArena = true;
#endif

    uint32 BracketSize = bg->GetMaxPlayers();
    uint32 TeamSize = bg->GetMaxPlayersPerTeam();

    uint32 ACount = sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][0] + sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][0];
    uint32 HCount = sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][1] + sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][1];

    uint32 BgCount = ACount + HCount;
    uint32 SCount, RCount = 0;

    uint32 TeamId = bot->GetTeam() == ALLIANCE ? 0 : 1;

#ifndef MANGOSBOT_ZERO
    if (isArena)
    {
        uint32 rated_players = sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][1];
        if (rated_players)
        {
            isRated = true;
        }
        isArena = true;
        BracketSize = (uint32)(type * 2);
        TeamSize = type;
        ACount = sRandomPlayerbotMgr.ArenaBots[queueTypeId][bracketId][isRated][0];
        HCount = sRandomPlayerbotMgr.ArenaBots[queueTypeId][bracketId][isRated][1];
        BgCount = sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][isRated] + sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][isRated];
        SCount = sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][0] + sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][0];
        RCount = sRandomPlayerbotMgr.BgBots[queueTypeId][bracketId][1] + sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][1];
    }

    // do not try if not a captain of arena team

    if (isRated)
    {
        if (!sObjectMgr.GetArenaTeamByCaptain(bot->GetObjectGuid()))
            return false;

        // check if bot has correct team
        ArenaTeam* arenateam = nullptr;
        for (uint32 arena_slot = 0; arena_slot < MAX_ARENA_SLOT; ++arena_slot)
        {
            ArenaTeam* temp = sObjectMgr.GetArenaTeamById(bot->GetArenaTeamId(arena_slot));
            if (!temp)
                continue;

            if (temp->GetType() != type)
                continue;

            arenateam = temp;
        }

        if (!arenateam)
            return false;

        ratedList.push_back(queueTypeId);
    }
#endif

    // hack fix crash in queue remove event
    if (!isRated && bot->GetGroup())
        return false;

    bool needBots = sRandomPlayerbotMgr.NeedBots[queueTypeId][bracketId][isArena ? isRated : GetTeamIndexByTeamId(bot->GetTeam())];

    // add more bots if players are not invited or 1st BG instance is full
    if (needBots/* || (hasPlayers && BgCount > BracketSize && (BgCount % BracketSize) != 0)*/)
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
    uint8 isRated;

#ifndef MANGOSBOT_ZERO
    uint64 arenatype;
    uint64 arenaByte;
    uint8 arenaTeam;
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
        p >> mapId;                         // map id
#ifdef MANGOSBOT_TWO
        p >> unk0;
#endif
        p >> Time1;                            // time to remove from queue, milliseconds
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
    BattleGroundBracketId bracketId;
#ifdef CMANGOS
#ifdef MANGOSBOT_TWO
    BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(_bgTypeId);
    PvPDifficultyEntry const* pvpDiff = GetBattlegroundBracketByLevel(mapId, bot->getLevel());
    if (pvpDiff)
        bracketId = pvpDiff->GetBracketId();

#else
    bracketId = bot->GetBattleGroundBracketIdFromLevel(_bgTypeId);
#endif
#endif

    bool isArena = false;
    uint8 type = false;
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
        ai->ResetStrategies(!IsRandomBot);
        ai->GetAiObjectContext()->GetValue<uint32>("bg type")->Set(NULL);
        ai->GetAiObjectContext()->GetValue<uint32>("bg role")->Set(NULL);
        ai->GetAiObjectContext()->GetValue<uint32>("arena type")->Set(NULL);
        ai::PositionMap& posMap = context->GetValue<ai::PositionMap&>("position")->Get();
        ai::PositionEntry pos = context->GetValue<ai::PositionMap&>("position")->Get()["bg objective"];
        pos.Reset();
        posMap["bg objective"] = pos;
    }
    if (statusid == STATUS_WAIT_QUEUE) //bot is in queue
    {
        // temp fix for crash
        return true;

        BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(_bgTypeId);
        if (!bg)
            return false;

        bool leaveQ = false;
        uint32 timer;
        if (isArena)
            timer = TIME_TO_AUTOREMOVE;
        else
            timer = TIME_TO_AUTOREMOVE + 1000 * (bg->GetMaxPlayers() * 4);

        if (Time2 > timer && isArena) // disabled for BG
            leaveQ = true;

        if (leaveQ && ((bot->GetGroup() && bot->GetGroup()->IsLeader(bot->GetObjectGuid())) || !(bot->GetGroup() || ai->GetMaster())))
        {
            uint32 TeamId = bot->GetTeam() == ALLIANCE ? 0 : 1;
            bool realPlayers = sRandomPlayerbotMgr.BgPlayers[queueTypeId][bracketId][TeamId];
            if (realPlayers)
                return false;
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

        // bg started so players should get invites by now
        sRandomPlayerbotMgr.NeedBots[queueTypeId][bracketId][isArena ? isRated : GetTeamIndexByTeamId(bot->GetTeam())] = false;

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
        context->GetValue<uint32>("bg role")->Set(urand(0, 9));
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

    WorldPacket packet(CMSG_BATTLEFIELD_STATUS);
    bot->GetSession()->HandleBattlefieldStatusOpcode(packet);
    return true;
}
bool BGStatusCheckAction::isUseful()
{
    return bot->InBattleGroundQueue();
}