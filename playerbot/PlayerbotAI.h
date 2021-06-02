#pragma once

#include "../botpch.h"
#include "PlayerbotMgr.h"
#include "PlayerbotAIBase.h"
#include "strategy/AiObjectContext.h"
#include "strategy/Engine.h"
#include "strategy/ExternalEventHelper.h"
#include "ChatFilter.h"
#include "PlayerbotSecurity.h"
#include "TravelMgr.h"
#include <stack>

class Player;
class PlayerbotMgr;
class ChatHandler;

using namespace std;
using namespace ai;

bool IsAlliance(uint8 race);

class PlayerbotChatHandler: protected ChatHandler
{
public:
    explicit PlayerbotChatHandler(Player* pMasterPlayer) : ChatHandler(pMasterPlayer->GetSession()) {}
    void sysmessage(string str) { SendSysMessage(str.c_str()); }
    uint32 extractQuestId(string str);
    uint32 extractSpellId(string str)
    {
        char* source = (char*)str.c_str();
        return ExtractSpellIdFromLink(&source);
    }
};

namespace ai
{
	class MinValueCalculator {
	public:
		MinValueCalculator(float def = 0.0f) {
			param = NULL;
			minValue = def;
		}

	public:
		void probe(float value, void* p) {
			if (!param || minValue >= value) {
				minValue = value;
				param = p;
			}
		}

	public:
		void* param;
		float minValue;
	};
};

enum HealingItemDisplayId
{
   HEALTHSTONE_DISPLAYID = 8026,
   MAJOR_HEALING_POTION = 24152,
   WHIPPER_ROOT_TUBER = 21974,
   NIGHT_DRAGON_BREATH = 21975,
   LIMITED_INVULNERABILITY_POTION = 24213,
   GREATER_DREAMLESS_SLEEP_POTION = 17403,
   SUPERIOR_HEALING_POTION = 15714,
   CRYSTAL_RESTORE = 2516,
   DREAMLESS_SLEEP_POTION = 17403,
   GREATER_HEALING_POTION = 15713,
   HEALING_POTION = 15712,
   LESSER_HEALING_POTION = 15711,
   DISCOLORED_HEALING_POTION = 15736,
   MINOR_HEALING_POTION = 15710,
   VOLATILE_HEALING_POTION = 24212,
   SUPER_HEALING_POTION = 37807,
   CRYSTAL_HEALING_POTION = 47132,
   FEL_REGENERATION_POTION = 37864,
   MAJOR_DREAMLESS_SLEEP_POTION = 37845,
};

enum BotState
{
    BOT_STATE_COMBAT = 0,
    BOT_STATE_NON_COMBAT = 1,
    BOT_STATE_DEAD = 2
};

#define BOT_STATE_MAX 3

enum RoguePoisonDisplayId
{
   DEADLY_POISON_DISPLAYID = 13707,
   INSTANT_POISON_DISPLAYID = 13710,
#ifdef MANGOSBOT_ZERO
   WOUND_POISON_DISPLAYID = 13708
#else
   WOUND_POISON_DISPLAYID = 37278
#endif
};

enum SharpeningStoneDisplayId
{
   ROUGH_SHARPENING_DISPLAYID = 24673,
   COARSE_SHARPENING_DISPLAYID = 24674,
   HEAVY_SHARPENING_DISPLAYID = 24675,
   SOLID_SHARPENING_DISPLAYID = 24676,
   DENSE_SHARPENING_DISPLAYID = 24677,
   CONSECRATED_SHARPENING_DISPLAYID = 24674,    // will not be used because bot can not know if it will face undead targets
   ELEMENTAL_SHARPENING_DISPLAYID = 21072,
   FEL_SHARPENING_DISPLAYID = 39192,
   ADAMANTITE_SHARPENING_DISPLAYID = 39193,
};

enum WeightStoneDisplayId
{
   ROUGH_WEIGHTSTONE_DISPLAYID = 24683,
   COARSE_WEIGHTSTONE_DISPLAYID = 24684,
   HEAVY_WEIGHTSTONE_DISPLAYID = 24685,
   SOLID_WEIGHTSTONE_DISPLAYID = 24686,
   DENSE_WEIGHTSTONE_DISPLAYID = 24687,
   FEL_WEIGHTSTONE_DISPLAYID = 39548,
   ADAMANTITE_WEIGHTSTONE_DISPLAYID = 39549,
};

