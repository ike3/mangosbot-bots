#include "botpch.h"
#include "../../playerbot.h"
#include "RewardAction.h"
#include "../ItemVisitors.h"
#include "../values/ItemCountValue.h"
#include "../values/ItemUsageValue.h"

using namespace ai;

bool RewardAction::Execute(Event event)
{
    string link = event.getParam();

    ItemIds itemIds = chat->parseItems(link);
    if (itemIds.empty())
        return false;

    uint32 itemId = *itemIds.begin();

    list<ObjectGuid> npcs = AI_VALUE(list<ObjectGuid>, "nearest npcs");
    for (list<ObjectGuid>::iterator i = npcs.begin(); i != npcs.end(); i++)
    {
        Unit* npc = ai->GetUnit(*i);
        if (npc && Reward(itemId, npc))
            return true;
    }

    list<ObjectGuid> gos = AI_VALUE(list<ObjectGuid>, "nearest game objects");
    for (list<ObjectGuid>::iterator i = gos.begin(); i != gos.end(); i++)
    {
        GameObject* go = ai->GetGameObject(*i);
        if (go && Reward(itemId, go))
            return true;
    }

    Unit* mtar = AI_VALUE(Unit*, "master target");
    if (mtar && Reward(itemId, mtar))
       return true;    


    ai->TellError("Cannot talk to quest giver");
    return false;
}

void RewardAction::EquipItem(int32 itemId)
{
    Item* newItem = bot->GetItemByEntry(itemId);
    if (!newItem)
        return;

    ItemUsage usage = AI_VALUE2(ItemUsage, "item usage", itemId);

    if (usage != ITEM_USAGE_EQUIP && usage != ITEM_USAGE_REPLACE && usage != ITEM_USAGE_BAD_EQUIP)
        return;

    uint8 bagIndex = newItem->GetBagSlot();
    uint8 slot = newItem->GetSlot();

    WorldPacket packet(CMSG_AUTOEQUIP_ITEM, 2);
    packet << bagIndex << slot;
    bot->GetSession()->HandleAutoEquipItemOpcode(packet);

    ostringstream out;
    out << "equipping " << chat->formatItem(newItem->GetProto());
    ai->TellMasterNoFacing(out.str());
}

bool RewardAction::Reward(uint32 itemId, Object* questGiver)
{
    QuestMenu& questMenu = bot->PlayerTalkClass->GetQuestMenu();
    for (uint32 iI = 0; iI < questMenu.MenuItemCount(); ++iI)
    {
        QuestMenuItem const& qItem = questMenu.GetItem(iI);

        uint32 questID = qItem.m_qId;
        Quest const* pQuest = sObjectMgr.GetQuestTemplate(questID);
        QuestStatus status = bot->GetQuestStatus(questID);

        // if quest is complete, turn it in
        if (status == QUEST_STATUS_COMPLETE &&
            ! bot->GetQuestRewardStatus(questID) &&
            pQuest->GetRewChoiceItemsCount() > 1 &&
            bot->CanRewardQuest(pQuest, false))
        {
            for (uint8 rewardIdx=0; rewardIdx < pQuest->GetRewChoiceItemsCount(); ++rewardIdx)
            {
                ItemPrototype const * const pRewardItem = sObjectMgr.GetItemPrototype(pQuest->RewChoiceItemId[rewardIdx]);
                if (itemId == pRewardItem->ItemId)
                {
                    bot->RewardQuest(pQuest, rewardIdx, questGiver, false);

                    string questTitle  = pQuest->GetTitle();
                    string itemName = pRewardItem->Name1;

                    ostringstream out; out << chat->formatItem(pRewardItem) << " rewarded";
                    ai->TellMaster(out);

                    if (sPlayerbotAIConfig.AutoEquipUpgradeLoot)
                    {
                        EquipItem(pRewardItem->ItemId);
                    }

                    return true;
                }
            }
        }
    }

    return false;
}
