#include "Config/Config.h"
#include "../botpch.h"
#include "playerbot.h"
#include "PlayerbotAIConfig.h"
#include "PlayerbotFactory.h"
#include "AccountMgr.h"
#include "ObjectMgr.h"
#include "DatabaseEnv.h"
#include "PlayerbotAI.h"
#include "Player.h"
#include "AiFactory.h"
#include "GuildTaskMgr.h"
#include "PlayerbotCommandServer.h"

#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "FleeManager.h"
#include "ServerFacade.h"

#include "BattleGround.h"
#include "BattleGroundMgr.h"

#include "World/WorldState.h"

#ifndef MANGOSBOT_ZERO
#ifdef CMANGOS
#include "Arena/ArenaTeam.h"
#endif
#ifdef MANGOS
#include "ArenaTeam.h"
#endif
#endif

#include "TravelMgr.h"

using namespace ai;
using namespace MaNGOS;

INSTANTIATE_SINGLETON_1(RandomPlayerbotMgr);

#ifdef CMANGOS
#include <boost/thread/thread.hpp>
#endif

#ifdef MANGOS
class PrintStatsThread: public ACE_Task <ACE_MT_SYNCH>
{
public:
    int svc(void) { sRandomPlayerbotMgr.PrintStats(); return 0; }
};
#endif
#ifdef CMANGOS
void PrintStatsThread()
{
    sRandomPlayerbotMgr.PrintStats();
}
#endif

void activatePrintStatsThread()
{
#ifdef MANGOS
    PrintStatsThread *thread = new PrintStatsThread();
    thread->activate();
#endif
#ifdef CMANGOS
    boost::thread t(PrintStatsThread);
    t.detach();
#endif
}

#ifdef MANGOS
class CheckBgQueueThread : public ACE_Task <ACE_MT_SYNCH>
{
public:
    int svc(void) { sRandomPlayerbotMgr.CheckBgQueue(); return 0; }
};
#endif
#ifdef CMANGOS
void CheckBgQueueThread()
{
    sRandomPlayerbotMgr.CheckBgQueue();
}
#endif

void activateCheckBgQueueThread()
{
#ifdef MANGOS
    CheckBgQueueThread *thread = new CheckBgQueueThread();
    thread->activate();
#endif
#ifdef CMANGOS
    boost::thread t(CheckBgQueueThread);
    t.detach();
#endif
}

#ifdef MANGOS
class CheckLfgQueueThread : public ACE_Task <ACE_MT_SYNCH>
{
public:
    int svc(void) { sRandomPlayerbotMgr.CheckLfgQueue(); return 0; }
};
#endif
#ifdef CMANGOS
void CheckLfgQueueThread()
{
    sRandomPlayerbotMgr.CheckLfgQueue();
}
#endif

void activateCheckLfgQueueThread()
{
#ifdef MANGOS
    CheckLfgQueueThread *thread = new CheckLfgQueueThread();
    thread->activate();
#endif
#ifdef CMANGOS
    boost::thread t(CheckLfgQueueThread);
    t.detach();
#endif
}

#ifdef MANGOS
class CheckPlayersThread : public ACE_Task <ACE_MT_SYNCH>
{
public:
    int svc(void) { sRandomPlayerbotMgr.CheckPlayers(); return 0; }
};
#endif
#ifdef CMANGOS
void CheckPlayersThread()
{
    sRandomPlayerbotMgr.CheckPlayers();
}
#endif

void activateCheckPlayersThread()
{
#ifdef MANGOS
    CheckPlayersThread *thread = new CheckPlayersThread();
    thread->activate();
#endif
#ifdef CMANGOS
    boost::thread t(CheckPlayersThread);
    t.detach();
#endif
}

RandomPlayerbotMgr::RandomPlayerbotMgr() : PlayerbotHolder(), processTicks(0), loginProgressBar(NULL)
{
    if (sPlayerbotAIConfig.enabled || sPlayerbotAIConfig.randomBotAutologin)
    {
        sPlayerbotCommandServer.Start();
        PrepareTeleportCache();
    }
}

RandomPlayerbotMgr::~RandomPlayerbotMgr()
{
}

int RandomPlayerbotMgr::GetMaxAllowedBotCount()
{
    return GetEventValue(0, "bot_count");
}

void RandomPlayerbotMgr::UpdateAIInternal(uint32 elapsed)
{
    if (!sPlayerbotAIConfig.randomBotAutologin || !sPlayerbotAIConfig.enabled)
        return;

    int maxAllowedBotCount = GetEventValue(0, "bot_count");
    if (!maxAllowedBotCount || (maxAllowedBotCount < sPlayerbotAIConfig.minRandomBots || maxAllowedBotCount > sPlayerbotAIConfig.maxRandomBots))
    {
        maxAllowedBotCount = urand(sPlayerbotAIConfig.minRandomBots, sPlayerbotAIConfig.maxRandomBots);
        SetEventValue(0, "bot_count", maxAllowedBotCount,
                urand(sPlayerbotAIConfig.randomBotCountChangeMinInterval, sPlayerbotAIConfig.randomBotCountChangeMaxInterval));
    }

   /*if (!loginProgressBar && playerBots.size() + 10 < maxAllowedBotCount)
    {
        sLog.outString("Logging in %d random bots in the background", maxAllowedBotCount);
        loginProgressBar = new BarGoLink(maxAllowedBotCount);
    }*/ // bar goes > 100%

    // Fix possible divide by zero if maxAllowedBotCount is smaller then sPlayerbotAIConfig.randomBotsPerInterval
    uint32 notDiv = 1;
    if (maxAllowedBotCount >  sPlayerbotAIConfig.randomBotsPerInterval)
        notDiv =  maxAllowedBotCount / sPlayerbotAIConfig.randomBotsPerInterval;

    //SetNextCheckDelay((uint32)max(1000, int(2000 * notDiv * sPlayerbotAIConfig.randomBotUpdateInterval) / 1000));

    if (playerBots.size() < int(sPlayerbotAIConfig.minRandomBots / 2))
        SetNextCheckDelay((uint32)max(1000, int(1000 * notDiv * sPlayerbotAIConfig.randomBotUpdateInterval) / 1000));
    else
        SetNextCheckDelay((uint32)max(1000, int(2000 * notDiv * sPlayerbotAIConfig.randomBotUpdateInterval) / 1000));

    list<uint32> bots = GetBots();
    int botCount = bots.size();

    PerformanceMonitorOperation *pmo = sPerformanceMonitor.start(PERF_MON_TOTAL,
            playerBots.size() < maxAllowedBotCount ? "RandomPlayerbotMgr::Login" : "RandomPlayerbotMgr::UpdateAIInternal");

    if (botCount < maxAllowedBotCount)
    {
        AddRandomBots();
    }

    if (sPlayerbotAIConfig.syncLevelWithPlayers && players.size())
    {
        if (time(NULL) > (PlayersCheckTimer + 60))
            activateCheckPlayersThread();
    }

    if (sPlayerbotAIConfig.randomBotJoinLfg && players.size())
    {
        if (time(NULL) > (LfgCheckTimer + 30))
            activateCheckLfgQueueThread();
    }

    if (sPlayerbotAIConfig.randomBotJoinBG && players.size())
    {
        if (time(NULL) > (BgCheckTimer + 30))
            activateCheckBgQueueThread();

        if (BgBotsActive && bgBotsCount < 40)
        {
            for (int i = BG_BRACKET_ID_FIRST; i < MAX_BATTLEGROUND_BRACKETS; ++i)
            {
                for (int j = BATTLEGROUND_QUEUE_AV; j < MAX_BATTLEGROUND_QUEUE_TYPES; ++j)
                {
                    BattleGroundQueueTypeId queueTypeId = BattleGroundQueueTypeId(j);
                    BattleGroundTypeId bgTypeId = sServerFacade.BgTemplateId(queueTypeId);
                    BattleGroundBracketId bracketId = BattleGroundBracketId(i);
                    uint32 bg_players = BgPlayers[queueTypeId][bracketId][0] + BgPlayers[queueTypeId][bracketId][1];
                    uint32 visual_players = VisualBots[queueTypeId][bracketId][0] + VisualBots[queueTypeId][bracketId][1];
                    if (bg_players)
                    {
#ifndef MANGOSBOT_ZERO
                        if (sServerFacade.BgArenaType(queueTypeId))
                        {
                            uint32 rated_players = BgPlayers[queueTypeId][bracketId][1];
                            if (rated_players)
                            {
                                AddBgBot(queueTypeId, bracketId, true);
                                bgBotsCount++;
                                continue;
                            }
                        }
#endif
                        AddBgBot(queueTypeId, bracketId);
                        bgBotsCount++;
                        continue;
                    }
                    if (!visual_players && !bg_players && bgBotsCount < 5)
                    {
                        AddBgBot(queueTypeId, bracketId, false, true);
                        bgBotsCount++;
                    }
                }
            }
        }
    }

    int botProcessed = 0;
    for (list<uint32>::iterator i = bots.begin(); i != bots.end(); ++i)
    {
        uint32 bot = *i;
		if (ProcessBot(bot)) {
			botProcessed++;
		}

        if (playerBots.size() < int(sPlayerbotAIConfig.minRandomBots / 4) && botProcessed >= sPlayerbotAIConfig.randomBotsPerInterval)
            break;
        else if (playerBots.size() < int(sPlayerbotAIConfig.minRandomBots / 2) && botProcessed >= int(sPlayerbotAIConfig.randomBotsPerInterval / 2))
            break;
        else if (playerBots.size() < sPlayerbotAIConfig.minRandomBots && botProcessed >= int(sPlayerbotAIConfig.randomBotsPerInterval / 5))
            break;
        else if (playerBots.size() >= sPlayerbotAIConfig.minRandomBots && botProcessed >= int(sPlayerbotAIConfig.randomBotsPerInterval / 30))
            break;
    }

    if (pmo) pmo->finish();
}

uint32 RandomPlayerbotMgr::AddRandomBots()
{
    set<uint32> bots;

    if (!currentBots.size())
    {
        QueryResult* results = PlayerbotDatabase.PQuery(
            "select `bot` from ai_playerbot_random_bots where event = 'add'");

        if (results)
        {
            do
            {
                Field* fields = results->Fetch();
                uint32 bot = fields[0].GetUInt32();
                bots.insert(bot);
            } while (results->NextRow());
            delete results;
        }
    }
    else
    {
        for (list<uint32>::iterator i = currentBots.begin(); i != currentBots.end(); i++)
        {
            uint32 bot = *i;
            bots.insert(bot);
        }
    }

    vector<uint32> guids;
    int maxAllowedBotCount = GetEventValue(0, "bot_count");
    int maxAllowedNewBotCount = maxAllowedBotCount - currentBots.size();
    for (list<uint32>::iterator i = sPlayerbotAIConfig.randomBotAccounts.begin(); i != sPlayerbotAIConfig.randomBotAccounts.end(); i++)
    {
        uint32 accountId = *i;
        if (!sAccountMgr.GetCharactersCount(accountId))
            continue;

        if (urand(0, 100) > 10)  // more random selection
            continue;

        QueryResult* result = CharacterDatabase.PQuery("SELECT guid, race, name, level FROM characters WHERE account = '%u'", accountId);
        if (!result)
            continue;

        do
        {
            Field* fields = result->Fetch();
            uint32 guid = fields[0].GetUInt32();
            uint8 race = fields[1].GetUInt8();
            std::string name = fields[2].GetString();
            uint32 level = fields[3].GetUInt32();

            if (bots.find(guid) != bots.end())
                continue;
			// Although this code works it cuts the Maximum Bots setting in half. 
			// And, also doesn't see to be any reason to do it.
            //bool alliance = guids.size() % 2 == 0;
            //if (bots.find(guid) == bots.end() &&
            //        ((alliance && IsAlliance(race)) || ((!alliance && !IsAlliance(race))
            //)))
            //{
                guids.push_back(guid);
                uint32 bot = guid;
                SetEventValue(bot, "add", 1, urand(sPlayerbotAIConfig.minRandomBotInWorldTime, sPlayerbotAIConfig.maxRandomBotInWorldTime));
                uint32 randomTime = 60 + urand(sPlayerbotAIConfig.randomBotUpdateInterval, sPlayerbotAIConfig.randomBotUpdateInterval * 3);
                ScheduleRandomize(bot, randomTime);
				SetEventValue(bot, "teleport", 1, sPlayerbotAIConfig.maxRandomBotInWorldTime);
				SetEventValue(bot, "change_strategy", 1, sPlayerbotAIConfig.maxRandomBotInWorldTime);
                SetEventValue(bot, "version", MANGOSBOT_VERSION, sPlayerbotAIConfig.maxRandomBotInWorldTime);
                bots.insert(bot);
                currentBots.push_back(bot);
                sLog.outBasic("Bot #%d %s:%d <%s>: log in", guid, IsAlliance(race) == ALLIANCE ? "A" : "H", level, name);
                if (guids.size() >= min((int)(sPlayerbotAIConfig.randomBotsPerInterval / 4), maxAllowedNewBotCount))
                {
                    delete result;
                    return guids.size();
                }
            //}
        } while (result->NextRow());
        delete result;
    }
    return guids.size();
}

