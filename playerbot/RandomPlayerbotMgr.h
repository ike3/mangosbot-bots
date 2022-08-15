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
    CachedEvent() : value(0), lastChangeTime(0), validIn(0), data("") {}
    CachedEvent(const CachedEvent& other) : value(other.value), lastChangeTime(other.lastChangeTime), validIn(other.validIn), data(other.data) {}
    CachedEvent(uint32 value, uint32 lastChangeTime, uint32 validIn, string data = "") : value(value), lastChangeTime(lastChangeTime), validIn(validIn), data(data) {}

public:
    bool IsEmpty() { return !lastChangeTime; }

public:
    uint32 value, lastChangeTime, validIn;
    string data;
};

class PerformanceMonitorOperation;

//https://gist.github.com/bradley219/5373998

class botPIDImpl;
class botPID
{
public:
    // Kp -  proportional gain
    // Ki -  Integral gain
    // Kd -  derivative gain
    // dt -  loop interval time
    // max - maximum value of manipulated variable
    // min - minimum value of manipulated variable
    botPID(double dt, double max, double min, double Kp, double Kd, double Ki);
    void adjust(double Kp, double Kd, double Ki);

    //To understand the values chosen below it's good to remember that 
    //-The goal (dif) is 100.
    //-Activity percentage scales the 'active alone bots' by a perencatage between 100% and 0%
    //-The activity mod starts at 100% and can be lowered and increased by the controller.
    //
    //The controller has a dt of 0.1 because each tick should take 0.1 seconds
    //The controller has max = 100 and min = -100 which means it can raise or lower the activity by 100% in one tick.
    //The proportional gian is set to 0.01 which means at a 200 diff (which is 100 dif above goal) it will lower the activity by 1% each tick.
    //The integral gain is set to 0.01 which means at 110 diff (which is 10 diff above goal) it will start lowering the activity by 1% each tick after 100 ticks if this increase persists.
    //The derivative gain is set to 0.01 which means if the diff was 200 last tick and 190 now (which decreasing at a speed of 10 per tick) it will increase the activity mod by 1% each tick (dampening the speed)
    //These 3 gains are summed to result in a total increase or decrease.
    //
    //For example the diff was stable at 100 for a while but jumped to 200 (because of player login) then dropped to 190.
    //The activity mod was 100%
    //The (internal) error will be -90 and the delta is 10.
    //Integral will be -90*0.01*0.1 = -0.09%
    //Proportional is -90*0.01= = -0.9% 
    //Derivative is 10*0.01/0.1= 1% per tick. 
    //The sum is +0.01% which means the activity actually stays at 100% bececause the diff was already moving the right direction.

    //Now next tick diff is still 190.
    //This means derivative drops to 0, the integral doubles to -0.18% so the activity will be lowered 1.08% this tick.
    
    double calculate(double setpoint, double pv);
    ~botPID();

private:
    botPIDImpl* pimpl;
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

        void LogPlayerLocation();

        virtual void UpdateAIInternal(uint32 elapsed, bool minimal = false);
	public:
        uint32 activeBots = 0;

        static bool HandlePlayerbotConsoleCommand(ChatHandler* handler, char const* args);
        bool IsRandomBot(Player* bot);
        bool IsRandomBot(uint32 bot);
        void Randomize(Player* bot);
        void RandomizeFirst(Player* bot);
        void UpdateGearSpells(Player* bot);
        void ScheduleTeleport(uint32 bot, uint32 time = 0);
        void ScheduleChangeStrategy(uint32 bot, uint32 time = 0);
        void HandleCommand(uint32 type, const string& text, Player& fromPlayer, string channelName = "", Team team = TEAM_BOTH_ALLOWED);
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
        void RandomTeleportForLevel(Player* bot, bool activeOnly);
        void RandomTeleportForLevel(Player* bot) { return RandomTeleportForLevel(bot, true); }
        void RandomTeleportForRpg(Player* bot, bool activeOnly);
        void RandomTeleportForRpg(Player* bot) { return RandomTeleportForRpg(bot, true); }
        int GetMaxAllowedBotCount();
        bool ProcessBot(Player* player);
        void Revive(Player* player);
        void ChangeStrategy(Player* player);
        void ChangeStrategyOnce(Player* player);
        uint32 GetValue(Player* bot, string type);
        uint32 GetValue(uint32 bot, string type);
        string GetData(uint32 bot, string type);
        void SetValue(uint32 bot, string type, uint32 value, string data = "");
        void SetValue(Player* bot, string type, uint32 value, string data = "");
        void Remove(Player* bot);
        void Hotfix(Player* player, uint32 version);
        uint32 GetBattleMasterEntry(Player* bot, BattleGroundTypeId bgTypeId, bool fake = false);
        const CreatureDataPair* GetCreatureDataByEntry(uint32 entry);
        uint32 GetCreatureGuidByEntry(uint32 entry);
        void LoadBattleMastersCache();
        map<uint32, map<uint32, map<uint32, bool> > > NeedBots;
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
        static Item* CreateTempItem(uint32 item, uint32 count, Player const* player, uint32 randomPropertyId = 0);

        bool AddRandomBot(uint32 bot);

        map<Team, map<BattleGroundTypeId, list<uint32> > > getBattleMastersCache() { return BattleMastersCache; }

        float getActivityMod() { return activityMod; }
        float getActivityPercentage() { return activityMod * 100.0f; }
        void setActivityPercentage(float percentage) { activityMod = percentage / 100.0f; }

        bool arenaTeamsDeleted, guildsDeleted = false;

	protected:
	    virtual void OnBotLoginInternal(Player * const bot);

    private:
        botPID pid = botPID(1, 100, -100, 0.0001, 0.001, 0.0001);
        float activityMod = 1;
        uint32 GetEventValue(uint32 bot, string event);
        string GetEventData(uint32 bot, string event);
        uint32 SetEventValue(uint32 bot, string event, uint32 value, uint32 validIn, string data = "");
        list<uint32> GetBots();
        list<uint32> GetBgBots(uint32 bracket);
        time_t BgCheckTimer;
        time_t LfgCheckTimer;
        time_t PlayersCheckTimer;
        uint32 AddRandomBots();
        bool ProcessBot(uint32 bot);
        void ScheduleRandomize(uint32 bot, uint32 time);
        void RandomTeleport(Player* bot);
        void RandomTeleport(Player* bot, vector<WorldLocation> &locs, bool hearth = false, bool activeOnly = false);
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
        list<uint32> arenaTeamMembers;
        uint32 bgBotsCount;
        uint32 playersLevel = sPlayerbotAIConfig.randombotStartingLevel;
        PerformanceMonitorOperation* totalPmo;
};

#define sRandomPlayerbotMgr RandomPlayerbotMgr::instance()

#endif
