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
        "not near master",
        NextAction::array(0, new NextAction("follow", 1.0f), NULL)));
}

void FollowMasterStrategy::InitDeadTriggers(std::list<TriggerNode*>& triggers)
{
    InitNonCombatTriggers(triggers);
}
