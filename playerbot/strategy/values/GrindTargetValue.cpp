#include "botpch.h"
#include "../../playerbot.h"
#include "GrindTargetValue.h"
#include "../../PlayerbotAIConfig.h"
#include "../../RandomPlayerbotMgr.h"
#include "../../ServerFacade.h"

using namespace ai;

Unit* GrindTargetValue::Calculate()
{
    uint32 memberCount = 1;
    Group* group = bot->GetGroup();
    if (group)
        memberCount = group->GetMembersCount();

    Unit* target = NULL;
    uint32 assistCount = 0;
    while (!target && assistCount < memberCount)
    {
        target = FindTargetForGrinding(assistCount++);
    }

    return target;
}


Unit* GrindTargetValue::FindTargetForGrinding(int assistCount)
{
    uint32 memberCount = 1;
    Group* group = bot->GetGroup();
    Player* master = GetMaster();

    list<ObjectGuid> attackers = context->GetValue<list<ObjectGuid> >("attackers")->Get();
    for (list<ObjectGuid>::iterator i = attackers.begin(); i != attackers.end(); i++)
    {
        Unit* unit = ai->GetUnit(*i);
        if (!unit || !sServerFacade.IsAlive(unit))
            continue;

        return unit;
    }

    list<ObjectGuid> targets = *context->GetValue<list<ObjectGuid> >("possible targets");

    if (targets.empty())
        return NULL;

    float distance = 0;
    Unit* result = NULL;

    unordered_map<uint32, bool> needForQuestMap;

    for (list<ObjectGuid>::iterator tIter = targets.begin(); tIter != targets.end(); tIter++)
    {
        Unit* unit = ai->GetUnit(*tIter);
        if (!unit)
            continue;

        if (abs(bot->GetPositionZ() - unit->GetPositionZ()) > sPlayerbotAIConfig.spellDistance)
            continue;

        if (!bot->InBattleGround() && GetTargetingPlayerCount(unit) > assistCount)
            continue;

        //if (master && master->GetDistance(unit) >= sPlayerbotAIConfig.grindDistance && !sRandomPlayerbotMgr.IsRandomBot(bot))
        //    continue;

        if (!bot->InBattleGround() && (int)unit->GetLevel() - (int)bot->GetLevel() > 4 && !unit->GetObjectGuid().IsPlayer())
            continue;

        if (needForQuestMap.find(unit->GetEntry()) == needForQuestMap.end())
            needForQuestMap[unit->GetEntry()] = needForQuest(unit);

        if (!needForQuestMap[unit->GetEntry()])
            if (urand(0, 100) < 75 || (context->GetValue<TravelTarget*>("travel target")->Get()->isWorking() && context->GetValue<TravelTarget*>("travel target")->Get()->getDestination()->getName() != "GrindTravelDestination"))
                continue;

        //if (bot->InBattleGround() && bot->GetDistance(unit) > 40.0f)
            //continue;

        Creature* creature = dynamic_cast<Creature*>(unit);
        if (creature && creature->GetCreatureInfo() && creature->GetCreatureInfo()->Rank > CREATURE_ELITE_NORMAL && !AI_VALUE(bool, "can fight elite"))
            continue;

        if (group)
        {
            Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
            for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
            {
                Player* member = sObjectMgr.GetPlayer(itr->guid);
                if (!member || !sServerFacade.IsAlive(member))
                    continue;

                float d = sServerFacade.GetDistance2d(member, unit);
                if (!result || d < distance)
                {
                    distance = d;
                    result = unit;
                }
            }
        }
        else
        {
            float newdistance = sServerFacade.GetDistance2d(bot, unit);
            if (!result || (newdistance < distance && urand(0, abs(distance - newdistance)) > sPlayerbotAIConfig.sightDistance * 0.1))
            {
                distance = newdistance;
                result = unit;
            }
        }
    }

    return result;
}

bool GrindTargetValue::needForQuest(Unit* target)
{
    bool justCheck = (bot->GetObjectGuid() == target->GetObjectGuid());

    QuestStatusMap& questMap = bot->getQuestStatusMap();
    for (auto& quest : questMap)
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
            QuestStatusData* questStatus = sTravelMgr.getQuestStatus(bot, questId);

            if (questTemplate->GetQuestLevel() > (int)bot->GetLevel())
                continue;

            for (int j = 0; j < QUEST_OBJECTIVES_COUNT; j++)
            {
                int32 entry = questTemplate->ReqCreatureOrGOId[j];

                if (entry && entry > 0)
                {
                    int required = questTemplate->ReqCreatureOrGOCount[j];
                    int available = questStatus->m_creatureOrGOcount[j];

                    if (required && available < required && (target->GetEntry() == entry || justCheck))
                        return true;
                }

                if (justCheck)
                {
                    int32 itemId = questTemplate->ReqItemId[j];

                    if (itemId && itemId > 0)
                    {
                        int required = questTemplate->ReqItemCount[j];
                        int available = questStatus->m_itemcount[j];

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

int GrindTargetValue::GetTargetingPlayerCount( Unit* unit )
{
    Group* group = bot->GetGroup();
    if (!group)
        return 0;

    int count = 0;
    Group::MemberSlotList const& groupSlot = group->GetMemberSlots();
    for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
    {
        Player *member = sObjectMgr.GetPlayer(itr->guid);
        if( !member || !sServerFacade.IsAlive(member) || member == bot)
            continue;

        PlayerbotAI* ai = member->GetPlayerbotAI();
        if ((ai && *ai->GetAiObjectContext()->GetValue<Unit*>("current target") == unit) ||
            (!ai && member->GetSelectionGuid() == unit->GetObjectGuid()))
            count++;
    }

    return count;
}

