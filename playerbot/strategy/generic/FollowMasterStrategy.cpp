#include "botpch.h"
#include "../../playerbot.h"
#include "FollowMasterStrategy.h"

using namespace ai;

void FollowMasterStrategy::InitNonCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "out of react range",
        NextAction::array(0, new NextAction("check mount state", ACTION_HIGH), new NextAction("flee to master", ACTION_HIGH), NULL)));

    triggers.push_back(new TriggerNode(
        "update follow",
        NextAction::array(0, new NextAction("follow", 1.0f), NULL)));
}

void FollowMasterStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    FollowMasterStrategy::InitNonCombatTriggers(triggers);
}

void FollowMasterStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    InitNonCombatTriggers(triggers);
}

void FollowMasterStrategy::InitReactionTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "stop follow",
        NextAction::array(0, new NextAction("stop follow", 100.0f), NULL)));
}

void FollowMasterStrategy::OnStrategyRemoved(BotState state)
{
    if (state == ai->GetState() && ai->GetBot()->GetMotionMaster()->GetCurrentMovementGeneratorType() == FOLLOW_MOTION_TYPE)
    {
        ai->StopMoving();
    }
}