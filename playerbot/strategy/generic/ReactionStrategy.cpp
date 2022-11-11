#include "botpch.h"
#include "../../playerbot.h"
#include "../../ServerFacade.h"
#include "ReactionStrategy.h"

using namespace ai;

void ReactionStrategy::InitTriggers(list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "combat start",
        NextAction::array(0, new NextAction("set combat state", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "combat end",
        NextAction::array(0, new NextAction("set non combat state", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "death",
        NextAction::array(0, new NextAction("set dead state", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "resurrect",
        NextAction::array(0, new NextAction("set non combat state", 60.0f), NULL)));
}