void RandomPlayerbotMgr::LoadBattleMastersCache()
{
    BattleMastersCache.clear();

    sLog.outString("---------------------------------------");
    sLog.outString("          Loading BattleMasters Cache  ");
    sLog.outString("---------------------------------------");
    sLog.outString();

    QueryResult* result = WorldDatabase.Query("SELECT `entry`,`bg_template` FROM `battlemaster_entry`");

    uint32 count = 0;

    if (!result)
    {
        sLog.outString(">> Loaded 0 battlemaster entries - table is empty!");
        sLog.outString();
        return;
    }

    do
    {
        ++count;

        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();
        uint32 bgTypeId = fields[1].GetUInt32();

        CreatureInfo const* bmaster = sObjectMgr.GetCreatureTemplate(entry);
        if (!bmaster)
            continue;

#ifdef MANGOS
        FactionTemplateEntry const* bmFaction = sFactionTemplateStore.LookupEntry(bmaster->FactionAlliance);
#endif
#ifdef CMANGOS
        FactionTemplateEntry const* bmFaction = sFactionTemplateStore.LookupEntry(bmaster->Faction);
#endif
        uint32 bmFactionId = bmFaction->faction;
#ifdef MANGOS
        FactionEntry const* bmParentFaction = sFactionStore.LookupEntry(bmFactionId);
#endif
#ifdef CMANGOS
#ifdef MANGOSBOT_ONE
        FactionEntry const* bmParentFaction = sFactionStore.LookupEntry<FactionEntry>(bmFactionId);
#else
        FactionEntry const* bmParentFaction = sFactionStore.LookupEntry(bmFactionId);
#endif
#endif
        uint32 bmParentTeam = bmParentFaction->team;
        Team bmTeam = TEAM_BOTH_ALLOWED;
        if (bmParentTeam == 891)
            bmTeam = ALLIANCE;
        if (bmFactionId == 189)
            bmTeam = ALLIANCE;
        if (bmParentTeam == 892)
            bmTeam = HORDE;
        if (bmFactionId == 66)
            bmTeam = HORDE;

        BattleMastersCache[bmTeam][BattleGroundTypeId(bgTypeId)].insert(BattleMastersCache[bmTeam][BattleGroundTypeId(bgTypeId)].end(), entry);
        sLog.outDetail("Cached Battmemaster #%d for BG Type %d (%s)", entry, bgTypeId, bmTeam == ALLIANCE ? "Alliance" : bmTeam == HORDE ? "Horde" : "Neutral");

    } while (result->NextRow());

    delete result;

    sLog.outString(">> Loaded %u battlemaster entries", count);
    sLog.outString();
}

void RandomPlayerbotMgr::CheckBgQueue()
{
    if (!BgCheckTimer)
        BgCheckTimer = time(NULL);

    uint32 count = 0;
    uint32 visual_count = 0;
    for (int i = BG_BRACKET_ID_FIRST; i < MAX_BATTLEGROUND_BRACKETS; ++i)
    {
        for (int j = BATTLEGROUND_QUEUE_AV; j < MAX_BATTLEGROUND_QUEUE_TYPES; ++j)
        {
            count += BgPlayers[j][i][0];
            count += BgPlayers[j][i][1];

            visual_count += VisualBots[j][i][0];
            visual_count += VisualBots[j][i][1];

            if (count == 0)
                continue;

            BattleGroundQueueTypeId queueTypeId = BattleGroundQueueTypeId(j);
            BattleGroundBracketId bracketId = BattleGroundBracketId(i);
            if (queueTypeId == BATTLEGROUND_QUEUE_NONE)
                continue;
            BattleGroundTypeId bgTypeId = sServerFacade.BgTemplateId(queueTypeId);
            BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
            if (!bg)
                continue;

            uint32 BracketSize = bg->GetMaxPlayers();
            uint32 TeamSize = bg->GetMaxPlayersPerTeam();

            uint32 ACount = BgBots[queueTypeId][bracketId][0] + BgPlayers[queueTypeId][bracketId][0];
            uint32 HCount = BgBots[queueTypeId][bracketId][1] + BgPlayers[queueTypeId][bracketId][1];

#ifndef MANGOSBOT_ZERO
            uint32 SCount, RCount;
            if (ArenaType type = sServerFacade.BgArenaType(queueTypeId))
            {
                SCount = BgBots[queueTypeId][bracketId][0] + BgPlayers[queueTypeId][bracketId][0];
                RCount = BgBots[queueTypeId][bracketId][1] + BgPlayers[queueTypeId][bracketId][1];

                BracketSize = type * 2;
                TeamSize = type;

                if ((SCount >= BracketSize && !RCount) || (RCount >= BracketSize && !SCount))
                    continue;
            }
#endif
            uint32 BgCount = ACount + HCount;

            if (BgCount >= BracketSize)
                continue;
        }
    }

    int check_time = count > 0 ? 60 : 30;

    if (time(NULL) < (BgCheckTimer + check_time))
    {
        BgBotsActive = (count > 0 || visual_count < (MAX_BATTLEGROUND_BRACKETS * 5));
        return;
    }
    else
    {
        BgCheckTimer = time(NULL);
        bgBotsCount = 0;
    }

    sLog.outBasic("Checking BG Queue...");

    for (int i = BG_BRACKET_ID_FIRST; i < MAX_BATTLEGROUND_BRACKETS; ++i)
    {
        for (int j = BATTLEGROUND_QUEUE_AV; j < MAX_BATTLEGROUND_QUEUE_TYPES; ++j)
        {
            BgPlayers[j][i][0] = 0;
            BgPlayers[j][i][1] = 0;
            BgBots[j][i][0] = 0;
            BgBots[j][i][1] = 0;
            ArenaBots[j][i][0][0] = 0;
            ArenaBots[j][i][0][1] = 0;
            ArenaBots[j][i][1][0] = 0;
            ArenaBots[j][i][1][1] = 0;
        }
    }

    for (vector<Player*>::iterator i = players.begin(); i != players.end(); ++i)
    {
        Player* player = *i;

        if (!player->InBattleGroundQueue())
            continue;

        if (player->InBattleGround() && player->GetBattleGround()->GetStatus() == STATUS_WAIT_LEAVE)
            continue;

        uint32 TeamId = player->GetTeam() == ALLIANCE ? 0 : 1;
        BattleGroundQueueTypeId queueTypeId = player->GetBattleGroundQueueTypeId(0);
        if (queueTypeId == BATTLEGROUND_QUEUE_NONE)
            continue;

        BattleGroundTypeId bgTypeId = sServerFacade.BgTemplateId(queueTypeId);
#ifdef MANGOS
        BattleGroundBracketId bracketId = player->GetBattleGroundBracketIdFromLevel(bgTypeId);
#endif
#ifdef CMANGOS
#ifdef MANGOSBOT_TWO
        BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
        uint32 mapId = bg->GetMapId();
        PvPDifficultyEntry const* pvpDiff = GetBattlegroundBracketByLevel(mapId, player->getLevel());
        if (!pvpDiff)
            continue;

        BattleGroundBracketId bracketId = pvpDiff->GetBracketId();
#else
        BattleGroundBracketId bracketId = player->GetBattleGroundBracketIdFromLevel(bgTypeId);
#endif
#endif
#ifndef MANGOSBOT_ZERO
        ArenaType arenaType = sServerFacade.BgArenaType(queueTypeId);
        if (arenaType != ARENA_TYPE_NONE)
        {
#ifdef MANGOS
            BattleGroundQueue& bgQueue = sServerFacade.bgQueue(queueTypeId);
            GroupQueueInfo ginfo;
            uint32 tempT = TeamId;

            if (bgQueue.GetPlayerGroupInfoData(player->GetObjectGuid(), &ginfo))
            {
                if (ginfo.IsRated)
                {
                    for (uint32 arena_slot = 0; arena_slot < MAX_ARENA_SLOT; ++arena_slot)
                    {
                        uint32 arena_team_id = player->GetArenaTeamId(arena_slot);
                        ArenaTeam* arenateam = sObjectMgr.GetArenaTeamById(arena_team_id);
                        if (!arenateam)
                            continue;
                        if (arenateam->GetType() != arenaType)
                            continue;

                        Rating[queueTypeId][bracketId][1] = arenateam->GetRating();
                    }
                }
                TeamId = ginfo.IsRated ? 1 : 0;
            }
            if (player->InArena())
            {
                if (player->GetBattleGround()->isRated() && (ginfo.IsRated && ginfo.ArenaTeamId && ginfo.ArenaTeamRating && ginfo.OpponentsTeamRating))
                    TeamId = 1;
                else
                    TeamId = 0;
            }
#endif
#ifdef CMANGOS
            BattleGroundQueue& bgQueue = sServerFacade.bgQueue(queueTypeId);
            GroupQueueInfo ginfo;
            uint32 tempT = TeamId;

            if (bgQueue.GetPlayerGroupInfoData(player->GetObjectGuid(), &ginfo))
            {
                if (ginfo.isRated)
                {
                    for (uint32 arena_slot = 0; arena_slot < MAX_ARENA_SLOT; ++arena_slot)
                    {
                        uint32 arena_team_id = player->GetArenaTeamId(arena_slot);
                        ArenaTeam* arenateam = sObjectMgr.GetArenaTeamById(arena_team_id);
                        if (!arenateam)
                            continue;
                        if (arenateam->GetType() != arenaType)
                            continue;

                        Rating[queueTypeId][bracketId][1] = arenateam->GetRating();
                    }
                }
                TeamId = ginfo.isRated ? 1 : 0;
            }
            if (player->InArena())
            {
                if (player->GetBattleGround()->IsRated() && (ginfo.isRated && ginfo.arenaTeamId && ginfo.arenaTeamRating && ginfo.opponentsTeamRating))
                    TeamId = 1;
                else
                    TeamId = 0;
            }
#endif
            ArenaBots[queueTypeId][bracketId][TeamId][tempT]++;
        }
#endif

        if (player->GetPlayerbotAI())
            BgBots[queueTypeId][bracketId][TeamId]++;
        else
            BgPlayers[queueTypeId][bracketId][TeamId]++;
    }

    for (PlayerBotMap::iterator i = playerBots.begin(); i != playerBots.end(); ++i)
    {
        Player* bot = i->second;

        if (!bot->InBattleGroundQueue())
            continue;

        if (!IsRandomBot(bot->GetGUIDLow()))
            continue;

        if (bot->GetBattleGround() && bot->GetBattleGround()->GetStatus() == STATUS_WAIT_LEAVE)
            continue;

        uint32 TeamId = bot->GetTeam() == ALLIANCE ? 0 : 1;
        BattleGroundQueueTypeId queueTypeId = bot->GetBattleGroundQueueTypeId(0);
        if (queueTypeId == BATTLEGROUND_QUEUE_NONE)
            continue;
        BattleGroundTypeId bgTypeId = sServerFacade.BgTemplateId(queueTypeId);
#ifdef MANGOS
        BattleGroundBracketId bracketId = bot->GetBattleGroundBracketIdFromLevel(bgTypeId);
#endif
#ifdef CMANGOS
#ifdef MANGOSBOT_TWO
        BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
        uint32 mapId = bg->GetMapId();
        PvPDifficultyEntry const* pvpDiff = GetBattlegroundBracketByLevel(mapId, bot->getLevel());
        if (!pvpDiff)
            continue;

        BattleGroundBracketId bracketId = pvpDiff->GetBracketId();
#else
        BattleGroundBracketId bracketId = bot->GetBattleGroundBracketIdFromLevel(bgTypeId);
#endif
#endif

#ifndef MANGOSBOT_ZERO
        ArenaType arenaType = sServerFacade.BgArenaType(queueTypeId);
        if (arenaType != ARENA_TYPE_NONE)
        {
            BattleGroundQueue& bgQueue = sServerFacade.bgQueue(queueTypeId);
            GroupQueueInfo ginfo;
            uint32 tempT = TeamId;
#ifdef MANGOS
            if (bgQueue.GetPlayerGroupInfoData(bot->GetObjectGuid(), &ginfo))
            {
                TeamId = ginfo.IsRated ? 1 : 0;
            }
            if (bot->InArena())
            {
                if (bot->GetBattleGround()->isRated() && (ginfo.IsRated && ginfo.ArenaTeamId && ginfo.ArenaTeamRating && ginfo.OpponentsTeamRating))
                    TeamId = 1;
                else
                    TeamId = 0;
            }
#endif
#ifdef CMANGOS
            if (bgQueue.GetPlayerGroupInfoData(bot->GetObjectGuid(), &ginfo))
            {
                TeamId = ginfo.isRated ? 1 : 0;
            }
            if (bot->InArena())
            {
                if (bot->GetBattleGround()->IsRated() && (ginfo.isRated && ginfo.arenaTeamId && ginfo.arenaTeamRating && ginfo.opponentsTeamRating))
                    TeamId = 1;
                else
                    TeamId = 0;
            }
#endif
            ArenaBots[queueTypeId][bracketId][TeamId][tempT]++;
        }
#endif
        BgBots[queueTypeId][bracketId][TeamId]++;
    }

    BgBotsActive = false;
    for (int i = BG_BRACKET_ID_FIRST; i < MAX_BATTLEGROUND_BRACKETS; ++i)
    {
        for (int j = BATTLEGROUND_QUEUE_AV; j < MAX_BATTLEGROUND_QUEUE_TYPES; ++j)
        {
            BattleGroundQueueTypeId queueTypeId = BattleGroundQueueTypeId(j);

            if ((BgPlayers[j][i][0] + BgBots[j][i][0] + BgPlayers[j][i][1] + BgBots[j][i][1]) == 0)
                continue;

#ifndef MANGOSBOT_ZERO
            if (sServerFacade.BgArenaType(queueTypeId))
            {
                ArenaType type = sServerFacade.BgArenaType(queueTypeId);
                sLog.outBasic("ARENA:%s %s: P (Skirmish:%d, Rated:%d) B (Skirmish:%d, Rated:%d) Total (Skirmish:%d Rated:%d)",
                    type == ARENA_TYPE_2v2 ? "2v2" : type == ARENA_TYPE_3v3 ? "3v3" : "5v5",
                    i == 0 ? "10-19" : i == 1 ? "20-29" : i == 2 ? "30-39" : i == 3 ? "40-49" : i == 4 ? "50-59" : (i == 5 && MAX_BATTLEGROUND_BRACKETS == 6) ? "60" : (i == 5 && MAX_BATTLEGROUND_BRACKETS == 7) ? "60-69" : i == 6 ? (i == 6 && MAX_BATTLEGROUND_BRACKETS == 16) ? "70-79" : "70" : "80",
                    BgPlayers[j][i][0],
                    BgPlayers[j][i][1],
                    BgBots[j][i][0],
                    BgBots[j][i][1],
                    BgPlayers[j][i][0] + BgBots[j][i][0],
                    BgPlayers[j][i][1] + BgBots[j][i][1]
                );

                BgBotsActive = true;
                continue;
            }
#endif
            BattleGroundTypeId bgTypeId = sServerFacade.BgTemplateId(queueTypeId);
            sLog.outBasic("BG:%s %s: P (%d:%d) B (%d:%d) Total (A:%d H:%d)",
                bgTypeId == BATTLEGROUND_AV ? "AV" : bgTypeId == BATTLEGROUND_WS ? "WSG" : bgTypeId == BATTLEGROUND_AB ? "AB" : "EoTS",
                i == 0 ? "10-19" : i == 1 ? "20-29" : i == 2 ? "30-39" : i == 3 ? "40-49" : i == 4 ? "50-59" : (i == 5 && MAX_BATTLEGROUND_BRACKETS == 6) ? "60" : (i == 5 && MAX_BATTLEGROUND_BRACKETS == 7) ? "60-69" : i == 6 ? (i == 6 && MAX_BATTLEGROUND_BRACKETS == 16) ? "70-79" : "70" : "80",
                BgPlayers[j][i][0],
                BgPlayers[j][i][1],
                BgBots[j][i][0],
                BgBots[j][i][1],
                BgPlayers[j][i][0] + BgBots[j][i][0],
                BgPlayers[j][i][1] + BgBots[j][i][1]
            );

            BgBotsActive = true;
        }
    }

    sLog.outBasic("BG Queue check finished");
    return;
}

