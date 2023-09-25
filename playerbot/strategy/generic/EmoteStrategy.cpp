#include "botpch.h"
#include "../../playerbot.h"
#include "EmoteStrategy.h"

using namespace ai;


void EmoteStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "random",
        NextAction::array(0, new NextAction("emote", 0.5f), NULL)));

    triggers.push_back(new TriggerNode(
        "seldom",
        NextAction::array(0, new NextAction("suggest what to do", 7.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "seldom",
        NextAction::array(0, new NextAction("suggest trade", 7.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "seldom",
        NextAction::array(0, new NextAction("check inventory", 7.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "new player nearby",
        NextAction::array(0, new NextAction("greet", 2.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("talk", 2.0f), NULL)));
}
