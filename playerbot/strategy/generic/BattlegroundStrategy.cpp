#include "botpch.h"
#include "../../playerbot.h"
#include "BattlegroundStrategy.h"

using namespace ai;

void BGStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "random",
        NextAction::array(0, new NextAction("bg join", relevance), NULL)));

    triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("bg status check", relevance), NULL)));
}

BGStrategy::BGStrategy(PlayerbotAI* ai) : PassTroughStrategy(ai)
{
}

void WarsongStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "bg active",
        NextAction::array(0, new NextAction("bg check flag", 5.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("bg use buff", 5.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "low health",
        NextAction::array(0, new NextAction("bg use buff", 10.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "low mana",
        NextAction::array(0, new NextAction("bg use buff", 10.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy flagcarrier near",
        NextAction::array(0, new NextAction("attack enemy flag carrier", 80.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "player has flag",
        NextAction::array(0, new NextAction("bg move to objective", 90.0f), NULL)));
}

void AlteracStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    /* placeholder */
}

void ArathiStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "bg active",
        NextAction::array(0, new NextAction("bg check flag", 5.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("bg use buff", 5.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "low health",
        NextAction::array(0, new NextAction("bg use buff", 10.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "low mana",
        NextAction::array(0, new NextAction("bg use buff", 10.0f), NULL)));
}

void BattlegroundStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "bg waiting",
        NextAction::array(0, new NextAction("bg move to start", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "bg active",
        NextAction::array(0, new NextAction("bg move to objective", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("bg check objective", 10.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy flagcarrier near",
        NextAction::array(0, new NextAction("attack enemy flag carrier", 80.0f), NULL)));

    /*triggers.push_back(new TriggerNode(
        "team flagcarrier near",
        NextAction::array(0, new NextAction("bg protect fc", 40.0f), NULL)));*/

    triggers.push_back(new TriggerNode(
        "player has flag",
        NextAction::array(0, new NextAction("bg move to objective", 90.0f), NULL)));
}

void ArenaStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "no possible targets",
        NextAction::array(0, new NextAction("arena tactics", 1.0f), NULL)));
}