void RandomPlayerbotMgr::CheckLfgQueue()
{
    if (!LfgCheckTimer || time(NULL) > (LfgCheckTimer + 30))
        LfgCheckTimer = time(NULL);

    sLog.outBasic("Checking LFG Queue...");

    // Clear LFG list
    LfgDungeons[HORDE].clear();
    LfgDungeons[ALLIANCE].clear();

    for (vector<Player*>::iterator i = players.begin(); i != players.end(); ++i)
    {
        Player* player = *i;

        bool isLFG = false;

#ifdef MANGOSBOT_ZERO
        Group* group = player->GetGroup();
        if (group)
        {
            if (sLFGMgr.IsGroupInQueue(group->GetId()))
            {
                isLFG = true;
                LFGGroupQueueInfo lfgInfo;
                sLFGMgr.GetGroupQueueInfo(&lfgInfo, group->GetId());
                LfgDungeons[player->GetTeam()].push_back(lfgInfo.areaId);
            }
        }
        else
        {
            if (sLFGMgr.IsPlayerInQueue(player->GetObjectGuid()))
            {
                isLFG = true;
                LFGPlayerQueueInfo lfgInfo;
                sLFGMgr.GetPlayerQueueInfo(&lfgInfo, player->GetObjectGuid());

                LfgDungeons[player->GetTeam()].push_back(lfgInfo.areaId);
            }
        }
#endif
#ifdef MANGOSBOT_TWO
        Group* group = player->GetGroup();
        if (group)
        {
            if (sLFGMgr.GetQueueInfo(group->GetObjectGuid()))
            {
                isLFG = true;
                LFGGroupState* gState = sLFGMgr.GetLFGGroupState(group->GetObjectGuid());
                if (gState->GetState() != LFG_STATE_NONE && gState->GetState() < LFG_STATE_DUNGEON)
                {
                    LFGDungeonSet const* dList = gState->GetDungeons();
                    for (LFGDungeonSet::const_iterator itr = dList->begin(); itr != dList->end(); ++itr)
                    {
                        LFGDungeonEntry const* dungeon = *itr;

                        if (!dungeon)
                            continue;

                        LfgDungeons[player->GetTeam()].push_back(dungeon->ID);
                    }
                }
            }
        }
        else
        {
            if (sLFGMgr.GetQueueInfo(player->GetObjectGuid()))
            {
                isLFG = true;
                LFGPlayerState* pState = sLFGMgr.GetLFGPlayerState(player->GetObjectGuid());
                if (pState->GetState() != LFG_STATE_NONE && pState->GetState() < LFG_STATE_DUNGEON)
                {
                    LFGDungeonSet const* dList = pState->GetDungeons();
                    for (LFGDungeonSet::const_iterator itr = dList->begin(); itr != dList->end(); ++itr)
                    {
                        LFGDungeonEntry const* dungeon = *itr;

                        if (!dungeon)
                            continue;

                        LfgDungeons[player->GetTeam()].push_back(dungeon->ID);
                    }
                }
            }
        }
#endif
    }
    sLog.outBasic("LFG Queue check finished");
    return;
}

void RandomPlayerbotMgr::CheckPlayers()
{
    if (!PlayersCheckTimer || time(NULL) > (PlayersCheckTimer + 60))
        PlayersCheckTimer = time(NULL);

    sLog.outBasic("Checking Players...");

    if (!playersLevel)
        playersLevel = sPlayerbotAIConfig.randombotStartingLevel;


    for (vector<Player*>::iterator i = players.begin(); i != players.end(); ++i)
    {
        Player* player = *i;

        if (player->IsGameMaster())
            continue;

        //if (player->GetSession()->GetSecurity() > SEC_PLAYER)
        //    continue;

        if (player->getLevel() > playersLevel)
            playersLevel = player->getLevel() + 3;
    }
    sLog.outBasic("Max player level is %d, max bot level set to %d", playersLevel - 3, playersLevel);
    return;
}

