#include "botpch.h"
#include "../../playerbot.h"
#include "LfgStrategy.h"

using namespace ai;

void LfgStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "random",
        NextAction::array(0, new NextAction("lfg join", relevance), NULL)));

    triggers.push_back(new TriggerNode(
        "seldom",
        NextAction::array(0, new NextAction("lfg leave", relevance), NULL)));
}

LfgStrategy::LfgStrategy(PlayerbotAI* ai) : PassTroughStrategy(ai)
{
}
