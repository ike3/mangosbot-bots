#include "botpch.h"
#include "../../playerbot.h"
#include "MeleeCombatStrategy.h"

using namespace ai;

void MeleeCombatStrategy::InitCombatTriggers(list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "enemy out of melee",
        NextAction::array(0, new NextAction("reach melee", ACTION_MOVE), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy too close for melee",
        NextAction::array(0, new NextAction("move out of enemy contact", ACTION_NORMAL + 8.0f), NULL)));
}

void SetBehindCombatStrategy::InitCombatTriggers(list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "not behind target",
        NextAction::array(0, new NextAction("set behind", ACTION_MOVE + 5.0f), NULL)));
}
