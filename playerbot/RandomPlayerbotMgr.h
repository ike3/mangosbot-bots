#ifndef _RandomPlayerbotMgr_H
#define _RandomPlayerbotMgr_H

#include "Common.h"
#include "PlayerbotAIBase.h"
#include "PlayerbotMgr.h"
#include "PlayerbotAIConfig.h"

class WorldPacket;
class Player;
class Unit;
class Object;
class Item;

using namespace std;

class CachedEvent
{
public:
    CachedEvent() : value(0), lastChangeTime(0), validIn(0) {}
    CachedEvent(const CachedEvent& other) : value(other.value), lastChangeTime(other.lastChangeTime), validIn(other.validIn) {}
    CachedEvent(uint32 value, uint32 lastChangeTime, uint32 validIn) : value(value), lastChangeTime(lastChangeTime), validIn(validIn) {}

public:
    bool IsEmpty() { return !lastChangeTime; }

public:
    uint32 value, lastChangeTime, validIn;
};

class RandomPlayerbotMgr : public PlayerbotHolder
{
    public:
        RandomPlayerbotMgr();
        virtual ~RandomPlayerbotMgr();
        static RandomPlayerbotMgr& instance()
        {
            static RandomPlayerbotMgr instance;
            return instance;
        }

        virtual void UpdateAIInternal(uint32 elapsed);

	public:
        static bool HandlePlayerbotConsoleCommand(ChatHandler* handler, char const* args);
        bool IsRandomBot(Player* bot);
        bool IsRandomBot(uint32 bot);
        void Randomize(Player* bot);
        void RandomizeFirst(Player* bot);
        void IncreaseLevel(Player* bot);
        void ScheduleTeleport(uint32 bot, uint32 time = 0);
        void ScheduleChangeStrategy(uint32 bot, uint32 time = 0);
        void HandleCommand(uint32 type, const string& text, Player& fromPlayer);
        string HandleRemoteCommand(string request);
        void OnPlayerLogout(Player* player);
        void OnPlayerLogin(Player* player);
        void OnPlayerLoginError(uint32 bot);
        Player* GetRandomPlayer();
        vector<Player*> GetPlayers() { return players; };
        PlayerBotMap GetAllBots() { return playerBots; };
        void PrintStats();
        double GetBuyMultiplier(Player* bot);
        double GetSellMultiplier(Player* bot);
        void AddTradeDiscount(Player* bot, Player* master, int32 value);
        void SetTradeDiscount(Player* bot, Player* master, uint32 value);
        uint32 GetTradeDiscount(Player* bot, Player* master);
        void Refresh(Player* bot);
        void RandomTeleportForLevel(Player* bot);
        void RandomTeleportForRpg(Player* bot);
        int GetMaxAllowedBotCount();
        bool ProcessBot(Player* player);
        void Revive(Player* player);
        void ChangeStrategy(Player* player);
        uint32 GetValue(Player* bot, string type);
        uint32 GetValue(uint32 bot, string type);
        void SetValue(uint32 bot, string type, uint32 value);
        void SetValue(Player* bot, string type, uint32 value);
        void Remove(Player* bot);
        void Hotfix(Player* player, uint32 version);
        uint32 GetBattleMasterEntryByRace(uint8 race);
        uint32 GetBattleMasterEntry(Player* bot, BattleGroundTypeId bgTypeId);
        uint32 GetBattleMasterGuidByRace(uint8 race);
        const CreatureDataPair* GetCreatureDataByEntry(uint32 entry);
        uint32 GetCreatureGuidByEntry(uint32 entry);
        void LoadBattleMastersCache();
        bool BgBotsActive;
        map<uint32, map<uint32, map<uint32, uint32> > > BgBots;
        map<uint32, map<uint32, map<uint32, uint32> > > VisualBots;
        map<uint32, map<uint32, map<uint32, uint32> > > BgPlayers;
        map<uint32, map<uint32, map<uint32, map<uint32, uint32> > > > ArenaBots;
        map<uint32, map<uint32, map<uint32, uint32> > > Rating;
        map<uint32, map<uint32, map<uint32, uint32> > > Supporters;
        map<Team, vector<uint32>> LfgDungeons;
        void CheckBgQueue();
        void CheckLfgQueue();
        void CheckPlayers();

	protected:
	    virtual void OnBotLoginInternal(Player * const bot);

    private:
        uint32 GetEventValue(uint32 bot, string event);
        uint32 SetEventValue(uint32 bot, string event, uint32 value, uint32 validIn);
        list<uint32> GetBots();
        list<uint32> GetBgBots(uint32 bracket);
        void AddBgBot(BattleGroundQueueTypeId queueTypeId, BattleGroundBracketId bracketId, bool isRated = false, bool visual = false);
        time_t BgCheckTimer;
        time_t LfgCheckTimer;
        time_t PlayersCheckTimer;
        uint32 AddRandomBots();
        bool ProcessBot(uint32 bot);
        void ScheduleRandomize(uint32 bot, uint32 time);
        void RandomTeleport(Player* bot);
        void RandomTeleport(Player* bot, vector<WorldLocation> &locs, bool hearth = false);
        uint32 GetZoneLevel(uint16 mapId, float teleX, float teleY, float teleZ);
        void PrepareTeleportCache();
        typedef void (RandomPlayerbotMgr::*ConsoleCommandHandler) (Player*);

    private:
        vector<Player*> players;
        int processTicks;
        map<uint8, vector<WorldLocation> > locsPerLevelCache;
        map<uint32, vector<WorldLocation> > rpgLocsCache;
		map<uint32, map<uint32, vector<WorldLocation> > > rpgLocsCacheLevel;
        map<Team, map<BattleGroundTypeId, list<uint32> > > BattleMastersCache;
        map<uint32, map<string, CachedEvent> > eventCache;
        BarGoLink* loginProgressBar;
        list<uint32> currentBots;
        uint32 bgBotsCount;
        uint32 playersLevel = sPlayerbotAIConfig.randombotStartingLevel;
};

#define sRandomPlayerbotMgr RandomPlayerbotMgr::instance()

#endif