void RandomPlayerbotMgr::AddBgBot(BattleGroundQueueTypeId queueTypeId, BattleGroundBracketId bracketId, bool isRated, bool visual)
{
    BattleGroundTypeId bgTypeId = sServerFacade.BgTemplateId(queueTypeId);
    BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
    if (!bg)
        return;

    uint32 BracketSize = bg->GetMaxPlayers();
    uint32 TeamSize = bg->GetMaxPlayersPerTeam();
    bool isArena = false;
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
    if (ArenaType type = sServerFacade.BgArenaType(queueTypeId))
    {
        switch (type)
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
    }
#endif

    uint32 ACount = BgBots[queueTypeId][bracketId][0] + BgPlayers[queueTypeId][bracketId][0];
    uint32 HCount = BgBots[queueTypeId][bracketId][1] + BgPlayers[queueTypeId][bracketId][1];

    uint32 BgCount = ACount + HCount;

    uint32 SCount, RCount;

#ifndef MANGOSBOT_ZERO
    ArenaType type = sServerFacade.BgArenaType(queueTypeId);
    if (type != ARENA_TYPE_NONE)
    {
        isArena = true;
        BracketSize = type * 2;
        TeamSize = type;
        ACount = ArenaBots[queueTypeId][bracketId][isRated][0];
        HCount = ArenaBots[queueTypeId][bracketId][isRated][1];
        BgCount = BgBots[queueTypeId][bracketId][isRated] + BgPlayers[queueTypeId][bracketId][isRated];
        SCount = BgBots[queueTypeId][bracketId][0] + BgPlayers[queueTypeId][bracketId][0];
        RCount = BgBots[queueTypeId][bracketId][1] + BgPlayers[queueTypeId][bracketId][1];
    }
#endif

    string bgType = isArena ? "Arena" : "BG";
    if (BgCount >= BracketSize && !visual && (ACount >= TeamSize) && (HCount >= TeamSize))
    {
        sLog.outDetail("Can't add BG Bots to %s %d (%s), it is full", bgType, bgTypeId, _bgType);
        return;
    }

    Player* player = NULL;

#ifndef MANGOSBOT_ZERO
    if (!visual && isArena && ((!isRated && SCount >= BracketSize) || (!isRated && RCount >= BracketSize)))
    {
        sLog.outDetail("Can't add bots to %s %s, Arena queue is full", bgType, _bgType);
        return;
    }
#endif

    if (!visual)
        sLog.outDetail("Searching bots for %s %s", bgType, _bgType);

    if (!isRated)
    {
        for (PlayerBotMap::iterator i = playerBots.begin(); i != playerBots.end(); ++i)
        {
            if (urand(0, 100) > 10)
                continue;

            Player* bot = i->second;

            if (bot->IsBeingTeleported())
                continue;

            // check Deserter debuff
            if (!bot->CanJoinToBattleground())
                continue;

            Map* map = bot->GetMap();
            if (map && map->Instanceable())
                continue;

            uint32 bgType = bot->GetPlayerbotAI()->GetAiObjectContext()->GetValue<uint32>("bg type")->Get();
            if (bgType && bgType != 20)
                continue;

            if (visual && bgType == 20)
                continue;

            if ((time(0) - bot->GetInGameTime()) < 30)
                continue;

            if (bot->getLevel() < bg->GetMinLevel())
                continue;

#ifdef MANGOS
            if (bot->GetBattleGroundBracketIdFromLevel(bgTypeId) != bracketId)
                continue;
#endif
#ifdef CMANGOS
#ifdef MANGOSBOT_TWO
            BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
            uint32 mapId = bg->GetMapId();
            PvPDifficultyEntry const* pvpDiff = GetBattlegroundBracketByLevel(mapId, bot->getLevel());
            if (!pvpDiff)
                continue;

            BattleGroundBracketId bracket_temp = pvpDiff->GetBracketId();

            if (bracket_temp != bracketId)
                continue;
#else
            if (bot->GetBattleGroundBracketIdFromLevel(bgTypeId) != bracketId)
                continue;
#endif
#endif

            if (bot->GetPlayerbotAI()->GetMaster() && !bot->GetPlayerbotAI()->GetMaster()->GetPlayerbotAI())
                continue;

            //if (bot->GetGroup())
            //    continue;

            if (bot->IsInCombat())
                continue;

            if (bot->InBattleGroundQueue())
                continue;

            if (bot->GetBattleGround() && bot->GetBattleGround()->GetStatus() == STATUS_WAIT_LEAVE)
                continue;

            if (!bot->GetBGAccessByLevel(bgTypeId))
                continue;

            // add only x2 - x9 level
            //if (!visual && bracketId < BG_BRACKET_ID_LAST && (bot->getLevel() < ((bracketId * 10) + 12)))
            //	continue;

            if (player == NULL)
            {
                player = bot;
                break;
            }
        }
    }

#ifndef MANGOSBOT_ZERO
    if (isRated)
    {
        bool found_team = true;
        ArenaType type = sServerFacade.BgArenaType(queueTypeId);
        vector<uint32> aTeams;
        uint32 rating = Rating[queueTypeId][bracketId][1];
        for (list<uint32>::iterator i = sPlayerbotAIConfig.randomBotArenaTeams.begin(); i != sPlayerbotAIConfig.randomBotArenaTeams.end(); ++i)
        {
            ArenaTeam* arenateam = sObjectMgr.GetArenaTeamById(*i);
            if (!arenateam)
                continue;

            if (arenateam->GetType() != type)
                continue;

            //if ((rating - arenateam->GetRating()) > 150)
            //    continue;

            aTeams.push_back(arenateam->GetId());
        }

        if (aTeams.empty())
        {
            found_team = false;
            sLog.outBasic("No teams found for %s match (Rating:%d)", _bgType, rating);
        }

        ArenaTeam* arenateam;
        vector<uint32> members;
        uint32 maxPlayers = type;
        if (found_team)
        {
            uint32 index = urand(0, aTeams.size() - 1);
            uint32 arenaTeamId = aTeams[index];
            uint32 count = 0;
            arenateam = sObjectMgr.GetArenaTeamById(arenaTeamId);
            if (arenateam)
            {
#ifdef MANGOS
                for (ArenaTeam::MemberList::iterator itr = arenateam->m_membersBegin(); itr != arenateam->m_membersEnd(); ++itr)
#endif
#ifdef CMANGOS
                for (ArenaTeam::MemberList::iterator itr = arenateam->GetMembers().begin(); itr != arenateam->GetMembers().end(); ++itr)
#endif
                {
                    if (count >= maxPlayers)
                        break;

                    bool offline = false;
                    Player* member = sObjectMgr.GetPlayer(itr->guid);
                    if (!member)
                    {
                        offline = true;
                    }
                    if (!member && !sObjectMgr.GetPlayerAccountIdByGUID(itr->guid))
                        continue;

                    if (offline)
                        sRandomPlayerbotMgr.AddPlayerBot(itr->guid.GetRawValue(), 0);

                    //if (member->GetArenaPersonalRating(arenateam->GetSlot()) < (rating - 150))
                    //    continue;

                    if (member->getLevel() < 70)
                        continue;

                    members.push_back(member->GetGUIDLow());
                    count++;
                }
            }
            if (!members.size() || members.size() < maxPlayers)
            {
                found_team = false;
                sLog.outDetail("Team #%d <%s> has no members for %s match, skipping", arenaTeamId, arenateam->GetName(), _bgType);
            }
        }

        if (found_team && arenateam)
        {
            uint32 count = 0;
            ObjectGuid capGuid = arenateam->GetCaptainGuid();
            Player* cap = sObjectMgr.GetPlayer(capGuid);
            if (!cap)
                sRandomPlayerbotMgr.AddPlayerBot(capGuid.GetRawValue(), 0);

            cap = sObjectMgr.GetPlayer(capGuid);

            bool found_cap = false;
            if (!cap)
            {
                uint32 mem_guid = urand(0, members.size() - 1);
                capGuid = ObjectGuid(HIGHGUID_PLAYER, members[mem_guid]);
                cap = sObjectMgr.GetPlayer(capGuid);

                if (!cap)
                    sRandomPlayerbotMgr.AddPlayerBot(capGuid.GetRawValue(), 0);

                cap = sObjectMgr.GetPlayer(capGuid);
            }

            if (cap != NULL)
            {
                found_cap = true;

                if (cap->getLevel() < 70)
                    found_cap = false;

                if (cap->GetPlayerbotAI()->GetMaster())
                    found_cap = false;

                if (cap->GetGroup())
                    found_cap = false;

                if (cap->IsInCombat())
                    found_cap = false;

                if (cap->InBattleGroundQueue())
                    found_cap = false;

                uint32 bgType = cap->GetPlayerbotAI()->GetAiObjectContext()->GetValue<uint32>("bg type")->Get();
                if (bgType && bgType != 20)
                    found_cap = false;

                if (visual && bgType == 20)
                    found_cap = false;
            }

            if (cap != NULL && found_cap)
            {
                sLog.outDetail("Bot #%d <%s>: captian of <%s> %s", cap->GetGUIDLow(), cap->GetName(), arenateam->GetName(), _bgType);
#ifndef MANGOSBOT_TWO
                Group* group = new Group();
#else
                Group* group = new Group(GROUPTYPE_NORMAL);
#endif
                group->Create(capGuid, cap->GetName());
                count = 0;
                for (auto i = begin(members); i != end(members); ++i)
                {
                    if (*i == cap->GetGUIDLow())
                        continue;

                    if (count >= maxPlayers)
                        break;

                    Player* member = sObjectMgr.GetPlayer(ObjectGuid(HIGHGUID_PLAYER, *i));
                    if (!member)
                        continue;

                    if (member->getLevel() < 70)
                        continue;

                    if (!group->AddMember(ObjectGuid(HIGHGUID_PLAYER, *i), member->GetName()))
                        continue;

                    sLog.outDetail("Bot #%d <%s>: member of <%s> %s", member->GetGUIDLow(), member->GetName(), arenateam->GetName(), _bgType);

                    count++;
                }
                if (cap != NULL && cap->getLevel() >= 70)
                    player = cap;
            }
        }
    }
#endif

    if (player == NULL)
    {
        if (visual)
            sLog.outDetail("No Bots found to simulate waiting for %s (%s)", bgType, _bgType);
        else
            sLog.outBasic("No Bots found for %s (%s)", bgType, _bgType);
        return;
    }
    if (!visual)
        sLog.outDetail("Found bot #%d <%s> for %s %s", player->GetGUIDLow(), player->GetName(), bgType, _bgType);

    uint32 bot = player->GetGUIDLow();
    PlayerbotAI* ai = player->GetPlayerbotAI();
    uint32 TeamId = player->GetTeam() == ALLIANCE ? 0 : 1;

    if (!isArena && ((ACount >= TeamSize && TeamId == 0) || (HCount >= TeamSize && TeamId == 1)))
    {
        sLog.outDetail("Can't add this bot to %s %s, BG queue for this faction is full", bgType, _bgType);
        return;
    }

    if (isArena && (((ACount >= TeamSize && HCount > 0) && TeamId == 0) || ((HCount >= TeamSize && ACount > 0) && TeamId == 1)))
    {
        sLog.outDetail("Can't add this bot to %s %s, Arena queue for this faction is full", bgType, _bgType);
        return;
    }

    if (isArena && (((ACount > TeamSize && HCount == 0) && TeamId == 1) || ((HCount > TeamSize && ACount == 0) && TeamId == 0)))
    {
        sLog.outDetail("Can't add this bot to %s %s, Arena queue for this faction is full", bgType, _bgType);
        return;
    }

    if (isArena && ((!isRated && SCount >= BracketSize) || (!isRated && RCount >= BracketSize)))
    {
        sLog.outDetail("Can't add this bot to %s %s, Arena queue is full", bgType, _bgType);
        return;
    }

#ifndef MANGOSBOT_ZERO
    if (ArenaType type = sServerFacade.BgArenaType(queueTypeId))
    {
        isArena = true;
        BracketSize = type * 2;
        TeamSize = type;
        uint32 BgCount = BgBots[queueTypeId][bracketId][isRated] + BgPlayers[queueTypeId][bracketId][isRated];
    }
#endif

    if (!visual)
        sLog.outDetail("Bot #%d <%s> (%d %s) : adding %s (%s) Bot", player->GetGUIDLow(), player->GetName(), player->getLevel(), TeamId == 0 ? "A" : "H", bgType, _bgType);

    TeamId == 0 ? ACount++ : HCount++;

    if (!visual)
    {
        sLog.outDetail("Changing strategy for bot #%d <%s> to PVP", bot, player->GetName());
        sLog.outDetail("Bot #%d <%s> (%d %s) %s bracket %d sent to BattmeMaster", bot, player->GetName(), player->getLevel(), TeamId == 0 ? "A" : "H", bgType, bracketId);
        sLog.outBasic("Bot #%d <%s> (%d %s): %s %d (%s), bracket %d (%d/%d) (A:%d H:%d)", bot, player->GetName(), player->getLevel(), TeamId == 0 ? "A" : "H", bgType, bgTypeId, _bgType, bracketId, BgCount + 1, BracketSize, ACount, HCount);
        // BG Tactics preference
        player->GetPlayerbotAI()->GetAiObjectContext()->GetValue<uint32>("bg role")->Set(urand(0, 9));
    }
    else
    {
        sLog.outDetail("Bot #%d <%s> (%d %s) simulates waiting %s %d (%s) bracket %d", bot, player->GetName(), player->getLevel(), TeamId == 0 ? "A" : "H", bgType, bgTypeId, _bgType, bracketId);
    }

    // Find BattleMaster by Entry
    uint32 BmEntry = sRandomPlayerbotMgr.GetBattleMasterEntry(player, bgTypeId);

    // if found entry
    if (BmEntry)
    {
        CreatureDataPair const* dataPair = sRandomPlayerbotMgr.GetCreatureDataByEntry(BmEntry);
        CreatureData const* data = &dataPair->second;
        // if BattleMaster found, Teleport
        if (data)
        {
            if (player->IsTaxiFlying())
            {
                player->GetMotionMaster()->MovementExpired();
#ifdef MANGOS
                player->m_taxi.ClearTaxiDestinations();
#endif
            }
#ifdef MANGOSBOT_TWO
            if (sServerFacade.BgArenaType(queueTypeId))
                player->TeleportTo(data->mapid, data->posX, data->posY, data->posZ, player->GetOrientation());
            else
                RandomTeleportForRpg(player);
#else
            player->TeleportTo(data->mapid, data->posX, data->posY, data->posZ, player->GetOrientation());
#endif
            ObjectGuid BmGuid = ObjectGuid(HIGHGUID_UNIT, BmEntry, dataPair->first);
            player->GetPlayerbotAI()->GetAiObjectContext()->GetValue<ObjectGuid>("bg master")->Set(BmGuid);
        }
    }
#ifndef MANGOSBOT_TWO
    if (!BmEntry)
    {
        if(!visual)
            sLog.outError("Bot #%d <%s> could not find Battlemaster for %s %d (%s) bracket %d", player->GetGUIDLow(), player->GetName(), bgType, bgTypeId, _bgType, bracketId);

        return;
    }
#else
    if (!BmEntry && isArena)
    {
        if (!visual)
            sLog.outError("Bot #%d <%s> could not find Battlemaster for %s %d (%s) bracket %d", player->GetGUIDLow(), player->GetName(), bgType, bgTypeId, _bgType, bracketId);

        return;
    }
    else
    {
        player->GetPlayerbotAI()->GetAiObjectContext()->GetValue<ObjectGuid>("bg master")->Set(player->GetObjectGuid());
    }
#endif

    player->GetPlayerbotAI()->ChangeStrategy("-travel,-grind,-rpg,-custom::say", BOT_STATE_NON_COMBAT);
    if (urand(0, 100) < 65)
        player->GetPlayerbotAI()->ChangeStrategy("+stay", BOT_STATE_NON_COMBAT);
    player->GetPlayerbotAI()->ChangeStrategy("-mount", BOT_STATE_NON_COMBAT);

    if (visual)
    {
        VisualBots[queueTypeId][bracketId][TeamId]++;
        if (urand(0, 5) < 3)
            player->GetPlayerbotAI()->ChangeStrategy("+rpg,-stay", BOT_STATE_NON_COMBAT);
        player->GetPlayerbotAI()->GetAiObjectContext()->GetValue<uint32>("bg type")->Set(20);
        return;
    }
    Refresh(player);
    if (isArena)
    {
        if (isRated)
        {
            BgBots[queueTypeId][bracketId][isRated] += TeamSize;
            ArenaBots[queueTypeId][bracketId][isRated][TeamId] += TeamSize;
        }
        else
        {
            BgBots[queueTypeId][bracketId][isRated]++;
            ArenaBots[queueTypeId][bracketId][isRated][TeamId]++;
        }
    }
    else
        BgBots[queueTypeId][bracketId][TeamId]++;

    player->GetPlayerbotAI()->GetAiObjectContext()->GetValue<uint32>("bg type")->Set(queueTypeId);
    if (isArena)
        player->GetPlayerbotAI()->GetAiObjectContext()->GetValue<uint32>("arena type")->Set(isRated);

    if (player->GetPlayerbotAI()->GetMaster() && player->GetPlayerbotAI()->GetMaster()->GetPlayerbotAI())
        player->GetPlayerbotAI()->DoSpecificAction("leave");

    player->GetPlayerbotAI()->ChangeStrategy("+bg", BOT_STATE_NON_COMBAT);
    //player->GetPlayerbotAI()->DoSpecificAction("bg join");
    return;
}

void RandomPlayerbotMgr::ScheduleRandomize(uint32 bot, uint32 time)
{
    SetEventValue(bot, "randomize", 1, time);
    //SetEventValue(bot, "logout", 1, time + 30 + urand(sPlayerbotAIConfig.randomBotUpdateInterval, sPlayerbotAIConfig.randomBotUpdateInterval * 3));
}

void RandomPlayerbotMgr::ScheduleTeleport(uint32 bot, uint32 time)
{
    if (!time)
        time = 60 + urand(sPlayerbotAIConfig.randomBotUpdateInterval, sPlayerbotAIConfig.randomBotUpdateInterval * 3);
    SetEventValue(bot, "teleport", 1, time);
}

void RandomPlayerbotMgr::ScheduleChangeStrategy(uint32 bot, uint32 time)
{
    if (!time)
        time = urand(sPlayerbotAIConfig.minRandomBotChangeStrategyTime, sPlayerbotAIConfig.maxRandomBotChangeStrategyTime);
    SetEventValue(bot, "change_strategy", 1, time);
}

