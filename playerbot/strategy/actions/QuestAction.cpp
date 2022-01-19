#include "botpch.h"
#include "../../playerbot.h"
#include "QuestAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"

using namespace ai;

bool QuestAction::Execute(Event event)
{
    ObjectGuid guid = event.getObject();

    Player* master = GetMaster();
    if (!master)
    {
        if (!guid)
            guid = bot->GetSelectionGuid();
    }
    else {
        if (!guid)
            guid = master->GetSelectionGuid();
    }

    if (!guid)
        return false;

    return ProcessQuests(guid);
}

bool QuestAction::CompleteQuest(Player* player, uint32 entry)
{
    Quest const* pQuest = sObjectMgr.GetQuestTemplate(entry);

    // If player doesn't have the quest
    if (!pQuest || player->GetQuestStatus(entry) == QUEST_STATUS_NONE)
    {
        return false;
    }

    // Add quest items for quests that require items
    for (uint8 x = 0; x < QUEST_ITEM_OBJECTIVES_COUNT; ++x)
    {
        uint32 id = pQuest->ReqItemId[x];
        uint32 count = pQuest->ReqItemCount[x];
        if (!id || !count)
        {
            continue;
        }

        uint32 curItemCount = player->GetItemCount(id, true);

        ItemPosCountVec dest;
        uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, id, count - curItemCount);
        if (msg == EQUIP_ERR_OK)
        {
            Item* item = player->StoreNewItem(dest, id, true);
            player->SendNewItem(item, count - curItemCount, true, false);
        }
    }

    // All creature/GO slain/casted (not required, but otherwise it will display "Creature slain 0/10")
    for (uint8 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
    {
        int32 creature = pQuest->ReqCreatureOrGOId[i];
        uint32 creaturecount = pQuest->ReqCreatureOrGOCount[i];

        if (uint32 spell_id = pQuest->ReqSpell[i])
        {
            for (uint16 z = 0; z < creaturecount; ++z)
            {
                player->CastedCreatureOrGO(creature, ObjectGuid(), spell_id);
            }
        }
        else if (creature > 0)
        {
            if (CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(creature))
                for (uint16 z = 0; z < creaturecount; ++z)
                {
                    player->KilledMonster(cInfo, nullptr);
                }
        }
        else if (creature < 0)
        {
            for (uint16 z = 0; z < creaturecount; ++z)
            {
                player->CastedCreatureOrGO(-creature, ObjectGuid(), 0);
            }
        }
    }

    // If the quest requires reputation to complete
    if (uint32 repFaction = pQuest->GetRepObjectiveFaction())
    {
        uint32 repValue = pQuest->GetRepObjectiveValue();
        uint32 curRep = player->GetReputationMgr().GetReputation(repFaction);
        if (curRep < repValue)
#ifdef MANGOS
            if (FactionEntry const* factionEntry = sFactionStore.LookupEntry(repFaction))
#endif
#ifdef CMANGOS
#ifdef MANGOSBOT_ONE
            if (FactionEntry const* factionEntry = sFactionStore.LookupEntry<FactionEntry>(repFaction))
#else
            if (FactionEntry const* factionEntry = sFactionStore.LookupEntry(repFaction))
#endif
#endif
            {
                player->GetReputationMgr().SetReputation(factionEntry, repValue);
            }
    }

    // If the quest requires money
    int32 ReqOrRewMoney = pQuest->GetRewOrReqMoney();
    if (ReqOrRewMoney < 0)
    {
        player->ModifyMoney(-ReqOrRewMoney);
    }

#ifdef MANGOS
    player->CompleteQuest(entry, QUEST_STATUS_FORCE_COMPLETE);
#endif
#ifdef CMANGOS
    player->CompleteQuest(entry);
#endif

    return true;
}

