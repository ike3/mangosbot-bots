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
        return false;

    if (!guid)
        guid = master->GetSelectionGuid();

    if (guid)
        return ProcessQuests(guid);

    list<ObjectGuid> npcs = AI_VALUE(list<ObjectGuid>, "nearest npcs");
    for (list<ObjectGuid>::iterator i = npcs.begin(); i != npcs.end(); i++)
    {
        Unit* unit = ai->GetUnit(*i);
        if (unit && bot->GetDistance(unit) <= INTERACTION_DISTANCE)
            return ProcessQuests(unit);
    }
    list<ObjectGuid> gos = AI_VALUE(list<ObjectGuid>, "nearest game objects");
    for (list<ObjectGuid>::iterator i = gos.begin(); i != gos.end(); i++)
    {
        GameObject* go = ai->GetGameObject(*i);
        if (go && bot->GetDistance(go) <= INTERACTION_DISTANCE)
            return ProcessQuests(go);
    }

    return false;
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

    if (bot->GetDistance(questGiver) > INTERACTION_DISTANCE)
    {
        ai->TellError("Cannot talk to quest giver");
        return false;
    }

    if (!sServerFacade.IsInFront(bot, questGiver, sPlayerbotAIConfig.sightDistance, CAST_ANGLE_IN_FRONT))
        sServerFacade.SetFacingTo(bot, questGiver);

    bot->SetSelectionGuid(guid);
    bot->PrepareQuestMenu(guid);
    QuestMenu& questMenu = bot->PlayerTalkClass->GetQuestMenu();
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

        if (bot->GetQuestStatus(questId) != QUEST_STATUS_NONE && bot->GetQuestStatus(questId) != QUEST_STATUS_AVAILABLE)
        {
            out << "Accepted " << chat->formatQuest(quest);
            ai->TellMaster(out);
            return true;
        }
    }

    out << " " << chat->formatQuest(quest);
    ai->TellError(out.str());
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