bool RandomPlayerbotMgr::ProcessBot(uint32 bot)
{
    Player* player = GetPlayerBot(bot);
    PlayerbotAI* ai = player ? player->GetPlayerbotAI() : NULL;

    uint32 isValid = GetEventValue(bot, "add");
    if (!isValid)
    {
		if (!player || !player->GetGroup())
		{
            if (player)
                sLog.outDetail("Bot #%d %s:%d <%s>: log out", bot, IsAlliance(player->getRace()) == ALLIANCE ? "A" : "H", player->getLevel(), player->GetName());
            else
                sLog.outDetail("Bot #%d: log out", bot);

			SetEventValue(bot, "add", 0, 0);
			currentBots.remove(bot);
			if (player) LogoutPlayerBot(bot);
		}
        return true;
    }

    uint32 isLogginIn = GetEventValue(bot, "login");
    if (isLogginIn)
        return false;

    if (!player)
    {
        if ((urand(0, 100) > 50) && currentBots.size() >= (sPlayerbotAIConfig.minRandomBots / 2)) // less lag during bots login
        {
            return true;
        }

        AddPlayerBot(bot, 0);
        SetEventValue(bot, "login", 1, sPlayerbotAIConfig.randomBotUpdateInterval);
        uint32 randomTime = urand(sPlayerbotAIConfig.minRandomBotReviveTime, sPlayerbotAIConfig.maxRandomBotReviveTime);
        SetEventValue(bot, "update", 1, randomTime);

        return true;
    }

    // Hotfix System
    /*if (player && !sServerFacade.UnitIsDead(player))
    {
        uint32 version = GetEventValue(bot, "version");
        if (!version)
        {
            version = 0;
        }
        if (version < VERSION)
        {
            Hotfix(player, version);
        }
    }*/

    SetEventValue(bot, "login", 0, 0);
    if (player->GetGroup() || player->IsTaxiFlying())
        return false;

    uint32 update = GetEventValue(bot, "update");
    if (!update)
    {
        if (ai) ai->GetAiObjectContext()->GetValue<bool>("random bot update")->Set(true);
        uint32 randomTime = urand(sPlayerbotAIConfig.minRandomBotReviveTime, sPlayerbotAIConfig.maxRandomBotReviveTime * 5);
        SetEventValue(bot, "update", 1, randomTime);
        return true;
    }

    uint32 logout = GetEventValue(bot, "logout");
    if (player && !logout && !isValid)
    {
        sLog.outBasic("Bot #%d %s:%d <%s>: log out", bot, IsAlliance(player->getRace()) == ALLIANCE ? "A" : "H", player->getLevel(), player->GetName());
        LogoutPlayerBot(bot);
        SetEventValue(bot, "logout", 1, sPlayerbotAIConfig.maxRandomBotInWorldTime);
        return true;
    }

    return false;
}

bool RandomPlayerbotMgr::ProcessBot(Player* player)
{
    uint32 bot = player->GetGUIDLow();

    if (player->InBattleGround())
        return false;

    if (player->InBattleGroundQueue())
        return false;

    // Hotfix System
    /*if (!sServerFacade.UnitIsDead(player))
    {
        uint32 version = GetEventValue(bot, "version");
        if (!version)
        {
            version = 0;
        }
        if (version < MANGOSBOT_VERSION)
        {
            //Hotfix(player, version); Temporary disable hotfix
        }
    }*/

    if (sServerFacade.UnitIsDead(player))
        return false;

    /*if (sServerFacade.UnitIsDead(player) && !player->InBattleGround())
    {
        if (!GetEventValue(bot, "dead"))
        {
            uint32 deathcount = GetEventValue(bot, "deathcount");
            if (!deathcount)
            {
                SetEventValue(bot, "deathcount", 1, sPlayerbotAIConfig.maxRandomBotInWorldTime);
                Revive(player);
                sLog.outDetail("Bot #%d <%s>: revived", bot, player->GetName());
            }
            else
            {
                if (deathcount > 4)
                {
                    SetEventValue(bot, "deathcount", 0, 0);
                    SetEventValue(bot, "dead", 0, 0);
                    SetEventValue(bot, "revive", 0, 0);
                    //Refresh(player);
                    RandomTeleportForRpg(player);
                    uint32 randomChange = urand(240 + sPlayerbotAIConfig.randomBotUpdateInterval, 600 + sPlayerbotAIConfig.randomBotUpdateInterval * 3);
                    ScheduleChangeStrategy(bot, randomChange);
                    SetEventValue(bot, "teleport", 1, sPlayerbotAIConfig.maxRandomBotInWorldTime);
                    sLog.outDetail("Bot #%d <%s>: died %d times, rest %d min", bot, player->GetName(), deathcount, int(randomChange / 60));
                }
                else
                {
                    SetEventValue(bot, "deathcount", deathcount + 1, sPlayerbotAIConfig.maxRandomBotInWorldTime);
                    Revive(player);
                    sLog.outDetail("Bot #%d <%s>: revived %d/5", bot, player->GetName(), deathcount + 1);
                }
            }
           return false; // increase revive rate
        }
    }*/

	if (urand(0, 100) > 20) // move optimisation to the next step
	{
		return true;
	}

	player->GetPlayerbotAI()->GetAiObjectContext()->GetValue<bool>("random bot update")->Set(false);

    bool randomiser = true;
    if (player->GetGuildId())
    {
		Guild* guild = sGuildMgr.GetGuildById(player->GetGuildId());
		if (guild->GetLeaderGuid().GetRawValue() == player->GetObjectGuid().GetRawValue()) {
			for (vector<Player*>::iterator i = players.begin(); i != players.end(); ++i)
				sGuildTaskMgr.Update(*i, player);
		}

        uint32 accountId = sObjectMgr.GetPlayerAccountIdByGUID(guild->GetLeaderGuid());

        if (!sPlayerbotAIConfig.IsInRandomAccountList(accountId))
        {
            int32 rank = guild->GetRank(player->GetObjectGuid());
            randomiser = rank < 4 ? false : true;
        }
    }

    uint32 randomize = GetEventValue(bot, "randomize");
    if (!randomize)
    {
        if (randomiser)
        {
            Randomize(player);
        }

        if (randomiser)
        {
            sLog.outBasic("Bot #%d <%s>: randomized", bot, player->GetName());
        }
        else
        {
            sLog.outBasic("Bot #%d %s <%s>: consumables refreshed", bot, player->GetName(), sGuildMgr.GetGuildById(player->GetGuildId())->GetName());
        }

        ChangeStrategy(player);

        uint32 randomTime = urand(sPlayerbotAIConfig.minRandomBotRandomizeTime, sPlayerbotAIConfig.maxRandomBotRandomizeTime);
        ScheduleRandomize(bot, randomTime);
        return true;
    }

	uint32 teleport = GetEventValue(bot, "teleport");
	if (!teleport)
	{
		sLog.outBasic("Bot #%d <%s>: sent to grind", bot, player->GetName());
		RandomTeleportForLevel(player);
		Refresh(player);
		SetEventValue(bot, "teleport", 1, sPlayerbotAIConfig.maxRandomBotInWorldTime);
		return true;
	}

	uint32 changeStrategy = GetEventValue(bot, "change_strategy");
	if (!changeStrategy)
	{
		sLog.outDetail("Changing strategy for bot #%d <%s>", bot, player->GetName());
		ChangeStrategy(player);
		return true;
	}

    return false;
}

void RandomPlayerbotMgr::Revive(Player* player)
{
    uint32 bot = player->GetGUIDLow();

    //sLog.outString("Bot %d revived", bot);
    SetEventValue(bot, "dead", 0, 0);
    SetEventValue(bot, "revive", 0, 0);

    if (sServerFacade.GetDeathState(player) == CORPSE)
    {
        RandomTeleport(player);
    }
    else
    {
        RandomTeleportForLevel(player);
        Refresh(player);
    }
}

void RandomPlayerbotMgr::RandomTeleport(Player* bot, vector<WorldLocation> &locs, bool hearth)
{
    if (bot->IsBeingTeleported())
        return;

    if (bot->InBattleGround())
        return;

    if (bot->InBattleGroundQueue())
        return;

	if (bot->getLevel() < 5)
		return;

    if (locs.empty())
    {
        sLog.outError("Cannot teleport bot %s - no locations available", bot->GetName());
        return;
    }

    vector<WorldLocation> tlocs = locs;

    //Do not teleport to maps disabled in config
    //tlocs.erase(std::remove_if(tlocs.begin(), tlocs.end(), [bot](WorldLocation const& l) {vector<uint32>::iterator i = find(sPlayerbotAIConfig.randomBotMaps.begin(), sPlayerbotAIConfig.randomBotMaps.end(), l.mapid); return i == sPlayerbotAIConfig.randomBotMaps.end(); }), tlocs.end());

    //5% + 0.1% per level chance node on different map in selection.
    tlocs.erase(std::remove_if(tlocs.begin(), tlocs.end(), [bot](WorldLocation const& l) {return l.mapid != bot->GetMapId() && urand(1, 100) > 0.5 * bot->getLevel(); }), tlocs.end());

    //Continent is about 20.000 large
    //Bot will travel 0-5000 units + 75-150 units per level.
    tlocs.erase(std::remove_if(tlocs.begin(), tlocs.end(), [bot](WorldLocation const& l) {return l.mapid == bot->GetMapId() && sServerFacade.GetDistance2d(bot, l.coord_x, l.coord_y) > urand(0, 5000) + bot->getLevel() * 15 * urand(5, 10); }), tlocs.end());

    if (tlocs.empty())
    {
        sLog.outError("Cannot teleport bot %s - no locations available", bot->GetName());
        return;
    }

    PerformanceMonitorOperation *pmo = sPerformanceMonitor.start(PERF_MON_RNDBOT, "RandomTeleportByLocations");
    for (int attemtps = 0; attemtps < 10; ++attemtps)
    {
        int index = urand(0, tlocs.size() - 1);
        WorldLocation loc = tlocs[index];

#ifndef MANGOSBOT_ZERO
        // Teleport to Dark Portal area if event is in progress
        if (sWorldState.GetExpansion() == EXPANSION_NONE && bot->getLevel() > 54 && urand(0, 100) > 20)
        {
            if (urand(0, 1))
                loc = WorldLocation(uint32(0), -11772.43f, -3272.84f, -17.9f, 3.32447f);
            else
                loc = WorldLocation(uint32(0), -11741.70f, -3130.3f, -11.7936f, 3.32447f);
        }
#endif

        float x = loc.coord_x + (attemtps > 0 ? urand(0, sPlayerbotAIConfig.grindDistance) - sPlayerbotAIConfig.grindDistance / 2 : 0);
        float y = loc.coord_y + (attemtps > 0 ? urand(0, sPlayerbotAIConfig.grindDistance) - sPlayerbotAIConfig.grindDistance / 2 : 0);
        float z = loc.coord_z;

        Map* map = sMapMgr.FindMap(loc.mapid, 0);
        if (!map)
            continue;

		const TerrainInfo * terrain = map->GetTerrain();
		if (!terrain)
			continue;

		AreaTableEntry const* area = GetAreaEntryByAreaID(terrain->GetAreaId(x, y, z));
		if (!area)
			continue;

#ifndef MANGOSBOT_ZERO
        // Do not teleport to outland before portal opening (allow new races zones)
        if (sWorldState.GetExpansion() == EXPANSION_NONE && loc.mapid == 530 && area->team != 2 && area->team != 4)
            continue;
#endif

        // Do not teleport to enemy zones if level is low
        if (area->team == 4 && bot->GetTeam() == ALLIANCE && bot->getLevel() < 40)
            continue;
        if (area->team == 2 && bot->GetTeam() == HORDE && bot->getLevel() < 40)
            continue;

		if (terrain->IsUnderWater(x, y, z) ||
			terrain->IsInWater(x, y, z))
			continue;

#ifdef MANGOSBOT_TWO
        float ground = map->GetHeight(bot->GetPhaseMask(), x, y, z + 0.5f);
#else
        float ground = map->GetHeight(x, y, z + 0.5f);
#endif
        if (ground <= INVALID_HEIGHT)
            continue;

        z = 0.05f + ground;
        sLog.outDetail("Random teleporting bot %s to %s %f,%f,%f (%u/%zu locations)",
                bot->GetName(), area->area_name[0], x, y, z, attemtps, tlocs.size());

        if (bot->IsTaxiFlying())
        {
            bot->GetMotionMaster()->MovementExpired();
#ifdef MANGOS
            bot->m_taxi.ClearTaxiDestinations();
#endif
        }
        if (hearth)
            bot->SetHomebindToLocation(loc, area->ID);

        bot->GetMotionMaster()->Clear();
        bot->TeleportTo(loc.mapid, x, y, z, 0);
        bot->SendHeartBeat();
        bot->GetPlayerbotAI()->ResetStrategies();
        bot->GetPlayerbotAI()->Reset();
        if (pmo) pmo->finish();
        return;
    }

    if (pmo) pmo->finish();
    sLog.outError("Cannot teleport bot %s - no locations available", bot->GetName());
}