#ifdef MANGOSBOT_ZERO
// m_zero
enum WizardOilDisplayId
{
    MINOR_WIZARD_OIL = 33194,
    LESSER_WIZARD_OIL = 33450,
    BRILLIANT_WIZARD_OIL = 33452,
    WIZARD_OIL = 33451,
    SUPERIOR_WIZARD_OIL = 47904,
    /// Blessed Wizard Oil = 26865,//scourge inv
};
// m_zero
enum ManaOilDisplayId
{
    MINOR_MANA_OIL = 33453,
    LESSER_MANA_OIL = 33454,
    BRILLIANT_MANA_OIL = 33455,
    SUPERIOR_MANA_OIL = 36862,
};
#else
enum WizardOilDisplayId
{
   MINOR_WIZARD_OIL     = 9731,
   LESSER_WIZARD_OIL    = 47903,
   BRILLIANT_WIZARD_OIL = 47901,
   WIZARD_OIL           = 47905,
   SUPERIOR_WIZARD_OIL  = 47904,
  /// Blessed Wizard Oil = 26865,//scourge inv
};

enum ManaOilDisplayId
{
   MINOR_MANA_OIL       = 34492,
   LESSER_MANA_OIL      = 47902,
   BRILLIANT_MANA_OIL   = 41488,
   SUPERIOR_MANA_OIL    = 36862,
};
#endif

enum ShieldWardDisplayId
{
   LESSER_WARD_OFSHIELDING = 38759,
   GREATER_WARD_OFSHIELDING = 38760,
};

enum BotTypeNumber
{
    GROUPER_TYPE_NUMBER = 1,
    ACTIVITY_TYPE_NUMBER = 2
};

enum GrouperType
{
    SOLO = 0,
    MEMBER = 1,
    LEADER_2 = 2,
    LEADER_3 = 3,
    LEADER_4 = 4,
    LEADER_5 = 5
};

enum ActivityType
{
    GRIND_ACTIVITY = 1,
    RPG_ACTIVITY = 2,
    TRAVEL_ACTIVITY = 3,
    OUT_OF_PARTY_ACTIVITY = 4,
    PACKET_ACTIVITY = 5,
    DETAILED_MOVE_ACTIVITY = 6,
    ALL_ACTIVITY = 7
};

enum BotRoles
{
    BOT_ROLE_NONE = 0x00,
    BOT_ROLE_TANK = 0x01,
    BOT_ROLE_HEALER = 0x02,
    BOT_ROLE_DPS = 0x04
};

class PacketHandlingHelper
{
public:
    void AddHandler(uint16 opcode, string handler);
    void Handle(ExternalEventHelper &helper);
    void AddPacket(const WorldPacket& packet);

private:
    map<uint16, string> handlers;
    stack<WorldPacket> queue;
};

class ChatCommandHolder
{
public:
    ChatCommandHolder(string command, Player* owner = NULL, uint32 type = CHAT_MSG_WHISPER, time_t time = 0) : command(command), owner(owner), type(type), time(time) {}
    ChatCommandHolder(ChatCommandHolder const& other)
    {
        this->command = other.command;
        this->owner = other.owner;
        this->type = other.type;
        this->time = other.time;
    }

public:
    string GetCommand() { return command; }
    Player* GetOwner() { return owner; }
    uint32 GetType() { return type; }
    time_t GetTime() { return time; }

private:
    string command;
    Player* owner;
    uint32 type;
    time_t time;
};

