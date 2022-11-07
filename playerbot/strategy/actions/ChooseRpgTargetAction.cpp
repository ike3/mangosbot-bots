#include "botpch.h"
#include "../../playerbot.h"
#include "ChooseRpgTargetAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../values/PossibleRpgTargetsValue.h"
#include "../../TravelMgr.h"
#include "../values/BudgetValues.h"
#include "GuildCreateActions.h"
#include "../values/Formations.h"
#include "RpgSubActions.h"

using namespace ai;

bool ChooseRpgTargetAction::HasSameTarget(ObjectGuid guid, uint32 max, list<ObjectGuid>& nearGuids)
{
    if (ai->HasRealPlayerMaster())
        return 0;

    uint32 num = 0;

    for (auto& i : nearGuids)
    {
        Player* player = sObjectMgr.GetPlayer(i);

        if (!player)
            continue;

        PlayerbotAI* ai = player->GetPlayerbotAI();

        if (!ai)
            continue;

        if (!ai->AllowActivity(GRIND_ACTIVITY))
            continue;

        if (PAI_VALUE(GuidPosition,"rpg target") != guid)
            continue;

        num++;

        if (num >= max)
            break;
    }

    return num;
}

float ChooseRpgTargetAction::getMaxRelevance(GuidPosition guidP)
{
    GuidPosition currentRpgTarget = AI_VALUE(GuidPosition, "rpg target");
    SET_AI_VALUE(GuidPosition, "rpg target", guidP);

    Strategy* rpgStrategy;

    list<TriggerNode*> triggerNodes;

    float maxRelevance = 0.0f;

    for (auto& strategy : ai->GetAiObjectContext()->GetSupportedStrategies())
    {
        if (strategy.find("rpg") == std::string::npos)
            continue;

        if (!ai->HasStrategy(strategy, BotState::BOT_STATE_NON_COMBAT))
            continue;

        rpgStrategy = ai->GetAiObjectContext()->GetStrategy(strategy);

        rpgStrategy->InitTriggers(triggerNodes);

        for (auto& triggerNode : triggerNodes)
        {
            Trigger* trigger = context->GetTrigger(triggerNode->getName());

            if (trigger)
            {
                triggerNode->setTrigger(trigger);

                if (triggerNode->getFirstRelevance() < maxRelevance || triggerNode->getFirstRelevance() > 2.0f)
                    continue;

                Trigger* trigger = triggerNode->getTrigger();

                if (!trigger->IsActive())
                    continue;

                NextAction** nextActions = triggerNode->getHandlers();

                bool isRpg = false;

                for (int32 i = 0; i < NextAction::size(nextActions); i++)
                {
                    NextAction* nextAction = nextActions[i];

                    Action* action = ai->GetAiObjectContext()->GetAction(nextAction->getName());

                    if (dynamic_cast<RpgEnabled*>(action))
                        isRpg = true;
                }
                NextAction::destroy(nextActions);

                if (isRpg)
                {
                    maxRelevance = triggerNode->getFirstRelevance();
                    rgpActionReason[guidP] = triggerNode->getName();
                }
            }
        }

        for (list<TriggerNode*>::iterator i = triggerNodes.begin(); i != triggerNodes.end(); i++)
        {
            TriggerNode* trigger = *i;
            delete trigger;
        }

        triggerNodes.clear();
    }

    SET_AI_VALUE(GuidPosition,"rpg target", currentRpgTarget);

    if (!maxRelevance)
        return 0.0;

    return (maxRelevance - 1.0) * 1000.0f;
}