void RandomPlayerbotMgr::PrepareTeleportCache()
{
    uint32 maxLevel = sPlayerbotAIConfig.randomBotMaxLevel;
    if (maxLevel > sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
        maxLevel = sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL);

    QueryResult* results = PlayerbotDatabase.PQuery("select map_id, x, y, z, level from ai_playerbot_tele_cache");
    if (results)
    {
        sLog.outString("Loading random teleport caches for %d levels...", maxLevel);
        do
        {
            Field* fields = results->Fetch();
            uint16 mapId = fields[0].GetUInt16();
            float x = fields[1].GetFloat();
            float y = fields[2].GetFloat();
            float z = fields[3].GetFloat();
            uint16 level = fields[4].GetUInt16();
            WorldLocation loc(mapId, x, y, z, 0);
            locsPerLevelCache[level].push_back(loc);
        } while (results->NextRow());
        delete results;
    }
    else
    {
        sLog.outString("Preparing random teleport caches for %d levels...", maxLevel);
        BarGoLink bar(maxLevel);
        for (uint8 level = 1; level <= maxLevel; level++)
        {
            QueryResult* results = WorldDatabase.PQuery("select map, position_x, position_y, position_z "
                "from (select map, position_x, position_y, position_z, avg(t.maxlevel), avg(t.minlevel), "
                "%u - (avg(t.maxlevel) + avg(t.minlevel)) / 2 delta "
                "from creature c inner join creature_template t on c.id = t.entry where t.NpcFlags = 0 and t.lootid != 0 and t.unitFlags != 768 group by t.entry having count(*) > 1) q "
                "where delta >= 0 and delta <= %u and map in (%s) and not exists ( "
                "select map, position_x, position_y, position_z from "
                "("
                "select map, c.position_x, c.position_y, c.position_z, avg(t.maxlevel), avg(t.minlevel), "
                "%u - (avg(t.maxlevel) + avg(t.minlevel)) / 2 delta "
                "from creature c "
                "inner join creature_template t on c.id = t.entry where t.NpcFlags = 0 and t.lootid != 0 group by t.entry "
                ") q1 "
                "where abs(delta) > %u and q1.map = q.map "
                "and sqrt("
                "(q1.position_x - q.position_x)*(q1.position_x - q.position_x) +"
                "(q1.position_y - q.position_y)*(q1.position_y - q.position_y) +"
                "(q1.position_z - q.position_z)*(q1.position_z - q.position_z)"
                ") < %u)",
                level,
                sPlayerbotAIConfig.randomBotTeleLevel,
                sPlayerbotAIConfig.randomBotMapsAsString.c_str(),
                level,
                sPlayerbotAIConfig.randomBotTeleLevel,
                (uint32)sPlayerbotAIConfig.sightDistance
                );
            if (results)
            {
                do
                {
                    Field* fields = results->Fetch();
                    uint16 mapId = fields[0].GetUInt16();
                    float x = fields[1].GetFloat();
                    float y = fields[2].GetFloat();
                    float z = fields[3].GetFloat();
                    WorldLocation loc(mapId, x, y, z, 0);
                    locsPerLevelCache[level].push_back(loc);

                    PlayerbotDatabase.PExecute("insert into ai_playerbot_tele_cache (level, map_id, x, y, z) values (%u, %u, %f, %f, %f)",
                            level, mapId, x, y, z);
                } while (results->NextRow());
                delete results;
            }
            bar.step();
        }
    }

    sLog.outString("Preparing RPG teleport caches for %d factions...", sFactionTemplateStore.GetNumRows());

		    results = WorldDatabase.PQuery("SELECT map, position_x, position_y, position_z, "
				"r.race, r.minl, r.maxl "
				"from creature c inner join ai_playerbot_rpg_races r on c.id = r.entry "
				"where r.race < 15");

	if (results)
	{
		do
		{
			for (uint32 level = 1; level < sPlayerbotAIConfig.randomBotMaxLevel + 1; level++)
			{
				Field* fields = results->Fetch();
				uint16 mapId = fields[0].GetUInt16();
				float x = fields[1].GetFloat();
				float y = fields[2].GetFloat();
				float z = fields[3].GetFloat();
				//uint32 faction = fields[4].GetUInt32();
				//string name = fields[5].GetCppString();
				uint32 race = fields[4].GetUInt32();
				uint32 minl = fields[5].GetUInt32();
				uint32 maxl = fields[6].GetUInt32();

				if (level > maxl || level < minl) continue;

				WorldLocation loc(mapId, x, y, z, 0);
				for (uint32 r = 1; r < MAX_RACES; r++)
				{
					if (race == r || race == 0) rpgLocsCacheLevel[r][level].push_back(loc);
				}
			}
			//bar.step();
		} while (results->NextRow());
		delete results;
	}
}

void RandomPlayerbotMgr::RandomTeleportForLevel(Player* bot)
{
    if (bot->InBattleGround())
        return;

    sLog.outDetail("Preparing location to random teleporting bot %s for level %u", bot->GetName(), bot->getLevel());
    RandomTeleport(bot, locsPerLevelCache[bot->getLevel()]);
}

void RandomPlayerbotMgr::RandomTeleport(Player* bot)
{
    if (bot->InBattleGround())
        return;

    PerformanceMonitorOperation *pmo = sPerformanceMonitor.start(PERF_MON_RNDBOT, "RandomTeleport");
    vector<WorldLocation> locs;

    list<Unit*> targets;
    float range = sPlayerbotAIConfig.randomBotTeleportDistance;
    MaNGOS::AnyUnitInObjectRangeCheck u_check(bot, range);
    MaNGOS::UnitListSearcher<MaNGOS::AnyUnitInObjectRangeCheck> searcher(targets, u_check);
    Cell::VisitAllObjects(bot, searcher, range);

    if (!targets.empty())
    {
        for (list<Unit *>::iterator i = targets.begin(); i != targets.end(); ++i)
        {
            Unit* unit = *i;
            bot->SetPosition(unit->GetPositionX(), unit->GetPositionY(), unit->GetPositionZ(), 0);
            FleeManager manager(bot, sPlayerbotAIConfig.sightDistance, 0, true);
            float rx, ry, rz;
            if (manager.CalculateDestination(&rx, &ry, &rz))
            {
                WorldLocation loc(bot->GetMapId(), rx, ry, rz);
                locs.push_back(loc);
            }
        }
    }
    else
    {
        RandomTeleportForLevel(bot);
    }

    if (pmo) pmo->finish();

    Refresh(bot);
}

void RandomPlayerbotMgr::Randomize(Player* bot)
{
    if (bot->InBattleGround())
        return;

    if (bot->getLevel() == 1)
        RandomizeFirst(bot);
#ifdef MANGOSBOT_TWO
    else if (bot->getLevel() == 55 && bot->getClass() == CLASS_DEATH_KNIGHT)
        RandomizeFirst(bot);
#endif
    else
        IncreaseLevel(bot);

    RandomTeleportForRpg(bot);

    //SetValue(bot, "version", MANGOSBOT_VERSION);
}

void RandomPlayerbotMgr::IncreaseLevel(Player* bot)
{
	uint32 maxLevel = sPlayerbotAIConfig.randomBotMaxLevel;
	if (maxLevel > sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
		maxLevel = sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL);

	PerformanceMonitorOperation *pmo = sPerformanceMonitor.start(PERF_MON_RNDBOT, "IncreaseLevel");
	uint32 lastLevel = GetValue(bot, "level");
	uint32 level = bot->getLevel();
	if (lastLevel != level)
	{
        PlayerbotFactory factory(bot, level);
        factory.Randomize(true);
	}

    if (pmo) pmo->finish();
}

void RandomPlayerbotMgr::RandomizeFirst(Player* bot)
{
	uint32 maxLevel = sPlayerbotAIConfig.randomBotMaxLevel;
	if (maxLevel > sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
		maxLevel = sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL);

    // if lvl sync is enabled, max level is limited by online players lvl
    if (sPlayerbotAIConfig.syncLevelWithPlayers)
        maxLevel = max(sPlayerbotAIConfig.randomBotMinLevel, min(playersLevel, sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL)));

	PerformanceMonitorOperation *pmo = sPerformanceMonitor.start(PERF_MON_RNDBOT, "RandomizeFirst");
    uint32 level = urand(sPlayerbotAIConfig.randomBotMinLevel, maxLevel);

#ifdef MANGOSBOT_TWO
    if (bot->getClass() == CLASS_DEATH_KNIGHT)
        level = urand(sWorld.getConfig(CONFIG_UINT32_START_HEROIC_PLAYER_LEVEL), max(sWorld.getConfig(CONFIG_UINT32_START_HEROIC_PLAYER_LEVEL), maxLevel));
#endif

    if (urand(0, 100) < 100 * sPlayerbotAIConfig.randomBotMaxLevelChance)
        level = maxLevel;

#ifndef MANGOSBOT_ZERO
    if (sWorldState.GetExpansion() == EXPANSION_NONE && level > 60)
        level = 60;
#endif

    SetValue(bot, "level", level);
    PlayerbotFactory factory(bot, level);
    factory.Randomize(false);
	
    uint32 randomTime = urand(sPlayerbotAIConfig.minRandomBotRandomizeTime, sPlayerbotAIConfig.maxRandomBotRandomizeTime);
	uint32 inworldTime = urand(sPlayerbotAIConfig.minRandomBotInWorldTime, sPlayerbotAIConfig.maxRandomBotInWorldTime);
    PlayerbotDatabase.PExecute("update ai_playerbot_random_bots set validIn = '%u' where event = 'randomize' and bot = '%u'",
            randomTime, bot->GetGUIDLow());
    PlayerbotDatabase.PExecute("update ai_playerbot_random_bots set validIn = '%u' where event = 'logout' and bot = '%u'",
			inworldTime, bot->GetGUIDLow());

	if (pmo) pmo->finish();
}

uint32 RandomPlayerbotMgr::GetZoneLevel(uint16 mapId, float teleX, float teleY, float teleZ)
{
	uint32 maxLevel = sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL);

	uint32 level;
    QueryResult* results = WorldDatabase.PQuery("select avg(t.minlevel) minlevel, avg(t.maxlevel) maxlevel from creature c "
            "inner join creature_template t on c.id = t.entry "
            "where map = '%u' and minlevel > 1 and abs(position_x - '%f') < '%u' and abs(position_y - '%f') < '%u'",
            mapId, teleX, sPlayerbotAIConfig.randomBotTeleportDistance / 2, teleY, sPlayerbotAIConfig.randomBotTeleportDistance / 2);

    if (results)
    {
        Field* fields = results->Fetch();
        uint8 minLevel = fields[0].GetUInt8();
        uint8 maxLevel = fields[1].GetUInt8();
        level = urand(minLevel, maxLevel);
        if (level > maxLevel)
            level = maxLevel;
		delete results;
    }
    else
    {
        level = urand(1, maxLevel);
    }

    return level;
}

void RandomPlayerbotMgr::Refresh(Player* bot)
{
    if (bot->InBattleGround())
        return;

    sLog.outDetail("Refreshing bot #%d <%s>", bot->GetGUIDLow(), bot->GetName());
    PerformanceMonitorOperation *pmo = sPerformanceMonitor.start(PERF_MON_RNDBOT, "Refresh");
    if (sServerFacade.UnitIsDead(bot))
    {
        bot->ResurrectPlayer(1.0f);
        bot->SpawnCorpseBones();
        bot->GetPlayerbotAI()->ResetStrategies(false);
    }

    bot->GetPlayerbotAI()->Reset();

    bot->DurabilityRepairAll(false, 1.0f
#ifndef MANGOSBOT_ZERO
        , false
#endif
    );
	bot->SetHealthPercent(100);
	bot->SetPvP(true);

    PlayerbotFactory factory(bot, bot->getLevel());
    factory.Refresh();

    if (bot->GetMaxPower(POWER_MANA) > 0)
        bot->SetPower(POWER_MANA, bot->GetMaxPower(POWER_MANA));

    if (bot->GetMaxPower(POWER_ENERGY) > 0)
        bot->SetPower(POWER_ENERGY, bot->GetMaxPower(POWER_ENERGY));

    uint32 money = bot->GetMoney();
    bot->SetMoney(money + 500 * sqrt(urand(1, bot->getLevel() * 5)));

    if (pmo) pmo->finish();
}


bool RandomPlayerbotMgr::IsRandomBot(Player* bot)
{
	return IsRandomBot(bot->GetObjectGuid());
}

bool RandomPlayerbotMgr::IsRandomBot(uint32 bot)
{
    return GetEventValue(bot, "add");
}

list<uint32> RandomPlayerbotMgr::GetBots()
{
    if (!currentBots.empty()) return currentBots;

    QueryResult* results = PlayerbotDatabase.Query(
            "select bot from ai_playerbot_random_bots where owner = 0 and event = 'add'");

    if (results)
    {
        do
        {
            Field* fields = results->Fetch();
            uint32 bot = fields[0].GetUInt32();
            currentBots.push_back(bot);
        } while (results->NextRow());
		delete results;
    }

    return currentBots;
}

list<uint32> RandomPlayerbotMgr::GetBgBots(uint32 bracket)
{
    //if (!currentBgBots.empty()) return currentBgBots;

    QueryResult* results = PlayerbotDatabase.PQuery(
        "select bot from ai_playerbot_random_bots where event = 'bg' AND value = '%d'", bracket);
    list<uint32> BgBots;
    if (results)
    {
        do
        {
            Field* fields = results->Fetch();
            uint32 bot = fields[0].GetUInt32();
            BgBots.push_back(bot);
        } while (results->NextRow());
        delete results;
    }

    return BgBots;
}