class PlayerbotAI : public PlayerbotAIBase
{
public:
	PlayerbotAI();
	PlayerbotAI(Player* bot);
	virtual ~PlayerbotAI();

public:
	virtual void UpdateAIInternal(uint32 elapsed);
	string HandleRemoteCommand(string command);
    void HandleCommand(uint32 type, const string& text, Player& fromPlayer);
	void HandleBotOutgoingPacket(const WorldPacket& packet);
    void HandleMasterIncomingPacket(const WorldPacket& packet);
    void HandleMasterOutgoingPacket(const WorldPacket& packet);
	void HandleTeleportAck();
    void ChangeEngine(BotState type);
    void DoNextAction();
    virtual bool DoSpecificAction(string name, Event event = Event(), bool silent = false);
    void ChangeStrategy(string name, BotState type);
    void ClearStrategies(BotState type);
    list<string> GetStrategies(BotState type);
    bool ContainsStrategy(StrategyType type);
    bool HasStrategy(string name, BotState type);
    void ResetStrategies(bool load = true);
    void ReInitCurrentEngine();
    void Reset();
    bool IsTank(Player* player);
    bool IsHeal(Player* player);
    bool IsRanged(Player* player);
    Creature* GetCreature(ObjectGuid guid);
    Unit* GetUnit(ObjectGuid guid);
    Unit* GetUnit(CreatureDataPair const* creatureDataPair);
    GameObject* GetGameObject(ObjectGuid guid);
    WorldObject* GetWorldObject(ObjectGuid guid);
    bool TellMaster(ostringstream &stream, PlayerbotSecurityLevel securityLevel = PLAYERBOT_SECURITY_ALLOW_ALL) { return TellMaster(stream.str(), securityLevel); }
    bool TellMaster(string text, PlayerbotSecurityLevel securityLevel = PLAYERBOT_SECURITY_ALLOW_ALL);
    bool TellMasterNoFacing(ostringstream& stream, PlayerbotSecurityLevel securityLevel = PLAYERBOT_SECURITY_ALLOW_ALL) { return TellMasterNoFacing(stream.str(), securityLevel); }
    bool TellMasterNoFacing(string text, PlayerbotSecurityLevel securityLevel = PLAYERBOT_SECURITY_ALLOW_ALL);
    bool TellError(string text, PlayerbotSecurityLevel securityLevel = PLAYERBOT_SECURITY_ALLOW_ALL);
    void SpellInterrupted(uint32 spellid);
    int32 CalculateGlobalCooldown(uint32 spellid);
    void InterruptSpell();
    void RemoveAura(string name);
    void RemoveShapeshift();
    void WaitForSpellCast(Spell *spell);
    bool PlaySound(uint32 emote);
    void Ping(float x, float y);
    Item * FindPoison() const;
    Item * FindConsumable(uint32 displayId) const;
    Item * FindBandage() const;
    Item* FindStoneFor(Item* weapon) const;
    Item* FindOilFor(Item* weapon) const;
#ifdef MANGOSBOT_ZERO
    void ImbueItem(Item* item, uint16 targetFlag, ObjectGuid targetGUID);
#else
    void ImbueItem(Item* item, uint32 targetFlag, ObjectGuid targetGUID);
#endif
    void ImbueItem(Item* item, uint8 targetInventorySlot);
    void ImbueItem(Item* item, Unit* target);
    void ImbueItem(Item* item);
    void EnchantItemT(uint32 spellid, uint8 slot);
    uint32 GetBuffedCount(Player* player, string spellname);
  

    virtual bool CanCastSpell(string name, Unit* target, uint8 effectMask, Item* itemTarget = NULL);
    virtual bool CastSpell(string name, Unit* target, Item* itemTarget = NULL);
    virtual bool HasAura(string spellName, Unit* player, bool maxStack = false);
    virtual bool HasAnyAuraOf(Unit* player, ...);
    uint8 GetHealthPercent(const Unit& target) const;
    uint8 GetHealthPercent() const;
    uint8 GetManaPercent(const Unit& target) const;
    uint8 GetManaPercent() const;

    virtual bool IsInterruptableSpellCasting(Unit* player, string spell, uint8 effectMask);
    virtual bool HasAuraToDispel(Unit* player, uint32 dispelType);
    bool CanCastSpell(uint32 spellid, Unit* target, uint8 effectMask, bool checkHasSpell = true, Item* itemTarget = NULL);
    bool CanCastSpell(uint32 spellid, float x, float y, float z, uint8 effectMask, bool checkHasSpell = true, Item* itemTarget = NULL);

