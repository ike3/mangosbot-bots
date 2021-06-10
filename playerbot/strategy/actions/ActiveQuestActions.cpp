#include "botpch.h"
#include "../../playerbot.h"
#include "ActiveQuestActions.h"


using namespace ai;

bool PickUpQuestAction::getPotentialTarget() { 

    for (auto& guidp : potentialTargets)
    {
        //if (guidp.GetCreatureTemplate())
        //    ai->TellMasterNoFacing(to_string(guidp.getPosition().distance(WorldPosition(bot))) + guidp.GetCreatureTemplate()->Name);
    }

    return !potentialTargets.empty() && (potentialTarget = WorldPosition(bot).closest<GuidPosition>(potentialTargets).first); 
}

bool PickUpQuestAction::isUseful()
{
    if (!ChooseMoveDoAction::isUseful())
        return false;

    return AI_VALUE(uint8, "free quest log slots") > 1;
};

bool PickUpQuestAction::ExecuteAction(Event event)
{
    if (!getObjectTarget())
        return false;

    ObjectGuid guid = getObjectTarget()->Get();

    //Parse rpg target to quest action.
    WorldPacket p(CMSG_QUESTGIVER_ACCEPT_QUEST);
    p << guid;
    p.rpos(0);

    return ai->DoSpecificAction("accept all quests", Event("pick up quest", p));
}

bool DoQuestObjectiveAction::ExecuteAction(Event event)
{
    if (!getObjectTarget())
        return false;

    ObjectGuid guid = getObjectTarget()->Get();

    //Parse rpg target to quest action.
    WorldPacket p(CMSG_QUESTGIVER_ACCEPT_QUEST);
    p << guid;
    p.rpos(0);

    if (guid.IsGameObject())
    {
        WorldObject* wo = ai->GetWorldObject(guid);

        return ai->DoSpecificAction("use", Event("rpg action", chat->formatWorldobject(wo)));
    }

    return ai->DoSpecificAction("attack anything", Event("do quest objective", p));    
}

bool HandInQuestAction::ExecuteAction(Event event)
{
    if (!getObjectTarget())
        return false;

    ObjectGuid guid = getObjectTarget()->Get();

    //Parse rpg target to quest action.
    WorldPacket p(CMSG_QUESTGIVER_ACCEPT_QUEST);
    p << guid;
    p.rpos(0);

    return ai->DoSpecificAction("talk to quest giver", Event("hand in quest", p));
}