uint32 RandomPlayerbotMgr::GetEventValue(uint32 bot, string event)
{
    CachedEvent e = eventCache[bot][event];
    if (e.IsEmpty())
    {
        QueryResult* results = PlayerbotDatabase.PQuery(
                "select `value`, `time`, validIn from ai_playerbot_random_bots where owner = 0 and bot = '%u' and event = '%s'",
                bot, event.c_str());

        if (results)
        {
            Field* fields = results->Fetch();
            e.value = fields[0].GetUInt32();
            e.lastChangeTime = fields[1].GetUInt32();
            e.validIn = fields[2].GetUInt32();
            eventCache[bot][event] = e;
            delete results;
        }
    }

    if ((time(0) - e.lastChangeTime) >= e.validIn && (event == "add" || IsRandomBot(bot)))
        e.value = 0;

    return e.value;
}

uint32 RandomPlayerbotMgr::SetEventValue(uint32 bot, string event, uint32 value, uint32 validIn)
{
    PlayerbotDatabase.PExecute("delete from ai_playerbot_random_bots where owner = 0 and bot = '%u' and event = '%s'",
            bot, event.c_str());
    if (value)
    {
        PlayerbotDatabase.PExecute(
                "insert into ai_playerbot_random_bots (owner, bot, `time`, validIn, event, `value`) values ('%u', '%u', '%u', '%u', '%s', '%u')",
                0, bot, (uint32)time(0), validIn, event.c_str(), value);
    }

    CachedEvent e(value, (uint32)time(0), validIn);
    eventCache[bot][event] = e;
    return value;
}

uint32 RandomPlayerbotMgr::GetValue(uint32 bot, string type)
{
    return GetEventValue(bot, type);
}

uint32 RandomPlayerbotMgr::GetValue(Player* bot, string type)
{
    return GetValue(bot->GetObjectGuid().GetCounter(), type);
}

void RandomPlayerbotMgr::SetValue(uint32 bot, string type, uint32 value)
{
    SetEventValue(bot, type, value, sPlayerbotAIConfig.maxRandomBotInWorldTime);
}

void RandomPlayerbotMgr::SetValue(Player* bot, string type, uint32 value)
{
    SetValue(bot->GetObjectGuid().GetCounter(), type, value);
}

bool RandomPlayerbotMgr::HandlePlayerbotConsoleCommand(ChatHandler* handler, char const* args)
{
    if (!sPlayerbotAIConfig.enabled)
    {
        sLog.outError("Playerbot system is currently disabled!");
        return false;
    }

    if (!args || !*args)
    {
        sLog.outError("Usage: rndbot stats/update/reset/init/refresh/add/remove");
        return false;
    }

    string cmd = args;

    if (cmd == "reset")
    {
        PlayerbotDatabase.PExecute("delete from ai_playerbot_random_bots");
        sRandomPlayerbotMgr.eventCache.clear();
        sLog.outString("Random bots were reset for all players. Please restart the Server.");
        return true;
    }

    if (cmd == "stats")
    {
        activatePrintStatsThread();
        return true;
    }

    if (cmd == "update")
    {
        sRandomPlayerbotMgr.UpdateAIInternal(0);
        return true;
    }

    map<string, ConsoleCommandHandler> handlers;
    handlers["init"] = &RandomPlayerbotMgr::RandomizeFirst;
    handlers["levelup"] = handlers["level"] = &RandomPlayerbotMgr::IncreaseLevel;
    handlers["refresh"] = &RandomPlayerbotMgr::Refresh;
    handlers["teleport"] = &RandomPlayerbotMgr::RandomTeleportForLevel;
    handlers["rpg"] = &RandomPlayerbotMgr::RandomTeleportForRpg;
    handlers["revive"] = &RandomPlayerbotMgr::Revive;
    handlers["grind"] = &RandomPlayerbotMgr::RandomTeleport;
    handlers["change_strategy"] = &RandomPlayerbotMgr::ChangeStrategy;

    for (map<string, ConsoleCommandHandler>::iterator j = handlers.begin(); j != handlers.end(); ++j)
    {
        string prefix = j->first;
        if (cmd.find(prefix) != 0) continue;
        string name = cmd.size() > prefix.size() + 1 ? cmd.substr(1 + prefix.size()) : "%";

        list<uint32> botIds;
        for (list<uint32>::iterator i = sPlayerbotAIConfig.randomBotAccounts.begin(); i != sPlayerbotAIConfig.randomBotAccounts.end(); ++i)
        {
            uint32 account = *i;
            if (QueryResult* results = CharacterDatabase.PQuery("SELECT guid FROM characters where account = '%u' and name like '%s'",
                    account, name.c_str()))
            {
                do
                {
                    Field* fields = results->Fetch();

                    uint32 botId = fields[0].GetUInt32();
                    ObjectGuid guid = ObjectGuid(HIGHGUID_PLAYER, botId);
                    Player* bot = sObjectMgr.GetPlayer(guid);
                    if (!bot)
                        continue;

                    botIds.push_back(botId);
                } while (results->NextRow());
				delete results;
			}
        }

        if (botIds.empty())
        {
            sLog.outString("Nothing to do");
            return false;
        }

        int processed = 0;
        for (list<uint32>::iterator i = botIds.begin(); i != botIds.end(); ++i)
        {
            ObjectGuid guid = ObjectGuid(HIGHGUID_PLAYER, *i);
            Player* bot = sObjectMgr.GetPlayer(guid);
            if (!bot)
                continue;

            sLog.outString("[%u/%zu] Processing command '%s' for bot '%s'",
                    processed++, botIds.size(), cmd.c_str(), bot->GetName());

            ConsoleCommandHandler handler = j->second;
            (sRandomPlayerbotMgr.*handler)(bot);
        }
        return true;
    }

    list<string> messages = sRandomPlayerbotMgr.HandlePlayerbotCommand(args, NULL);
    for (list<string>::iterator i = messages.begin(); i != messages.end(); ++i)
    {
        sLog.outString("%s",i->c_str());
    }
    return true;
}

void RandomPlayerbotMgr::HandleCommand(uint32 type, const string& text, Player& fromPlayer)
{
    for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
    {
        Player* const bot = it->second;
        bot->GetPlayerbotAI()->HandleCommand(type, text, fromPlayer);
    }
}

void RandomPlayerbotMgr::OnPlayerLogout(Player* player)
{
     DisablePlayerBot(player->GetObjectGuid().GetRawValue());

    for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
    {
        Player* const bot = it->second;
        PlayerbotAI* ai = bot->GetPlayerbotAI();
        if (player == ai->GetMaster())
        {
            ai->SetMaster(NULL);
            if (!bot->InBattleGround())
            {
                ai->ResetStrategies();
                ai->ChangeStrategy("-rpg", BOT_STATE_NON_COMBAT);
                ai->ChangeStrategy("-grind", BOT_STATE_NON_COMBAT);
            }
        }
    }

    vector<Player*>::iterator i = find(players.begin(), players.end(), player);
    if (i != players.end())
        players.erase(i);
}

void RandomPlayerbotMgr::OnBotLoginInternal(Player * const bot)
{
    sLog.outDetail("%lu/%d Bot %s logged in", playerBots.size(), sRandomPlayerbotMgr.GetMaxAllowedBotCount(), bot->GetName());
	//if (loginProgressBar && playerBots.size() < sRandomPlayerbotMgr.GetMaxAllowedBotCount()) { loginProgressBar->step(); }
	//if (loginProgressBar && playerBots.size() >= sRandomPlayerbotMgr.GetMaxAllowedBotCount() - 1) {
    //if (loginProgressBar && playerBots.size() + 1 >= sRandomPlayerbotMgr.GetMaxAllowedBotCount()) {
	//	sLog.outString("All bots logged in");
    //    delete loginProgressBar;
	//}
}

void RandomPlayerbotMgr::OnPlayerLogin(Player* player)
{
    for (PlayerBotMap::const_iterator it = GetPlayerBotsBegin(); it != GetPlayerBotsEnd(); ++it)
    {
        Player* const bot = it->second;
        if (player == bot/* || player->GetPlayerbotAI()*/) // TEST
            continue;

        Group* group = bot->GetGroup();
        if (!group)
            continue;

        for (GroupReference *gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player* member = gref->getSource();
            PlayerbotAI* ai = bot->GetPlayerbotAI();
            if (member == player && (!ai->GetMaster() || ai->GetMaster()->GetPlayerbotAI()))
            {
                if (!bot->InBattleGround())
                {
                    ai->SetMaster(player);
                    ai->ResetStrategies();
                    ai->TellMaster("Hello");
                    if (!player->GetPlayerbotAI())
                    {
                        ai->ChangeStrategy("-rpg", BOT_STATE_NON_COMBAT);
                        ai->ChangeStrategy("-grind", BOT_STATE_NON_COMBAT);
                    }
                }
                break;
            }
        }
    }

    if (IsRandomBot(player))
    {
        uint64 guid = player->GetObjectGuid().GetRawValue();
        SetEventValue((uint32)guid, "login", 0, 0);
    }
    else
    {
        players.push_back(player);
        sLog.outDebug("Including non-random bot player %s into random bot update", player->GetName());
    }
}

void RandomPlayerbotMgr::OnPlayerLoginError(uint32 bot)
{
    SetEventValue(bot, "add", 0, 0);
    currentBots.remove(bot);
}

Player* RandomPlayerbotMgr::GetRandomPlayer()
{
    if (players.empty())
        return NULL;

    uint32 index = urand(0, players.size() - 1);
    return players[index];
}

void RandomPlayerbotMgr::PrintStats()
{
    sLog.outString("%lu Random Bots online", playerBots.size());

    map<uint32, int> alliance, horde;
    for (uint32 i = 0; i < 10; ++i)
    {
        alliance[i] = 0;
        horde[i] = 0;
    }

    map<uint8, int> perRace, perClass;
    for (uint8 race = RACE_HUMAN; race < MAX_RACES; ++race)
    {
        perRace[race] = 0;
    }
    for (uint8 cls = CLASS_WARRIOR; cls < MAX_CLASSES; ++cls)
    {
        perClass[cls] = 0;
    }

    int dps = 0, heal = 0, tank = 0, active = 0, update = 0, randomize = 0, teleport = 0, changeStrategy = 0, dead = 0, revive = 0;
    int stateCount[MAX_TRAVEL_STATE + 1] = { 0 };
    vector<pair<Quest const*, int32>> questCount;
    for (PlayerBotMap::iterator i = playerBots.begin(); i != playerBots.end(); ++i)
    {
        Player* bot = i->second;
        if (IsAlliance(bot->getRace()))
            alliance[bot->getLevel() / 10]++;
        else
            horde[bot->getLevel() / 10]++;

        perRace[bot->getRace()]++;
        perClass[bot->getClass()]++;

        if (bot->GetPlayerbotAI()->IsActive())
            active++;

        if (bot->GetPlayerbotAI()->GetAiObjectContext()->GetValue<bool>("random bot update")->Get())
            update++;

        uint32 botId = (uint32)bot->GetObjectGuid().GetRawValue();
        if (!GetEventValue(botId, "randomize"))
            randomize++;

        if (!GetEventValue(botId, "teleport"))
            teleport++;

        if (!GetEventValue(botId, "change_strategy"))
            changeStrategy++;

        if (sServerFacade.UnitIsDead(bot))
        {
            dead++;
            if (!GetEventValue(botId, "dead"))
                revive++;
        }

        int spec = AiFactory::GetPlayerSpecTab(bot);
        switch (bot->getClass())
        {
        case CLASS_DRUID:
            if (spec == 2)
                heal++;
            else
                dps++;
            break;
        case CLASS_PALADIN:
            if (spec == 1)
                tank++;
            else if (spec == 0)
                heal++;
            else
                dps++;
            break;
        case CLASS_PRIEST:
            if (spec != 2)
                heal++;
            else
                dps++;
            break;
        case CLASS_SHAMAN:
            if (spec == 2)
                heal++;
            else
                dps++;
            break;
        case CLASS_WARRIOR:
            if (spec == 2)
                tank++;
            else
                dps++;
            break;
#ifdef MANGOSBOT_TWO
        case CLASS_DEATH_KNIGHT:
            if (spec == 0)
                tank++;
            else
                dps++;
            break;
#endif
        default:
            dps++;
            break;
        }

        TravelTarget* target = bot->GetPlayerbotAI()->GetAiObjectContext()->GetValue<TravelTarget*>("travel target")->Get();
        if (target)
        {
            TravelState state = target->getTravelState();
            stateCount[state]++;

            Quest const* quest;

            if (target->getDestination())
                quest = target->getDestination()->GetQuestTemplate();


            if (quest)
            {
                bool found = false;

                for (auto& q : questCount)
                {
                    if (q.first != quest)
                        continue;

                    q.second++;
                    found = true;
                }

                if (!found)
                    questCount.push_back(make_pair(quest, 1));
            }
        }
    }

    sLog.outString("Per level:");
	uint32 maxLevel = sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL);
	for (uint32 i = 0; i < 10; ++i)
    {
        if (!alliance[i] && !horde[i])
            continue;

        uint32 from = i*10;
        uint32 to = min(from + 9, maxLevel);
        if (!from) from = 1;
        sLog.outString("    %d..%d: %d alliance, %d horde", from, to, alliance[i], horde[i]);
    }
    sLog.outString("Per race:");
    for (uint8 race = RACE_HUMAN; race < MAX_RACES; ++race)
    {
        if (perRace[race])
            sLog.outString("    %s: %d", ChatHelper::formatRace(race).c_str(), perRace[race]);
    }
    sLog.outString("Per class:");
    for (uint8 cls = CLASS_WARRIOR; cls < MAX_CLASSES; ++cls)
    {
        if (perClass[cls])
            sLog.outString("    %s: %d", ChatHelper::formatClass(cls).c_str(), perClass[cls]);
    }
    sLog.outString("Per role:");
    sLog.outString("    tank: %d", tank);
    sLog.outString("    heal: %d", heal);
    sLog.outString("    dps: %d", dps);

    sLog.outString("Active bots: %d", active);
    sLog.outString("Dead bots: %d", dead);
    sLog.outString("Bots to:");
    sLog.outString("    update: %d", update);
    sLog.outString("    randomize: %d", randomize);
    sLog.outString("    teleport: %d", teleport);
    sLog.outString("    change_strategy: %d", changeStrategy);
    sLog.outString("    revive: %d", revive);

    sLog.outString("Bots travel:");
    sLog.outString("    travel to pick up quest: %d", stateCount[TRAVEL_STATE_TRAVEL_PICK_UP_QUEST]);
    sLog.outString("    try to pick up quest: %d", stateCount[TRAVEL_STATE_WORK_PICK_UP_QUEST]);
    sLog.outString("    travel to do quest: %d", stateCount[TRAVEL_STATE_TRAVEL_DO_QUEST]);
    sLog.outString("    try to do quest: %d", stateCount[TRAVEL_STATE_WORK_DO_QUEST]);
    sLog.outString("    travel to hand in quest: %d", stateCount[TRAVEL_STATE_TRAVEL_HAND_IN_QUEST]);
    sLog.outString("    try to hand in quest: %d", stateCount[TRAVEL_STATE_WORK_HAND_IN_QUEST]);
    sLog.outString("    idling: %d", stateCount[TRAVEL_STATE_IDLE]);


    sort(questCount.begin(), questCount.end(), [](pair<Quest const*, int32> i, pair<Quest const*, int32> j) {return i.second > j.second; });

    sLog.outString("Bots top quests:");

    int cnt = 0;
    for (auto& quest : questCount)
    {
        sLog.outString("    [%d]: %s (%d)", quest.second, quest.first->GetTitle().c_str(), quest.first->GetQuestLevel());
        cnt++;
        if (cnt > 25)
            break;
    }
}

