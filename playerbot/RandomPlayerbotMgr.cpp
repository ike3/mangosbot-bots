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
    if (!maxAllowedBotCount)
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
        
    SetNextCheckDelay( (1000 * sPlayerbotAIConfig.randomBotUpdateInterval) / notDiv);

    list<uint32> bots = GetBots();
    int botCount = bots.size();

    PerformanceMonitorOperation *pmo = sPerformanceMonitor.start(PERF_MON_TOTAL,
            playerBots.size() < maxAllowedBotCount ? "RandomPlayerbotMgr::Login" : "RandomPlayerbotMgr::UpdateAIInternal");

    if (botCount < maxAllowedBotCount)
    {
        AddRandomBots();
    }
        

    int botProcessed = 0;
    for (list<uint32>::iterator i = bots.begin(); i != bots.end(); ++i)
    {
        uint32 bot = *i;
		if (ProcessBot(bot)) {
			botProcessed++;
		}

        if (botProcessed >= sPlayerbotAIConfig.randomBotsPerInterval)
            break;
    }

    if (pmo) pmo->finish();
}

uint32 RandomPlayerbotMgr::AddRandomBots()
{
    set<uint32> bots;

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

    vector<uint32> guids;
    int maxAllowedBotCount = GetEventValue(0, "bot_count");
    for (list<uint32>::iterator i = sPlayerbotAIConfig.randomBotAccounts.begin(); i != sPlayerbotAIConfig.randomBotAccounts.end(); i++)
    {
        uint32 accountId = *i;
        if (!sAccountMgr.GetCharactersCount(accountId))
            continue;

        QueryResult* result = CharacterDatabase.PQuery("SELECT guid, race FROM characters WHERE account = '%u'", accountId);
        if (!result)
            continue;

        do
        {
            Field* fields = result->Fetch();
            uint32 guid = fields[0].GetUInt32();
            uint8 race = fields[1].GetUInt8();
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
                sLog.outBasic( "New random bot %d added", bot);
                if (bots.size() >= maxAllowedBotCount)
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

void RandomPlayerbotMgr::CheckBgQueue(BattleGroundTypeId bgTypeId, BattleGroundBracketId bracketId)
{
    if(!BgCheckTimer)
        BgCheckTimer = time(NULL);

    if (time(NULL) < (BgCheckTimer + 15))
    {
        return;
    }
    else
    {
        BgCheckTimer = time(NULL);
    }

    sLog.outBasic("Checking BG Queue...");

    for (int i = BG_BRACKET_ID_FIRST; i < MAX_BATTLEGROUND_BRACKETS; ++i)
    {
        for (int j = BATTLEGROUND_AV; j < BATTLEGROUND_AB; ++j)
        {
            BracketPlayers[j][i][0] = 0;
            BracketPlayers[j][i][1] = 0;
            BracketBots[j][i][0] = 0;
            BracketBots[j][i][1] = 0;
        }
    }

    /*QueryResult* result = CharacterDatabase.Query("SELECT guid, instance_id, team FROM character_battleground_data");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 guidlow = fields[0].GetUInt32();
            uint32 instanceId = fields[1].GetUInt32();
            uint32 Team = fields[1].GetUInt32();
            BattleGround* bg = NULL;
            if (instanceId)
            {
                bg = sBattleGroundMgr.GetBattleGroundThroughClientInstance(instanceId, bgTypeId);
            }
            if (!bg)
                continue;

            if (bg->GetStatus() == STATUS_WAIT_LEAVE)
                continue;

            //if (bg->GetTypeID() != bgTypeId)
                //continue;

            //if (bg->GetBracketId() != bracketId)
                //continue;

            uint32 TeamId = (Team == ALLIANCE ? 0 : 1);

            //BracketPlayers[bgTypeId][bracketId][TeamId]++;
            if (IsRandomBot(guidlow))
                continue;//BracketBots[bg->GetTypeID()][bg->GetBracketId()][TeamId]++;
            else
                BracketPlayers[bg->GetTypeID()][bg->GetBracketId()][TeamId]++;

        } while (result->NextRow());
        delete result;
    }*/

    for (vector<Player*>::iterator i = players.begin(); i != players.end(); ++i)
    {
        Player* player = *i;

        if (!player->InBattleGroundQueue())
            continue;

        if (player->InBattleGround() && player->GetBattleGround()->GetStatus() == STATUS_WAIT_LEAVE)
            continue;

        uint32 TeamId = player->GetTeamId();
        //uint32 qindex = player->GetBattleGroundQueueIndex(BattleGroundQueueTypeId(bgTypeId));
        BattleGroundBracketId bracket = player->GetBattleGroundBracketIdFromLevel(bgTypeId);
        BattleGroundQueueTypeId queue = player->GetBattleGroundQueueTypeId(0);
        if (queue == BATTLEGROUND_QUEUE_NONE)
            continue;
        /*if (bracket == bracketId && queue == bgTypeId)
        {
            BracketPlayers[bgTypeId][bracketId][TeamId]++;
        }*/
        if(player->GetPlayerbotAI())
            BracketBots[queue][bracket][TeamId]++;
        else
            BracketPlayers[queue][bracket][TeamId]++;
    }

    for (PlayerBotMap::iterator i = playerBots.begin(); i != playerBots.end(); ++i)
    {
        Player* bot = i->second;

        if (!bot->InBattleGroundQueue())
            continue;

        if (bot->GetBattleGround() && bot->GetBattleGround()->GetStatus() == STATUS_WAIT_LEAVE)
            continue;

        uint32 TeamId = bot->GetTeamId();
        //uint32 qindex = bot->GetBattleGroundQueueIndex(BattleGroundQueueTypeId(bgTypeId));
        BattleGroundBracketId bracket = bot->GetBattleGroundBracketIdFromLevel(bgTypeId);
        BattleGroundQueueTypeId queue = bot->GetBattleGroundQueueTypeId(0);
        if (queue == BATTLEGROUND_QUEUE_NONE)
            continue;

        BracketBots[queue][bracket][TeamId]++;
    }
    
    for (int i = BG_BRACKET_ID_FIRST; i < MAX_BATTLEGROUND_BRACKETS; ++i)
    {
        for (int j = BATTLEGROUND_AV; j < BATTLEGROUND_AB; ++j)
        {
            // don't show other BG than WSG just yet
            if (j != BATTLEGROUND_WS)
                continue;

            if ((BracketPlayers[j][i][0] + BracketBots[j][i][0] + BracketPlayers[j][i][1] + BracketBots[j][i][1]) == 0)
                continue;

            sLog.outBasic("WSG %s: P (%d:%d) B (%d:%d) Total (A:%d H:%d)",
                i == 0 ? "10-19" : i == 1 ? "20-29" : i == 2 ? "30-39" : i == 3 ? "40-49" : i == 4 ? "50-59" : (i == 5 && MAX_BATTLEGROUND_BRACKETS == 6) ? "60" : (i == 5 && MAX_BATTLEGROUND_BRACKETS == 7) ? "60-69" : i == 6 ? "70" : "",
                BracketPlayers[j][i][0],
                BracketPlayers[j][i][1],
                BracketBots[j][i][0],
                BracketBots[j][i][1],
                BracketPlayers[j][i][0] + BracketBots[j][i][0],
                BracketPlayers[j][i][1] + BracketBots[j][i][1]
                );
        }
    }

    sLog.outBasic("BG Queue check finished");

    //if (BracketPlayers[bgTypeId][bracketId][0] > 0 || BracketPlayers[bgTypeId][bracketId][1] > 0)
        //return;

    return;
}

void RandomPlayerbotMgr::AddBgBot(Player* player, BattleGroundTypeId bgTypeId, BattleGroundBracketId bracketId, bool visual)
{
    BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
    if (!bg)
        return;

    if (player->getLevel() < bg->GetMinLevel())
        return;

    uint32 BracketSize = bg->GetMaxPlayers();
    uint32 TeamSize = bg->GetMaxPlayersPerTeam();
    uint32 bot = player->GetGUIDLow();
    PlayerbotAI* ai = player->GetPlayerbotAI();
    uint32 TeamId = player->GetTeamId();

    uint32 ACount = BracketBots[bgTypeId][bracketId][0] + BracketPlayers[bgTypeId][bracketId][0];
    uint32 HCount = BracketBots[bgTypeId][bracketId][1] + BracketPlayers[bgTypeId][bracketId][1];

    /*if (ACount > HCount && TeamId == 0)
    {
        return;
    }

    if (HCount > ACount && TeamId == 1)
    {
        return;
    }*/

    /*if ((ACount > HCount) && (ACount - HCount > 3) && TeamId == 0)
    {
        return;
    }

    if ((HCount > ACount) && (HCount - ACount > 3) && TeamId == 1)
    {
        return;
    }*/

    if ((ACount >= TeamSize && TeamId == 0) || (HCount >= TeamSize && TeamId == 1))
        return;

    uint32 BgCount = ACount + HCount;

    if (BgCount >= BracketSize)
        return;
    //if (!(BgCount < BracketSize && BracketBots[bgTypeId][bracketId][player->GetTeamId()] < TeamSize))
    //{
    //    return;
    //}

    TeamId == 0 ? ACount++ : HCount++;

    if (!player->GetGroup() && !ai->GetMaster())
    {
        if (!visual)
        {
            sLog.outDetail("Changing strategy for bot %s to PVP", player->GetName());
            sLog.outDetail("Bot %d (%d %s) bracket %d sent to BattmeMaster", bot, player->getLevel(), player->GetTeamId() == 0 ? "A" : "H", bracketId);
            sLog.outBasic("Bot %d (%d %s) joins BG bracket %d (%d/%d) (A:%d H:%d)", bot, player->getLevel(), TeamId == 0 ? "A" : "H", bracketId, BgCount + 1, BracketSize, ACount, HCount);
            // BG Tactics preference
            SetEventValue(bot, "preference", urand(0, 9), sPlayerbotAIConfig.maxRandomBotInWorldTime);
        }
        else
        {
            sLog.outDetail("Bot %d (%d %s) simulates waiting near BattleMaster", bot, player->getLevel(), player->GetTeamId() == 0 ? "A" : "H");
        }

        // Find BattleMaster by Entry
        uint32 BmEntry = player->GetPlayerbotAI()->GetBattleMasterEntryByRace(player->getRace());

        // if found entry
        if (BmEntry != 0 && ObjectMgr::GetCreatureTemplate(BmEntry))
        {
            CreatureData const* data = player->GetPlayerbotAI()->GetCreatureDataByEntry(BmEntry);
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
                player->TeleportTo(data->mapid, data->posX, data->posY, data->posZ, player->GetOrientation());
            }
        }
        if (!BmEntry || BmEntry == 0)
        {
            sLog.outError("Bot %d could not find Battlemaster", player->GetGUIDLow());
            return;
        }

        player->GetPlayerbotAI()->ChangeStrategy("-grind,-rpg,-custom::say", BOT_STATE_NON_COMBAT);
        //player->GetPlayerbotAI()->ChangeStrategy("-rpg", BOT_STATE_NON_COMBAT);
        if (urand(0, 5) < 3)
            player->GetPlayerbotAI()->ChangeStrategy("+stay", BOT_STATE_NON_COMBAT);
        //if (urand(0, 5) < 2)
        //    player->GetPlayerbotAI()->ChangeStrategy("-collision", BOT_STATE_NON_COMBAT);
        player->GetPlayerbotAI()->ChangeStrategy("-mount", BOT_STATE_NON_COMBAT);

        if (visual)
        {
            VisualBots[bgTypeId][bracketId][player->GetTeamId()]++;
            if (urand(0, 5) < 3)
                player->GetPlayerbotAI()->ChangeStrategy("+rpg,-stay", BOT_STATE_NON_COMBAT);
            return;

        }
        Refresh(player);
        BracketBots[bgTypeId][bracketId][player->GetTeamId()]++;
        player->GetPlayerbotAI()->ChangeStrategy("+bg", BOT_STATE_NON_COMBAT);

        return;
    }
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
			sLog.outString("Bot %d expired", bot);
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
        if (urand(0, 100) > 50) // less lag during bots login
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
        uint32 randomTime = urand(sPlayerbotAIConfig.minRandomBotReviveTime, sPlayerbotAIConfig.maxRandomBotReviveTime);
        SetEventValue(bot, "update", 1, randomTime);
        return true;
    }

    uint32 logout = GetEventValue(bot, "logout");
    if (!logout)
    {
        sLog.outDetail("Bot %d logged out", bot);
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
    if (!sServerFacade.UnitIsDead(player))
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
    }

    if (sServerFacade.UnitIsDead(player) && !player->InBattleGround())
    {
        if (!GetEventValue(bot, "dead"))
        {
            uint32 deathcount = GetEventValue(bot, "deathcount");
            if (!deathcount)
            {
                SetEventValue(bot, "deathcount", 1, sPlayerbotAIConfig.maxRandomBotInWorldTime);
                Revive(player);
                sLog.outBasic("Bot %d revived", bot);
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
                    sLog.outString("Bot %d died %d times and is sent to city for %d minutes", bot, deathcount, int(randomChange / 60));
                }
                else
                {
                    SetEventValue(bot, "deathcount", deathcount + 1, sPlayerbotAIConfig.maxRandomBotInWorldTime);
                    Revive(player);
                    sLog.outBasic("Bot %d revived %d/5", bot, deathcount + 1);
                }
            }
           return false; // increase revive rate
        }
    }

	if (urand(0, 100) > 20) // move optimisation to the next step
	{
		return true;
	}

	player->GetPlayerbotAI()->GetAiObjectContext()->GetValue<bool>("random bot update")->Set(false);

    bool BgPlayers = false;
    bool BgLevel = true;
    bool BgVisual = false;

    // get BG bracketId
    uint32 bgType = BATTLEGROUND_WS;
    BattleGroundTypeId bgTypeId = BattleGroundTypeId(bgType);
    BattleGroundBracketId bracketId = player->GetBattleGroundBracketIdFromLevel(bgTypeId);

    if (sPlayerbotAIConfig.randomBotJoinBG && player->getLevel() > 9)
    {
        // check bg queue for real players
        CheckBgQueue(bgTypeId, bracketId);

        // checking if players found
        if (BracketPlayers[bgTypeId][bracketId][0] > 0 || BracketPlayers[bgTypeId][bracketId][1] > 0)
            BgPlayers = true;

        // add only x3 - x9 level
        if (bracketId < BG_BRACKET_ID_LAST && (player->getLevel() < ((bracketId * 10) + 14)))
            BgLevel = false;

        if (!BgPlayers && VisualBots[bgTypeId][bracketId][player->GetTeamId()] < 3)
            BgVisual = true;
    }

    // if yes, then bot can queue to BG
    if ((BgPlayers || BgVisual) && sPlayerbotAIConfig.randomBotJoinBG && BgLevel && player->getLevel() > 9 && !player->GetGroup() && !player->GetPlayerbotAI()->GetMaster() && urand(0, 100) > 30)
    {
        // check if bot is not attacked
        bool noattackers = (player->IsInCombat() || player->getAttackers().size() > 0) ? false : true;

        // check bracketId && MinLevel
        if (noattackers && player->GetBGAccessByLevel(bgTypeId))
        {
            AddBgBot(player, bgTypeId, bracketId, BgVisual);
            return true;
        }
    }

    bool randomiser = true;
    if (player->GetGuildId())
    {
		Guild* guild = sGuildMgr.GetGuildById(player->GetGuildId());
		if (guild->GetLeaderGuid().GetRawValue() == player->GetObjectGuid().GetRawValue()) {
			for (vector<Player*>::iterator i = players.begin(); i != players.end(); ++i)
				sGuildTaskMgr.Update(*i, player);
		}

        if (!IsRandomBot(guild->GetLeaderGuid())) {
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
        else
        {
            RandomTeleportForRpg(player);
        }

		SetEventValue(bot, "teleport", 1, sPlayerbotAIConfig.maxRandomBotInWorldTime);
		uint32 randomChange = urand(sPlayerbotAIConfig.randomBotUpdateInterval * 5, sPlayerbotAIConfig.randomBotUpdateInterval * 15);
		ScheduleChangeStrategy(bot, randomChange);

        if (randomiser)
        {
            sLog.outString("Bot %d is randomized and sent to city", bot);
        }
        else
        {
            sLog.outString("Bot %d from <%s> is refreshed and sent to city", bot, sGuildMgr.GetGuildById(player->GetGuildId())->GetName());
        }

        uint32 randomTime = urand(sPlayerbotAIConfig.minRandomBotRandomizeTime, sPlayerbotAIConfig.maxRandomBotRandomizeTime);
        ScheduleRandomize(bot, randomTime);
        return true;
    }

	uint32 teleport = GetEventValue(bot, "teleport");
	if (!teleport)
	{
		sLog.outString("Bot %d is grinding", bot);
		RandomTeleportForLevel(player);
		Refresh(player);
		SetEventValue(bot, "teleport", 1, sPlayerbotAIConfig.maxRandomBotInWorldTime);
		return true;
	}

	uint32 changeStrategy = GetEventValue(bot, "change_strategy");
	if (!changeStrategy)
	{
		sLog.outDetail("Changing strategy for bot %d", bot);
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

void RandomPlayerbotMgr::RandomTeleport(Player* bot, vector<WorldLocation> &locs)
{
    if (bot->IsBeingTeleported())
        return;

    if (bot->InBattleGround())
        return;

	if (bot->getLevel() < 5)
		return;

    if (locs.empty())
    {
        sLog.outError("Cannot teleport bot %s - no locations available", bot->GetName());
        return;
    }

    vector<WorldLocation> tlocs = locs;

    //5% + 0.1% per level chance node on different map in selection.
    tlocs.erase(std::remove_if(tlocs.begin(), tlocs.end(), [bot](WorldLocation const& l) {return l.mapid != bot->GetMapId() && urand(1, 100) > 5 + 0.1 * bot->getLevel(); }), tlocs.end());

    //Continent is about 20.000 large
    //Bot will travel 0-5000 units + 75-150 units per level.
    tlocs.erase(std::remove_if(tlocs.begin(), tlocs.end(), [bot](WorldLocation const& l) {return  bot->GetDistance2d(l.coord_x, l.coord_y) > urand(0, 5000) + bot->getLevel() * 15 * urand(5, 10); }), tlocs.end());

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
    else
        IncreaseLevel(bot);

    SetValue(bot, "version", MANGOSBOT_VERSION);
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

	if (level > 4) {
		RandomTeleportForRpg(bot);
	}
    if (pmo) pmo->finish();
}

void RandomPlayerbotMgr::RandomizeFirst(Player* bot)
{
	uint32 maxLevel = sPlayerbotAIConfig.randomBotMaxLevel;
	if (maxLevel > sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
		maxLevel = sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL);

	PerformanceMonitorOperation *pmo = sPerformanceMonitor.start(PERF_MON_RNDBOT, "RandomizeFirst");
    uint32 level = urand(sPlayerbotAIConfig.randomBotMinLevel, maxLevel);

    if (urand(0, 100) < 100 * sPlayerbotAIConfig.randomBotMaxLevelChance)
        level = maxLevel;

    SetValue(bot, "level", level);
    PlayerbotFactory factory(bot, level);
    factory.Randomize(false);

	if (level > 4) {
		RandomTeleportForRpg(bot);
	}
	
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

    sLog.outDetail("Refreshing bot %s", bot->GetName());
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
                    ai->ChangeStrategy("-rpg", BOT_STATE_NON_COMBAT);
                    ai->ChangeStrategy("-grind", BOT_STATE_NON_COMBAT);
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
        default:
            dps++;
            break;
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
        sLog.outDetail("Changing strategy for bot %s to grinding", player->GetName());
        sLog.outString("Bot %d sent to grind", bot);
        ScheduleTeleport(bot, 30);
    }
    else
    {
        sLog.outDetail("Changing strategy for bot %s to RPG", player->GetName());
		sLog.outString("Bot %d sent to city", bot);
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
    RandomTeleport(bot, rpgLocsCacheLevel[race][level]);
	Refresh(bot);
}

void RandomPlayerbotMgr::Remove(Player* bot)
{
    uint64 owner = bot->GetObjectGuid().GetRawValue();
    CharacterDatabase.PExecute("delete from ai_playerbot_random_bots where owner = 0 and bot = '%lu'", owner);
    eventCache[owner].clear();

    LogoutPlayerBot(owner);
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
