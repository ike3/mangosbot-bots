#include "botpch.h"
#include "../../playerbot.h"
#include "DefenseStrategy.h"

using namespace ai;

void DefenseStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "not defense target active",
        NextAction::array(0, new NextAction("defense", 50.0f), NULL)));
}

