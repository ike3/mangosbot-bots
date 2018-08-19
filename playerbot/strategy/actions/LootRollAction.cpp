#include "botpch.h"
#include "../../playerbot.h"
#include "LootRollAction.h"


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
    for (vector<Roll*>::iterator i = group->GetRolls().begin(); i != group->GetRolls().end(); ++i)
    {
        if ((*i)->isValid() && (*i)->lootedTargetGUID == guid && (*i)->itemSlot == slot)
        {
            uint32 itemId = (*i)->itemid;
            ItemPrototype const *proto = sItemStorage.LookupEntry<ItemPrototype>(itemId);
            if (!proto)
                continue;

            if (IsLootAllowed(itemId, bot->GetPlayerbotAI()))
            {
                vote = ROLL_NEED;
                break;
            }
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

    RollVote vote = IsLootAllowed(item->itemId, bot->GetPlayerbotAI()) ? ROLL_NEED : ROLL_PASS;

    GroupLootRoll* lootRoll = loot->GetRollForSlot(slot);
    if (!lootRoll)
        return false;

    lootRoll->PlayerVote(bot, vote);
#endif

    return true;
}
