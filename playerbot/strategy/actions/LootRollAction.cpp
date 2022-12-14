#include "botpch.h"
#include "../../playerbot.h"
#include "LootRollAction.h"
#include "../values/ItemUsageValue.h"
#include "../values/LootValues.h"

using namespace ai;

bool LootStartRollAction::ExecuteCommand(Event& event)
{
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

    LootRollMap lootRolls = AI_VALUE(LootRollMap, "active rolls");

    lootRolls.insert({ creatureGuid, itemSlot });    
    
    ActiveRolls::CleanUp(bot,lootRolls);

    SET_AI_VALUE(LootRollMap, "active rolls", lootRolls);

    return false;
}

bool RollAction::ExecuteCommand(Event& event)
{      
    string text = event.getParam();

    if (text.empty())
    {
        ai->TellMasterNoFacing("Please give a roll type or item. See " + ChatHelper::formatValue("help", "action:roll", "roll help") + " for more information.");
        return false;
    }

    ItemIds ids = ChatHelper::parseItems(text);

    string type = "auto";
    if (ids.empty())
        type = text;
    else
        type = text.substr(0, text.find(" "));

    if (type != "need" && type != "greed" && type != "pass" && type != "auto")
    {
        ai->TellMasterNoFacing("Please give a correct roll type. need, greed, pass or auto. See " + ChatHelper::formatValue("help", "action:roll", "roll help") + " for more information.");
        return false;
    }

    RollVote vote = ROLL_NOT_VALID;

    if (type.find("need") == 0)
        vote = ROLL_NEED;
    else if (type.find("greed") == 0)
        vote = ROLL_GREED;
    else if (type.find("pass") == 0)
        vote = ROLL_PASS;

    uint32 rolledItems = 0;

    LootRollMap lootRolls = AI_VALUE(LootRollMap, "active rolls");

    for (auto roll : lootRolls)
    {
        ItemPrototype const* proto = GetRollItem(roll.first, roll.second);

        if (!proto)
            continue;

        if (!ids.empty() && ids.find(proto->ItemId) == ids.end())
            continue;

        RollVote doVote = vote;
        if (doVote == ROLL_NOT_VALID) //Auto
            doVote = CalculateRollVote(proto);

        rolledItems += RollOnItemInSlot(doVote, roll.first, roll.second);     
    }

    return rolledItems;
}

bool RollAction::isPossible()
{
    return bot->GetGroup() && !AI_VALUE(LootRollMap, "active rolls").empty(); 
}

ItemPrototype const* RollAction::GetRollItem(ObjectGuid lootGuid, uint32 slot)
{
    Loot* loot = sLootMgr.GetLoot(bot, lootGuid);
    if (!loot)
        return nullptr;

    LootItem* item = loot->GetLootItemInSlot(slot);

    if (!item)
        return nullptr;

    return sItemStorage.LookupEntry<ItemPrototype>(item->itemId);
}

RollVote RollAction::CalculateRollVote(ItemPrototype const* proto)
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
    case ITEM_USAGE_FORCE:
        needVote = (AI_VALUE2(ForceItemUsage, "force item usage", proto->ItemId) == ForceItemUsage::FORCE_USAGE_NEED) ? ROLL_NEED : ROLL_GREED;
        break;
    }
    return StoreLootAction::IsLootAllowed(proto->ItemId, bot->GetPlayerbotAI()) ? needVote : ROLL_PASS;
}

bool RollAction::RollOnItemInSlot(RollVote vote, ObjectGuid lootGuid, uint32 slot)
{
    Loot* loot = sLootMgr.GetLoot(bot, lootGuid);
    if (!loot)
        return false;

    LootItem* item = loot->GetLootItemInSlot(slot);
    ItemPrototype const* proto = sItemStorage.LookupEntry<ItemPrototype>(item->itemId);
    if (!proto)
        return false;

    GroupLootRoll* lootRoll = loot->GetRollForSlot(slot);
    if (!lootRoll)
        return false;

    bool didRoll = lootRoll->PlayerVote(bot, vote);

    if (didRoll)
    {
        LootRollMap lootRolls = AI_VALUE(LootRollMap, "active rolls");

        ActiveRolls::CleanUp(bot, lootRolls, lootGuid, slot);

        SET_AI_VALUE(LootRollMap, "active rolls", lootRolls);
    }

    return didRoll;
}

bool LootRollAction::ExecuteCommand(Event& event)
{
    Player* bot = QueryItemUsageAction::ai->GetBot();

    WorldPacket p(event.getPacket()); //WorldPacket packet for CMSG_LOOT_ROLL, (8+4+1)
    ObjectGuid guid;
    uint32 slot;
    uint8 rollType;
    p.rpos(0); //reset packet pointer
    p >> guid; //guid of the item rolled
    p >> slot; //number of players invited to roll
    p >> rollType; //need,greed or pass on roll

    ItemPrototype const* proto = GetRollItem(guid, slot);

    if (!proto)
        return false;

    RollVote vote = CalculateRollVote(proto);

    return RollOnItemInSlot(vote, guid, slot);
}

bool AutoLootRollAction::ExecuteCommand(Event& event)
{
    LootRollMap lootRolls = AI_VALUE(LootRollMap, "active rolls");

    auto currentRoll = lootRolls.begin();

    currentRoll = std::next(currentRoll, urand(0, lootRolls.size() - 1));

    ItemPrototype const* proto = GetRollItem(currentRoll->first, currentRoll->second);

    if (!proto)
        return false;

    RollVote vote = CalculateRollVote(proto);

    return RollOnItemInSlot(vote, currentRoll->first, currentRoll->second);
}