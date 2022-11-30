#include "botpch.h"
#include "../../playerbot.h"
#include "MoltenCoreDungeonStrategies.h"

using namespace ai;

void MoltenCoreDungeonStrategy::InitTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "start magmadar fight",
        NextAction::array(0, new NextAction("enable magmadar fight strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "end magmadar fight",
        NextAction::array(0, new NextAction("disable magmadar fight strategy", 100.0f), NULL)));
}

void MagmadarFightStrategy::InitTriggers(std::list<TriggerNode*>& triggers)
{
    // We need this strategy to remove itself in case it stays active outside of the dungeon
    triggers.push_back(new TriggerNode(
        "end magmadar fight",
        NextAction::array(0, new NextAction("disable magmadar fight strategy", 100.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "magmadar lava bomb",
        NextAction::array(0, new NextAction("move away from magmadar lava bomb", 100.0f), NULL)));
}