double RandomPlayerbotMgr::GetBuyMultiplier(Player* bot)
{
    uint32 id = bot->GetGUIDLow();
    uint32 value = GetEventValue(id, "buymultiplier");
    if (!value)
    {
        value = urand(50, 120);
        uint32 validIn = urand(sPlayerbotAIConfig.minRandomBotsPriceChangeInterval, sPlayerbotAIConfig.maxRandomBotsPriceChangeInterval);
        SetEventValue(id, "buymultiplier", value, validIn);
    }

    return (double)value / 100.0;
}

double RandomPlayerbotMgr::GetSellMultiplier(Player* bot)
{
    uint32 id = bot->GetGUIDLow();
    uint32 value = GetEventValue(id, "sellmultiplier");
    if (!value)
    {
        value = urand(80, 250);
        uint32 validIn = urand(sPlayerbotAIConfig.minRandomBotsPriceChangeInterval, sPlayerbotAIConfig.maxRandomBotsPriceChangeInterval);
        SetEventValue(id, "sellmultiplier", value, validIn);
    }

    return (double)value / 100.0;
}

void RandomPlayerbotMgr::AddTradeDiscount(Player* bot, Player* master, int32 value)
{
    if (!master) return;
    uint32 discount = GetTradeDiscount(bot, master);
    int32 result = (int32)discount + value;
    discount = (result < 0 ? 0 : result);

    SetTradeDiscount(bot, master, discount);
}

void RandomPlayerbotMgr::SetTradeDiscount(Player* bot, Player* master, uint32 value)
{
    if (!master) return;
    uint32 botId =  bot->GetGUIDLow();
    uint32 masterId =  master->GetGUIDLow();
    ostringstream name; name << "trade_discount_" << masterId;
    SetEventValue(botId, name.str(), value, sPlayerbotAIConfig.maxRandomBotInWorldTime);
}

uint32 RandomPlayerbotMgr::GetTradeDiscount(Player* bot, Player* master)
{
    if (!master) return 0;
    uint32 botId =  bot->GetGUIDLow();
    uint32 masterId = master->GetGUIDLow();
    ostringstream name; name << "trade_discount_" << masterId;
    return GetEventValue(botId, name.str());
}

string RandomPlayerbotMgr::HandleRemoteCommand(string request)
{
    string::iterator pos = find(request.begin(), request.end(), ',');
    if (pos == request.end())
    {
        ostringstream out; out << "invalid request: " << request;
        return out.str();
    }

    string command = string(request.begin(), pos);
    uint64 guid = atoi(string(pos + 1, request.end()).c_str());
    Player* bot = GetPlayerBot(guid);
    if (!bot)
        return "invalid guid";

    PlayerbotAI *ai = bot->GetPlayerbotAI();
    if (!ai)
        return "invalid guid";

    return ai->HandleRemoteCommand(command);
}

void RandomPlayerbotMgr::ChangeStrategy(Player* player)
{
    uint32 bot = player->GetGUIDLow();

    if (urand(0, 100) > 100 * sPlayerbotAIConfig.randomBotRpgChance) // select grind / pvp
    {
        sLog.outDetail("Changing strategy for bot #%d <%s> to grinding", bot, player->GetName());
        ScheduleTeleport(bot, 30);
    }
    else
    {
        sLog.outDetail("Changing strategy for bot #%d <%s> to RPG", bot, player->GetName());
		sLog.outBasic("Bot #%d <%s>: sent to inn", bot, player->GetName());
        RandomTeleportForRpg(player);
		SetEventValue(bot, "teleport", 1, sPlayerbotAIConfig.maxRandomBotInWorldTime);
    }

    ScheduleChangeStrategy(bot);
}

void RandomPlayerbotMgr::RandomTeleportForRpg(Player* bot)
{
    uint32 race = bot->getRace();
	uint32 level = bot->getLevel();
    sLog.outDetail("Random teleporting bot %s for RPG (%zu locations available)", bot->GetName(), rpgLocsCacheLevel[race][level].size());
    RandomTeleport(bot, rpgLocsCacheLevel[race][level], true);
	Refresh(bot);
}

void RandomPlayerbotMgr::Remove(Player* bot)
{
    uint64 owner = bot->GetObjectGuid().GetRawValue();
    PlayerbotDatabase.PExecute("delete from ai_playerbot_random_bots where owner = 0 and bot = '%lu'", owner);
    eventCache[owner].clear();

    LogoutPlayerBot(owner);
}

uint32 RandomPlayerbotMgr::GetBattleMasterEntryByRace(uint8 race)
{
    switch (race)
    {
    case RACE_HUMAN:       return 14981;
    case RACE_ORC:         return 3890;
    case RACE_DWARF:       return 14982;
    case RACE_NIGHTELF:    return 2302;
    case RACE_UNDEAD:      return 2804;
    case RACE_TAUREN:      return 10360;
    case RACE_GNOME:       return 14982;
    case RACE_TROLL:       return 3890;
#ifndef MANGOSBOT_ZERO
    case RACE_DRAENEI:     return 20118;
    case RACE_BLOODELF:    return 16696;
#endif
    default:    return 0;
    }
}

uint32 RandomPlayerbotMgr::GetBattleMasterGuidByRace(uint8 race)
{
    uint32 guid = 0;
    int entry = GetBattleMasterEntryByRace(race);
    if (entry)
        guid = GetCreatureGuidByEntry(entry);
    return guid;
}

const CreatureDataPair* RandomPlayerbotMgr::GetCreatureDataByEntry(uint32 entry)
{
    if (entry != 0 && ObjectMgr::GetCreatureTemplate(entry))
    {
        FindCreatureData worker(entry, NULL);
        sObjectMgr.DoCreatureData(worker);
        CreatureDataPair const* dataPair = worker.GetResult();
        return dataPair;
    }
    return NULL;
}

uint32 RandomPlayerbotMgr::GetCreatureGuidByEntry(uint32 entry)
{
    uint32 guid = 0;

    CreatureDataPair const* dataPair = sRandomPlayerbotMgr.GetCreatureDataByEntry(entry);
    guid = dataPair->first;

    return guid;
}

uint32 RandomPlayerbotMgr::GetBattleMasterEntry(Player* bot, BattleGroundTypeId bgTypeId)
{
    Team team = bot->GetTeam();
    uint32 entry = 0;
    vector<uint32> Bms;

    for (auto i = begin(BattleMastersCache[team][bgTypeId]); i != end(BattleMastersCache[team][bgTypeId]); ++i)
    {
        Bms.insert(Bms.end(), *i);
    }

    for (auto i = begin(BattleMastersCache[TEAM_BOTH_ALLOWED][bgTypeId]); i != end(BattleMastersCache[TEAM_BOTH_ALLOWED][bgTypeId]); ++i)
    {
        Bms.insert(Bms.end(), *i);
    }

    if (Bms.empty())
        return entry;

    float dist1 = 10000.0f;
    for (auto j = 0; j < 3; ++j)
    {
        for (auto i = begin(Bms); i != end(Bms); ++i)
        {
            if (entry && j == 2)
                break;

            CreatureDataPair const* dataPair = sRandomPlayerbotMgr.GetCreatureDataByEntry(*i);
            if (!dataPair)
                continue;

            CreatureData const* data = &dataPair->second;
            if (bot->GetMapId() != data->mapid && j == 0)
                continue;

            Unit* Bm = sMapMgr.FindMap(data->mapid)->GetUnit(ObjectGuid(HIGHGUID_UNIT, *i, dataPair->first));
            if (!Bm)
                continue;

            AreaTableEntry const* area = GetAreaEntryByAreaID(Bm->GetAreaId());
            if (area->team == 4 && bot->GetTeam() == ALLIANCE)
                continue;
            if (area->team == 2 && bot->GetTeam() == HORDE)
                continue;

            if (Bm->GetDeathState() == DEAD)
                continue;

            float dist2 = sServerFacade.GetDistance2d(bot, data->posX, data->posY);
            if (dist2 < dist1)
            {
                dist1 = dist2;
                entry = *i;
            }
        }
    }

    return entry;
}

void RandomPlayerbotMgr::Hotfix(Player* bot, uint32 version)
{
    PlayerbotFactory factory(bot, bot->getLevel());
    uint32 exp = bot->GetUInt32Value(PLAYER_XP);
    uint32 level = bot->getLevel();
    uint32 id = bot->GetGUIDLow();

    for (int fix = version; fix <= MANGOSBOT_VERSION; fix++)
    {
        int count = 0;
        switch (fix)
        {
            case 1: // Apply class quests to previously made random bots

                if (level < 10)
                {
                    break;
                }

                for (list<uint32>::iterator i = factory.classQuestIds.begin(); i != factory.classQuestIds.end(); ++i)
                {
                    uint32 questId = *i;
                    Quest const *quest = sObjectMgr.GetQuestTemplate(questId);

                    if (!bot->SatisfyQuestClass(quest, false) ||
                        quest->GetMinLevel() > bot->getLevel() ||
                        !bot->SatisfyQuestRace(quest, false) || bot->GetQuestStatus(questId) == QUEST_STATUS_COMPLETE)
                        continue;

                    bot->SetQuestStatus(questId, QUEST_STATUS_COMPLETE);
                    bot->RewardQuest(quest, 0, bot, false);
                    bot->SetLevel(level);
                    bot->SetUInt32Value(PLAYER_XP, exp);
                    sLog.outDetail("Bot %d rewarded quest %d",
                        bot->GetGUIDLow(), questId);
                    count++;
                }

                if (count > 0)
                {
                    sLog.outDetail("Bot %d hotfix (Class Quests), %d quests rewarded",
                        bot->GetGUIDLow(), count);
                    count = 0;
                }
                break;
            case 2: // Init Riding skill fix

                if (level < 20)
                {
                    break;
                }
                factory.InitSkills();
                sLog.outDetail("Bot %d hotfix (Riding Skill) applied",
                    bot->GetGUIDLow());
                break;

            default:
                break;
        }
    }
    SetValue(bot, "version", MANGOSBOT_VERSION);
    sLog.outBasic("Bot %d hotfix v%d applied",
        bot->GetGUIDLow(), MANGOSBOT_VERSION);
}
