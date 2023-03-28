#include "botpch.h"
#include "../../playerbot.h"
#include "ShareQuestAction.h"

using namespace ai;

bool ShareQuestAction::Execute(Event& event)
{
    string link = event.getParam();
    if (!GetMaster())
        return false;

    PlayerbotChatHandler handler(GetMaster());
    uint32 entry = handler.extractQuestId(link);
    if (!entry)
        return false;

    Quest const* quest = sObjectMgr.GetQuestTemplate(entry);
    if (!quest)
        return false;

    // remove all quest entries for 'entry' from quest log
    for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        uint32 logQuest = bot->GetQuestSlotQuestId(slot);
        if (logQuest == entry)
        {
            WorldPacket p;
            p << entry;
            bot->GetSession()->HandlePushQuestToParty(p);
            ai->TellMaster("Quest shared", PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
            return true;
        }
    }

    return false;
}

bool AutoShareQuestAction::Execute(Event& event)
{
    bool shared = false;

    for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        uint32 logQuest = bot->GetQuestSlotQuestId(slot);
        Quest const* quest = sObjectMgr.GetQuestTemplate(logQuest);

        if (!quest)
            continue;

        bool partyNeedsQuest = false;

        for (GroupReference* itr = bot->GetGroup()->GetFirstMember(); itr != nullptr; itr = itr->next())
        {
            Player* pPlayer = itr->getSource();

            if (!pPlayer || pPlayer == bot || !pPlayer->IsInWorld() || !ai->IsSafe(pPlayer))         // skip self
                continue;

            if (bot->GetDistance(pPlayer) > 10)
                continue;

            if (!pPlayer->SatisfyQuestStatus(quest, false))
                continue;

            if (pPlayer->GetQuestStatus(logQuest) == QUEST_STATUS_COMPLETE)
                continue;

            if (!pPlayer->CanTakeQuest(quest, false))
                continue;


            if (!pPlayer->SatisfyQuestLog(false))
                continue;

            if (pPlayer->GetDividerGuid())
                continue;

            partyNeedsQuest = true;

            break;
        }

        if (!partyNeedsQuest)
            continue;

        WorldPacket p;
        p << logQuest;
        bot->GetSession()->HandlePushQuestToParty(p);
        ai->TellMaster("Quest shared", PlayerbotSecurityLevel::PLAYERBOT_SECURITY_ALLOW_ALL, false);
        shared = true;
    }

    return shared;
}
