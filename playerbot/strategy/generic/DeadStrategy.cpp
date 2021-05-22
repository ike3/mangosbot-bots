#include "botpch.h"
#include "../../playerbot.h"
#include "../Strategy.h"
#include "DeadStrategy.h"

using namespace ai;

void DeadStrategy::InitTriggers(std::list<TriggerNode*>& triggers)
{
    PassTroughStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("auto release", relevance), NULL)));

    triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("leave far away", relevance), NULL))); //If partyleader just walks away without reviving. Leave group.

    triggers.push_back(new TriggerNode(
        "often",
       NextAction::array(0, new NextAction("find corpse", relevance), NULL)));

    triggers.push_back(new TriggerNode(
        "corpse near",
        NextAction::array(0, new NextAction("revive from corpse", relevance), NULL)));

    triggers.push_back(new TriggerNode(
        "resurrect request",
        NextAction::array(0, new NextAction("accept resurrect", relevance), NULL)));

    triggers.push_back(new TriggerNode(
        "falling far",
        NextAction::array(0, new NextAction("repop", relevance), NULL)));
        
    triggers.push_back(new TriggerNode(
        "bg active",
        NextAction::array(0, new NextAction("auto release", relevance), NULL)));
}

DeadStrategy::DeadStrategy(PlayerbotAI* ai) : PassTroughStrategy(ai)
{
}