bool ChooseRpgTargetAction::Execute(Event& event)
{
    TravelTarget* travelTarget = AI_VALUE(TravelTarget*, "travel target");

    unordered_map<ObjectGuid, uint32> targets;
    vector<ObjectGuid> targetList;

    list<ObjectGuid> possibleTargets = AI_VALUE(list<ObjectGuid>, "possible rpg targets");
    list<ObjectGuid> possibleObjects = bot->GetMap()->IsDungeon() ? AI_VALUE(list<ObjectGuid>, "nearest game objects") : AI_VALUE(list<ObjectGuid>, "nearest game objects no los"); // skip not in LOS objects in dungeons
    list<ObjectGuid> possiblePlayers = AI_VALUE(list<ObjectGuid>, "nearest friendly players");
    set<ObjectGuid>& ignoreList = AI_VALUE(set<ObjectGuid>&, "ignore rpg target");

    for (auto target : possibleTargets)
        targets[target] = 0.0f;

    for (auto target : possibleObjects)
        targets[target] = 0.0f;

    for (auto target : possiblePlayers)
        targets[target] = 0.0f;

    if (targets.empty())
    {
        return false;
    }

    if (urand(0, 9))
    {
        for (auto target : ignoreList)
            targets.erase(target);
    }

    SET_AI_VALUE(string, "next rpg action", this->getName());

    bool hasGoodRelevance = false;

    rgpActionReason.clear();

    for (auto& target : targets)
        targetList.push_back(target.first);

    std::shuffle(targetList.begin(), targetList.end(), *GetRandomGenerator());

    //Update tradeskill items so we can use lazy in trigger check.
    if(ai->HasStrategy("rpg craft", BotState::BOT_STATE_NON_COMBAT))
        AI_VALUE2(list<uint32>, "inventory item ids", "usage " + to_string(ITEM_USAGE_SKILL));

    uint16 checked = 0;

    for (auto& guid :targetList)
    {
        GuidPosition guidP(guid);

        if (!guidP)
            continue;

        float priority = 1;

        if (guidP.GetWorldObject() && !isFollowValid(bot, guidP.GetWorldObject()))
            continue;

        if (guidP.IsGameObject())
        {
            GameObject* go = guidP.GetGameObject();
            if (!go || !sServerFacade.isSpawned(go)
                || go->IsInUse()
                || go->GetGoState() != GO_STATE_READY)
                continue;

            // skip objects too high
            if (fabs(go->GetPositionZ() - bot->GetPositionZ()) > 20.f)
                continue;
        }
        else if (guidP.IsPlayer())
        {
            Player* player = guidP.GetPlayer();

            if (!player)
                continue;

            if (player->GetPlayerbotAI())
            {
                GuidPosition guidPP = PAI_VALUE(GuidPosition, "rpg target");

                if (guidPP.IsPlayer())
                {
                    continue;
                }
            }
        }

        if (possiblePlayers.size() < 200 && HasSameTarget(guidP, urand(5, 15), possiblePlayers))
            continue;

        float relevance = getMaxRelevance(guidP);

        if (!hasGoodRelevance || relevance > 1)
            targets[guidP] = relevance;

        if (targets[guidP] > 1)
            hasGoodRelevance = true;

        checked++;

        if (checked >= 50) //Some limit on stuff to check.
            break;
    }

    SET_AI_VALUE(string, "next rpg action", "");

    for (auto it = begin(targets); it != end(targets);)
    {
        if (it->second == 0 || (hasGoodRelevance && it->second <= 1.0))
        {
            it = targets.erase(it);
        }
        else
            ++it;
    }

    if (targets.empty())
    {
        if (ai->HasStrategy("debug rpg", BotState::BOT_STATE_NON_COMBAT))
        {
            ostringstream out;
            out << "found: no targets, " << checked << " checked.";
            ai->TellMasterNoFacing(out);
        }
        sLog.outDetail("%s can't choose RPG target: all %zu are not available", bot->GetName(), possibleTargets.size());
        RESET_AI_VALUE(set<ObjectGuid>&,"ignore rpg target");
        RESET_AI_VALUE(GuidPosition, "rpg target");
        return false;
    }

    if (ai->HasStrategy("debug rpg", BotState::BOT_STATE_NON_COMBAT))
    {
        vector<pair<ObjectGuid, uint32>> sortedTargets(targets.begin(), targets.end());

        std::sort(sortedTargets.begin(), sortedTargets.end(), [](pair<ObjectGuid, uint32>i, pair<ObjectGuid, uint32> j) {return i.second > j.second; });

        ai->TellMasterNoFacing("------" + to_string(targets.size()) + "------");

        uint32 checked = 0;

        for (auto target : sortedTargets)
        {
            GuidPosition guidP(target.first);

            if (!guidP.GetWorldObject())
                continue;

            ostringstream out;
            out << chat->formatWorldobject(guidP.GetWorldObject());

            out << " " << rgpActionReason[guidP] << " " << target.second;

            ai->TellMasterNoFacing(out);

            checked++;

            if (checked >= 10)
            {
                ostringstream out;
                out << "and " << (sortedTargets.size()-checked) << " more...";
                ai->TellMasterNoFacing(out);
                break;
            }
        }
    }

    vector<GuidPosition> guidps;
    vector<int> relevances;

    for (auto& target : targets)
    {
        guidps.push_back(target.first);
        relevances.push_back(target.second);
    }

    std::mt19937 gen(time(0));

    sTravelMgr.weighted_shuffle(guidps.begin(), guidps.end(), relevances.begin(), relevances.end(), gen);

    GuidPosition guidP(guidps.front());

    if (!guidP)
    {
        RESET_AI_VALUE(GuidPosition, "rpg target");
        return false;
    }

    if ((ai->HasStrategy("debug", BotState::BOT_STATE_NON_COMBAT) || ai->HasStrategy("debug rpg", BotState::BOT_STATE_NON_COMBAT)) && guidP.GetWorldObject())
    {
        ostringstream out;
        out << "found: ";
        out << chat->formatWorldobject(guidP.GetWorldObject());

        out << " " << rgpActionReason[guidP] << " " << targets[guidP];

        ai->TellMasterNoFacing(out);
    }

    SET_AI_VALUE(GuidPosition, "rpg target", guidP);

    return true;
}

