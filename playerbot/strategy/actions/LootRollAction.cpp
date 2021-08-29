#include "botpch.h"
#include "../../playerbot.h"
#include "LootRollAction.h"
#include "../values/ItemUsageValue.h"


using namespace ai;

bool LootRollAction::Execute(Event event)
{
    Player *bot = QueryItemUsageAction::ai->GetBot();

    WorldPacket p(event.getPacket()); //WorldPacket packet for CMSG_LOOT_ROLL, (8+4+1)
    ObjectGuid guid;
    uint32 slot;
    uint8 rollType;
    p.rpos(0); //reset packet pointer
    p >> guid; //guid of the item rolled
    p >> slot; //number of players invited to roll
    p >> rollType; //need,greed or pass on roll

    Group* group = bot->GetGroup();
    if(!group)
        return false;

#ifdef MANGOS
    RollVote vote = ROLL_PASS;
	ItemPrototype const *proto = sItemStorage.LookupEntry<ItemPrototype>(guid.GetEntry());
	if (proto)
	{
		switch (proto->Class)
		{
		case ITEM_CLASS_WEAPON:
		case ITEM_CLASS_ARMOR:
			if (QueryItemUsage(proto))
				vote = ROLL_NEED;
			break;
		default:
			if (StoreLootAction::IsLootAllowed(guid.GetEntry(), ai))
				vote = ROLL_NEED;
			break;
		}
	}

    switch (group->GetLootMethod())
    {
    case MASTER_LOOT:
    case FREE_FOR_ALL:
        group->CountRollVote(bot, guid, slot, ROLL_PASS);
        break;
    default:
        group->CountRollVote(bot, guid, slot, vote);
        break;
    }
#endif

#ifdef CMANGOS
    Loot* loot = sLootMgr.GetLoot(bot, guid);
    if (!loot)
        return false;

    LootItem* item = loot->GetLootItemInSlot(slot);
    ItemPrototype const *proto = sItemStorage.LookupEntry<ItemPrototype>(item->itemId);
    if (!proto)
        return false;

    RollVote vote = CalculateRollVote(proto);

    GroupLootRoll* lootRoll = loot->GetRollForSlot(slot);
    if (!lootRoll)
        return false;

    lootRoll->PlayerVote(bot, vote);
#endif

    return true;
}

RollVote LootRollAction::CalculateRollVote(ItemPrototype const *proto)
{
    ostringstream out; out << proto->ItemId;
    ItemUsage usage = AI_VALUE2(ItemUsage, "item usage", out.str());

    RollVote needVote = ROLL_PASS;
    switch (usage)
    {
    case ITEM_USAGE_EQUIP:
    case ITEM_USAGE_REPLACE:
    case ITEM_USAGE_GUILD_TASK:
    case ITEM_USAGE_BAD_EQUIP:
        needVote = ROLL_NEED;
        break;
    case ITEM_USAGE_SKILL:
    case ITEM_USAGE_USE:
    case ITEM_USAGE_DISENCHANT:
    case ITEM_USAGE_AH:
    case ITEM_USAGE_VENDOR:
        needVote = ROLL_GREED;
        break;
    }
    return StoreLootAction::IsLootAllowed(proto->ItemId, bot->GetPlayerbotAI()) ? needVote : ROLL_PASS;
}

bool MasterLootRollAction::Execute(Event event)
{
    Player* bot = QueryItemUsageAction::ai->GetBot();

    WorldPacket p(event.getPacket()); //WorldPacket packet for CMSG_LOOT_ROLL, (8+4+1)
    ObjectGuid creatureGuid;
    uint32 itemSlot;
    uint32 itemId;
    uint32 randomSuffix;
    uint32 randomPropertyId;
#ifdef MANGOSBOT_TWO
    uint32 mapId;
    uint32 count;
#endif 
    uint32 timeout;
    
    p.rpos(0); //reset packet pointer
    p >> creatureGuid; //creature guid what we're looting
#ifdef MANGOSBOT_TWO
    p >> mapId; /// 3.3.3 mapid
#endif 
    p >> itemSlot; // the itemEntryId for the item that shall be rolled for
    p >> itemId; // the itemEntryId for the item that shall be rolled for
    p >> randomSuffix; // randomSuffix
    p >> randomPropertyId; // item random property ID
#ifdef MANGOSBOT_TWO
    p >> count; // items in stack
#endif 
    p >> timeout;  // the countdown time to choose "need" or "greed"

    Group* group = bot->GetGroup();
    if (!group)
        return false;

    Loot* loot = sLootMgr.GetLoot(bot, creatureGuid);
    if (!loot)
        return false;

    ItemPrototype const* proto = sItemStorage.LookupEntry<ItemPrototype>(itemId);
    if (!proto)
        return false;
    
    RollVote vote = CalculateRollVote(proto);

    GroupLootRoll* lootRoll = loot->GetRollForSlot(itemSlot);
    if (!lootRoll)
        return false;

    lootRoll->PlayerVote(bot, vote);

    return true;
}