bool QuestAction::ProcessQuests(ObjectGuid questGiver)
{
    GameObject *gameObject = ai->GetGameObject(questGiver);
    if (gameObject && gameObject->GetGoType() == GAMEOBJECT_TYPE_QUESTGIVER)
        return ProcessQuests(gameObject);

    Creature* creature = ai->GetCreature(questGiver);
    if (creature)
        return ProcessQuests(creature);

    return false;
}

bool QuestAction::ProcessQuests(WorldObject* questGiver)
{
    ObjectGuid guid = questGiver->GetObjectGuid();

    if (bot->GetDistance(questGiver) > INTERACTION_DISTANCE && !sPlayerbotAIConfig.syncQuestWithPlayer)
    {
        ai->TellError("Cannot talk to quest giver");
        return false;
    }

    if (!sServerFacade.IsInFront(bot, questGiver, sPlayerbotAIConfig.sightDistance, CAST_ANGLE_IN_FRONT))
        sServerFacade.SetFacingTo(bot, questGiver);

    bot->SetSelectionGuid(guid);
    bot->PrepareQuestMenu(guid);
    QuestMenu& questMenu = bot->GetPlayerMenu()->GetQuestMenu();

    for (uint32 i = 0; i < questMenu.MenuItemCount(); ++i)
    {
        QuestMenuItem const& menuItem = questMenu.GetItem(i);
        uint32 questID = menuItem.m_qId;
        Quest const* quest = sObjectMgr.GetQuestTemplate(questID);
        if (!quest)
            continue;

        ProcessQuest(quest, questGiver);
    }

    return true;
}

bool QuestAction::AcceptQuest(Quest const* quest, uint64 questGiver)
{
    std::ostringstream out;

    uint32 questId = quest->GetQuestId();

    if (bot->GetQuestStatus(questId) == QUEST_STATUS_COMPLETE)
        out << "Already completed";
    else if (! bot->CanTakeQuest(quest, false))
    {
        if (! bot->SatisfyQuestStatus(quest, false))
            out << "Already on";
        else
            out << "Can't take";
    }
    else if (! bot->SatisfyQuestLog(false))
        out << "Quest log is full";
    else if (! bot->CanAddQuest(quest, false))
        out << "Bags are full";

    else
    {
        WorldPacket p(CMSG_QUESTGIVER_ACCEPT_QUEST);
        uint32 unk1 = 0;
        p << questGiver << questId << unk1;
        p.rpos(0);
        bot->GetSession()->HandleQuestgiverAcceptQuestOpcode(p);

        if (bot->GetQuestStatus(questId) == QUEST_STATUS_NONE && sPlayerbotAIConfig.syncQuestWithPlayer)
        {
            Object* pObject = bot->GetObjectByTypeMask((ObjectGuid)questGiver, TYPEMASK_CREATURE_GAMEOBJECT_PLAYER_OR_ITEM);
            bot->AddQuest(quest, pObject);

        }

        if (bot->GetQuestStatus(questId) != QUEST_STATUS_NONE && bot->GetQuestStatus(questId) != QUEST_STATUS_AVAILABLE)
        {
            out << "Accepted " << chat->formatQuest(quest);
            ai->TellMaster(out);
            return true;
        }
    }

    out << " " << chat->formatQuest(quest);
    ai->TellMaster(out);
    return false;
}

bool QuestObjectiveCompletedAction::Execute(Event event)
{
    WorldPacket p(event.getPacket());
    p.rpos(0);

    uint32 entry, questId, available, required;
    ObjectGuid guid;
    p >> questId >> entry >> available >> required >> guid;

    if (entry & 0x80000000)
    {
        entry &= 0x7FFFFFFF;
        GameObjectInfo const* info = sObjectMgr.GetGameObjectInfo(entry);
        if (info)
            ai->TellMaster(chat->formatQuestObjective(info->name, available, required));
    }
    else
    {
        CreatureInfo const* info = sObjectMgr.GetCreatureTemplate(entry);
        if (info)
            ai->TellMaster(chat->formatQuestObjective(info->Name, available, required));
    }

    return true;
}