bool ChooseRpgTargetAction::isUseful()
{
    if (!ai->AllowActivity(RPG_ACTIVITY))
        return false;

    if (AI_VALUE(GuidPosition, "rpg target"))
        return false;

    TravelTarget* travelTarget = AI_VALUE(TravelTarget*, "travel target");

    if (travelTarget->isTraveling() && isFollowValid(bot, *travelTarget->getPosition()))
        return false;

    if (AI_VALUE(list<ObjectGuid>, "possible rpg targets").empty())
        return false;

    if (!AI_VALUE(bool, "can move around"))
        return false;

    return true;
}

bool ChooseRpgTargetAction::isFollowValid(Player* bot, WorldObject* target)
{
    if (!target)
        return false;

    WorldLocation location;
    target->GetPosition(location);
    return isFollowValid(bot, location);
}

bool ChooseRpgTargetAction::isFollowValid(Player* bot, WorldPosition pos)
{
    PlayerbotAI* ai = bot->GetPlayerbotAI();
    Player* master = ai->GetGroupMaster();
    Player* realMaster = ai->GetMaster();
    AiObjectContext* context = ai->GetAiObjectContext();

    bool inDungeon = false;

    if (ai->HasActivePlayerMaster())
    {
        if (realMaster->IsInWorld() &&
            realMaster->GetMap()->IsDungeon() &&
            bot->GetMapId() == realMaster->GetMapId())
            inDungeon = true;

        if (realMaster &&
            realMaster->IsInWorld() &&
            realMaster->GetMap()->IsDungeon() &&
            (realMaster->GetMapId() != pos.getMapId()))
            return false;
    }

    if (!master || bot == master)
        return true;

    if (!ai->HasStrategy("follow", BotState::BOT_STATE_NON_COMBAT))
        return true;

    Formation* formation = AI_VALUE(Formation*, "formation");
    float distance = sqrt(master->GetDistance2d(pos.getX(), pos.getY(), DIST_CALC_NONE));

    if (!ai->HasActivePlayerMaster())
    {
        Player* player = master;
        if(PAI_VALUE(WorldPosition,"last long move").distance(pos) < sPlayerbotAIConfig.reactDistance)
            return true;

        if (!master->IsMoving() && distance < 50.0f)
            return true;
    }

    if ((inDungeon || master->IsInCombat()) && (realMaster == master) && distance > 5.0f)
        return false;

    float maxDist = formation->GetMaxDistance();

    uint32 lastMasterMove = MEM_AI_VALUE(WorldPosition, "master position")->LastChangeDelay();

    if (lastMasterMove > 30.0f) //After 30 seconds increase the range by 1y each second.
        maxDist += (lastMasterMove - 30);

    if (maxDist > sPlayerbotAIConfig.reactDistance)
        maxDist = sPlayerbotAIConfig.reactDistance;

    if (distance < maxDist)
        return true;

    return false;
}