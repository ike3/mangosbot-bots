#include "botpch.h"
#include "../../playerbot.h"
#include "NonCombatStrategy.h"

using namespace ai;

void NonCombatStrategy::InitTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "timer",
        NextAction::array(0, new NextAction("check mount state", 1.0f), new NextAction("check values", 1.0f), NULL)));

    /*triggers.push_back(new TriggerNode(
        "bg status",
        NextAction::array(0, new NextAction("bg status", ACTION_EMERGENCY), NULL)));*/

    triggers.push_back(new TriggerNode(
        "levelup",
        NextAction::array(0, new NextAction("auto talents", 4.0f), new NextAction("auto learn spell", 4.0f), NULL)));
}


void LfgStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "no possible targets",
        NextAction::array(0, new NextAction("lfg join", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "lfg proposal",
        NextAction::array(0, new NextAction("lfg accept", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "lfg proposal active",
        NextAction::array(0, new NextAction("lfg accept", 1.0f), NULL)));
}

void CollisionStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "collision",
        NextAction::array(0, new NextAction("move out of collision", 2.0f), NULL)));
}

void BGStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    /*triggers.push_back(new TriggerNode(
        "bg invite",
        NextAction::array(0, new NextAction("accept bg invitation", 1.0f), NULL)));*/

    /*triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("wsg join", 1.0f), NULL)));*/

    triggers.push_back(new TriggerNode(
        "no possible targets",
        NextAction::array(0, new NextAction("bg join", ACTION_HIGH), NULL)));

    /*triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("bg join", 10.0f), NULL)));*/

    /*triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("bg status", 1.0f), NULL)));*/

    /*triggers.push_back(new TriggerNode(
        "bg status",
        NextAction::array(0, new NextAction("bg status", ACTION_EMERGENCY), NULL)));*/

}

void WarsongStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "no possible targets",
        NextAction::array(0, new NextAction("bg tactics ws", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "dead",
        NextAction::array(0, new NextAction("bg tactics ws", ACTION_EMERGENCY), NULL)));

    triggers.push_back(new TriggerNode(
        "player has flag",
        NextAction::array(0, new NextAction("bg tactics ws", ACTION_EMERGENCY + 2), NULL)));

    /*triggers.push_back(new TriggerNode(
        "team has flag",
        NextAction::array(0, new NextAction("bg tactics ws", ACTION_HIGH + 20), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy team has flag",
        NextAction::array(0, new NextAction("bg tactics ws", ACTION_HIGH + 20), NULL)));*/

    triggers.push_back(new TriggerNode(
        "enemy flagcarrier near",
        NextAction::array(0, new NextAction("bg tactics ws", ACTION_EMERGENCY + 2), NULL)));

    triggers.push_back(new TriggerNode(
        "medium mana",
        NextAction::array(0, new NextAction("drink", 1.0f), NULL)));
}

void MountStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    /*triggers.push_back(new TriggerNode(
        "no possible targets",
        NextAction::array(0, new NextAction("mount", 1.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "no rpg target",
        NextAction::array(0, new NextAction("mount", 1.0f), NULL)));*/

    /*triggers.push_back(new TriggerNode(
        "often",
        NextAction::array(0, new NextAction("mount", 4.0f), NULL)));*/
}