    bool HasAura(uint32 spellId, const Unit* player);
    bool CastSpell(uint32 spellId, Unit* target, Item* itemTarget = NULL);
    bool CastSpell(uint32 spellId, float x, float y, float z, Item* itemTarget = NULL);
    bool canDispel(const SpellEntry* entry, uint32 dispelType);

    uint32 GetEquipGearScore(Player* player, bool withBags, bool withBank);
    bool HasSkill(SkillType skill);
    bool IsAllowedCommand(string text);
    float GetRange(string type);

    static ReputationRank GetFactionReaction(FactionTemplateEntry const* thisTemplate, FactionTemplateEntry const* otherTemplate);
    static bool friendToAlliance(FactionTemplateEntry const* templateEntry) { return GetFactionReaction(templateEntry, sFactionTemplateStore.LookupEntry(1)) >= REP_NEUTRAL; }
    static bool friendToHorde(FactionTemplateEntry const* templateEntry) { return GetFactionReaction(templateEntry, sFactionTemplateStore.LookupEntry(2)) >= REP_NEUTRAL; }
    static bool AddAura(Unit* unit, uint32 spellId);
    ReputationRank getReaction(FactionTemplateEntry const* factionTemplate) { return GetFactionReaction(bot->GetFactionTemplateEntry(), factionTemplate);}
    
private:
    void _fillGearScoreData(Player *player, Item* item, std::vector<uint32>* gearScore, uint32& twoHandScore);
    bool IsTellAllowed(PlayerbotSecurityLevel securityLevel = PLAYERBOT_SECURITY_ALLOW_ALL);

public:
	Player* GetBot() { return bot; }
    Player* GetMaster() { return master; }

    //Checks if the bot is really a player. Players always have themselves as master.
    bool isRealPlayer() { return master ? (master == bot) : false; } 
    //Bot has a master that is a player.
    bool hasRealPlayerMaster() { return master && (!master->GetPlayerbotAI() || master->GetPlayerbotAI()->isRealPlayer()); } 
    //Get the group leader or the master of the bot.
    Player* GetGroupMaster() { return bot->GetGroup() ? (sObjectMgr.GetPlayer(bot->GetGroup()->GetLeaderGuid()) ? sObjectMgr.GetPlayer(bot->GetGroup()->GetLeaderGuid()) : master) : master; }
    //Returns a semi-random (cycling) number that is fixed for each bot.
    uint32 GetFixedBotNumer(BotTypeNumber typeNumber, uint32 maxNum = 100, uint32 cyclePerMin = 1); 

    GrouperType GetGrouperType();
    bool HasPlayerNearby(WorldPosition* pos, float range = sPlayerbotAIConfig.reactDistance);
    bool HasPlayerNearby(float range = sPlayerbotAIConfig.reactDistance) { WorldPosition botPos(bot);  return HasPlayerNearby(&botPos, range); };
    bool HasManyPlayersNearby(uint32 trigerrValue = 20, float range = sPlayerbotAIConfig.sightDistance);
    bool AllowActive(ActivityType activityType);
    void SetMaster(Player* master) { this->master = master; }
    AiObjectContext* GetAiObjectContext() { return aiObjectContext; }
    ChatHelper* GetChatHelper() { return &chatHelper; }
    bool IsOpposing(Player* player);
    static bool IsOpposing(uint8 race1, uint8 race2);
    PlayerbotSecurity* GetSecurity() { return &security; }

protected:
	Player* bot;
	Player* master;
	uint32 accountId;
    AiObjectContext* aiObjectContext;
    Engine* currentEngine;
    Engine* engines[BOT_STATE_MAX];
    BotState currentState;
    ChatHelper chatHelper;
    queue<ChatCommandHolder> chatCommands;
    PacketHandlingHelper botOutgoingPacketHandlers;
    PacketHandlingHelper masterIncomingPacketHandlers;
    PacketHandlingHelper masterOutgoingPacketHandlers;
    CompositeChatFilter chatFilter;
    PlayerbotSecurity security;
    map<string, time_t> whispers;
    pair<ChatMsg, time_t> currentChat;
    static set<string> unsecuredCommands;

};

