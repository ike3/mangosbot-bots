#pragma once

#include "Config.h"
#include "Talentspec.h"

class Player;
class PlayerbotMgr;
class ChatHandler;

class PlayerbotAIConfig
{
public:
    PlayerbotAIConfig();
    static PlayerbotAIConfig& instance()
    {
        static PlayerbotAIConfig instance;
        return instance;
    }

public:
    bool Initialize();
    bool IsInRandomAccountList(uint32 id);
    bool IsInRandomQuestItemList(uint32 id);
	bool IsInPvpProhibitedZone(uint32 id);

    bool enabled;
    bool allowGuildBots;
    uint32 globalCoolDown, reactDelay, maxWaitForMove, expireActionTime, dispelAuraDuration, passiveDelay, repeatDelay,
        errorDelay, rpgDelay, sitDelay, returnDelay, lootDelay;
    float sightDistance, spellDistance, reactDistance, grindDistance, lootDistance, shootDistance,
        fleeDistance, tooCloseDistance, meleeDistance, followDistance, whisperDistance, contactDistance,
        aoeRadius, rpgDistance, targetPosRecalcDistance, farDistance, healDistance, aggroDistance;
    uint32 criticalHealth, lowHealth, mediumHealth, almostFullHealth;
    uint32 lowMana, mediumMana;

    uint32 openGoSpell;
    bool randomBotAutologin;
    bool botAutologin;
    std::string randomBotMapsAsString;
    std::vector<uint32> randomBotMaps;
    std::list<uint32> randomBotQuestItems;
    std::list<uint32> randomBotAccounts;
    std::list<uint32> randomBotSpellIds;
    std::list<uint32> randomBotQuestIds;
    uint32 randomBotTeleportDistance;
    float randomGearLoweringChance;
    float randomBotMaxLevelChance;
    float randomBotRpgChance;
    uint32 minRandomBots, maxRandomBots;
    uint32 randomBotUpdateInterval, randomBotCountChangeMinInterval, randomBotCountChangeMaxInterval;
    uint32 minRandomBotInWorldTime, maxRandomBotInWorldTime;
    uint32 minRandomBotRandomizeTime, maxRandomBotRandomizeTime;
    uint32 minRandomBotChangeStrategyTime, maxRandomBotChangeStrategyTime;
    uint32 minRandomBotReviveTime, maxRandomBotReviveTime;
    uint32 minRandomBotPvpTime, maxRandomBotPvpTime;
    uint32 randomBotsPerInterval;
    uint32 minRandomBotsPriceChangeInterval, maxRandomBotsPriceChangeInterval;
    bool randomBotJoinLfg;
    bool randomBotJoinBG;
    uint32 randomBotBracketCount;
    bool randomBotLoginAtStartup;
    uint32 randomBotTeleLevel;
    bool logInGroupOnly, logValuesPerTick;
    bool fleeingEnabled;
    bool summonAtInnkeepersEnabled;
    std::string combatStrategies, nonCombatStrategies;
    std::string randomBotCombatStrategies, randomBotNonCombatStrategies;
    uint32 randomBotMinLevel, randomBotMaxLevel;
    float randomChangeMultiplier;
    uint32 specProbability[MAX_CLASSES][10];
    string premadeLevelSpec[MAX_CLASSES][10][91]; //lvl 10 - 100
    ClassSpecs classSpecs[MAX_CLASSES];
    std::string commandPrefix, commandSeparator;
    std::string randomBotAccountPrefix;
    uint32 randomBotAccountCount;
    bool deleteRandomBotAccounts;
    uint32 randomBotGuildCount;
    bool deleteRandomBotGuilds;
    uint32 randomBotArenaTeamCount;
    bool deleteRandomBotArenaTeams;
    std::list<uint32> randomBotArenaTeams;
	bool RandombotsWalkingRPG;
	bool RandombotsWalkingRPGInDoors;
    std::list<uint32> randomBotGuilds;
	std::list<uint32> pvpProhibitedZoneIds;
    bool enableGreet;
    bool randomBotShowHelmet;
    bool randomBotShowCloak;
    bool disableRandomLevels;
    bool gearscorecheck;
	bool randomBotPreQuests;
    uint32 playerbotsXPrate;
    uint32 botActiveAlone;
    uint32 minEnchantingBotLevel;
    uint32 randombotStartingLevel;
    bool randomBotSayWithoutMaster;
    bool randomBotGroupNearby;

    bool guildTaskEnabled;
    uint32 minGuildTaskChangeTime, maxGuildTaskChangeTime;
    uint32 minGuildTaskAdvertisementTime, maxGuildTaskAdvertisementTime;
    uint32 minGuildTaskRewardTime, maxGuildTaskRewardTime;
    uint32 guildTaskAdvertCleanupTime;

    uint32 iterationsPerTick;

    std::string autoPickReward;
    bool autoEquipUpgradeLoot;
    bool syncQuestWithPlayer;
    bool syncQuestForPlayer;
    std::string autoTrainSpells;
    std::string autoPickTalents;
    bool autoLearnTrainerSpells;
    bool autoLearnQuestSpells;
    bool autoDoQuests;
    bool syncLevelWithPlayers;
    uint32 tweakValue; //Debugging config

    std::mutex m_logMtx;
    string logFileName[10];
    bool logFileOpen[10] = { false };
    FILE* logFile[10];

    struct worldBuff{
        uint32 spellId;
        uint32 factionId = 0;
        uint32 classId = 0;
        uint32 minLevel = 0;
        uint32 maxLevel = 0;
    };

    vector<worldBuff> worldBuffs;

    int commandServerPort;
    bool perfMonEnabled;

    std::string GetValue(std::string name);
    void SetValue(std::string name, std::string value);

    void loadWorldBuf(Config* config, uint32 factionId, uint32 classId, uint32 minLevel, uint32 maxLevel);

    std::string GetTimestampStr();
    bool hasLog(uint32);
    void openLog(uint32, char const* mode);
    void log(uint32 logId, const char* str, ...);
private:
    Config config;
};

#define sPlayerbotAIConfig MaNGOS::Singleton<PlayerbotAIConfig>::Instance()

