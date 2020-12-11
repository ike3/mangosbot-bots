#include "botpch.h"
#include "../../playerbot.h"
#include "../../LootObjectStack.h"
#include "ChooseTravelTargetAction.h"
#include "../../PlayerbotAIConfig.h"

#include "../../ServerFacade.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"


using namespace ai;
using namespace MaNGOS;

bool ChooseTravelTargetAction::Execute(Event event)
{
    bool addLoot = ai->DoSpecificAction("add all loot", Event(), true);

    if (addLoot || !needForQuest(bot))
    {
        context->GetValue<ObjectGuid>("travel target")->Set(ObjectGuid());
        return false;
    }


    Unit* selTarget;
    float selDistance;
    Unit* newTarget;
    float newDistance;
    bool foundTarget = false;

    list<Unit*> targets;
    AnyUnitInObjectRangeCheck u_check(bot, 9000);
    UnitListSearcher<AnyUnitInObjectRangeCheck> searcher(targets, u_check);
    Cell::VisitAllObjects(bot, searcher, 9000);

    list<ObjectGuid> nearGuids = ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest friendly players")->Get();

    for (auto& i : targets)
    {
        newTarget = i;

        if (!newTarget)
            continue;

        if (newTarget->GetMapId() != bot->GetMapId())
            continue;

        if (!newTarget->IsAlive())
            continue;

        if (bot->GetObjectGuid() == newTarget->GetObjectGuid())
            continue;

        bool needForQ = needForQuest(newTarget);

        if (!needForQ)// && (foundTarget || !newTarget->IsFriendlyTo(bot)))
            continue;

        if (newTarget->IsInCombat())
            continue;

        newDistance = newTarget->GetDistance(bot);

        if (foundTarget && (newDistance > selDistance || urand(0,abs(selDistance- newDistance)) < 60) && newDistance > sPlayerbotAIConfig.lootDistance)
            continue;

        int num = 0;

        for (auto& i : nearGuids)
        {
            Player* player = sObjectMgr.GetPlayer(i);

            if (!player)
                continue;

            PlayerbotAI* ai = player->GetPlayerbotAI();

            if (!ai)
                continue;

            if (ai->GetAiObjectContext()->GetValue<ObjectGuid>("travel target")->Get() != newTarget->GetObjectGuid())
                continue;

            num++;
        }

        if (num > urand(1, 15))
            continue;

        selTarget = newTarget;
        selDistance = newDistance;
        //if(!needForQ)
            foundTarget = true;
    }

    /*if (!foundTarget && ai->GetMaster())
    {
        foundTarget = true;
        selTarget = ai->GetMaster();
    }
    */

    if (foundTarget && selTarget->GetDistance(bot) < sPlayerbotAIConfig.lootDistance) {
        context->GetValue<ObjectGuid>("travel target")->Set(ObjectGuid());
        return false;
    }

    if (!foundTarget)
    {
        context->GetValue<ObjectGuid>("travel target")->Set(ObjectGuid());
        return false;
    }

    //ostringstream os; os << "Choosing: " << selTarget->GetName() << " at distance " << selTarget->GetDistance(bot);

    //ai->TellMaster(os);

    context->GetValue<ObjectGuid>("travel target")->Set(selTarget->GetObjectGuid());

    return true;
}

bool ChooseTravelTargetAction::isUseful()
{
    return !context->GetValue<ObjectGuid>("travel target")->Get() && context->GetValue<LootObject>("loot target")->Get().IsEmpty();
}


bool ChooseTravelTargetAction::needForQuest(Unit* target)
{
    bool justCheck = (bot->GetObjectGuid() == target->GetObjectGuid());

    QuestStatusMap& questMap = bot->getQuestStatusMap();
    for (auto & quest : questMap)
    {
        const Quest* questTemplate = sObjectMgr.GetQuestTemplate(quest.first);
        if (!questTemplate)
            continue;

        uint32 questId = questTemplate->GetQuestId();

        if (!questId)
            continue;

        QuestStatus status = bot->GetQuestStatus(questId);

        if ((status == QUEST_STATUS_COMPLETE && !bot->GetQuestRewardStatus(questId)))
        {
            if (!justCheck && !target->HasInvolvedQuest(questId))
                continue;

            return true;
        }
        else if (status == QUEST_STATUS_INCOMPLETE)
        {
            QuestStatusData questStatus = quest.second;

            if (questTemplate->GetQuestLevel() > bot->getLevel())
                continue;

            for (int j = 0; j < QUEST_OBJECTIVES_COUNT; j++)
            {
                int32 entry = questTemplate->ReqCreatureOrGOId[j];

                if (entry && entry > 0)
                {
                    int required = questTemplate->ReqCreatureOrGOCount[j];
                    int available = questStatus.m_creatureOrGOcount[j];

                    if(required && available < required && (target->GetEntry() == entry || justCheck))
                        return true;
                }         

                if (justCheck)
                {
                    int32 itemId = questTemplate->ReqItemId[j];

                    if (itemId && itemId > 0)
                    {
                        int required = questTemplate->ReqItemCount[j];
                        int available = questStatus.m_itemcount[j];

                        if (required && available < required)
                            return true;
                    }
                }
            }

            if (!justCheck)
            {
                CreatureInfo const* data = sObjectMgr.GetCreatureTemplate(target->GetEntry());

                if (data)
                {
                    uint32 lootId = data->LootId;

                    if (lootId)
                    {
                        if (LootTemplates_Creature.HaveQuestLootForPlayer(lootId, bot))
                            return true;
                    }
                }
            }
        }

    }
    return false;
}

bool ChooseTravelTargetAction::needItemForQuest(uint32 itemId, const Quest* questTemplate, const QuestStatusData* questStatus)
{
    for (int i = 0; i < QUEST_OBJECTIVES_COUNT; i++)
    {
        if (questTemplate->ReqItemId[i] != itemId)
            continue;

        int required = questTemplate->ReqItemCount[i];
        int available = questStatus->m_itemcount[i];

        if (!required)
            continue;

        return available < required;
    }

    return false;
}
