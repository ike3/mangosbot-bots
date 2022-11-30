#include "botpch.h"
#include "../../playerbot.h"
#include "OnyxiasLairDungeonStrategies.h"

using namespace ai;

void OnyxiasLairDungeonStrategy::InitTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "start onyxia fight",
        NextAction::array(0, new NextAction("enable onyxia fight strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "end onyxia fight",
        NextAction::array(0, new NextAction("disable onyxia fight strategy", 100.0f), NULL)));
}

void OnyxiaFightStrategy::InitTriggers(std::list<TriggerNode*>& triggers)
{
    // We need this strategy to remove itself in case it stays active outside of the dungeon
    triggers.push_back(new TriggerNode(
        "end onyxia fight",
        NextAction::array(0, new NextAction("disable onyxia fight strategy", 100.0f), NULL)));
}