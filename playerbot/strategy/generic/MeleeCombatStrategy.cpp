#include "botpch.h"
#include "../../playerbot.h"
#include "MeleeCombatStrategy.h"

using namespace ai;


void MeleeCombatStrategy::InitCombatTriggers(list<TriggerNode*> &triggers)
{
    CombatStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "enemy out of melee",
        NextAction::array(0, new NextAction("reach melee", 60.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "enemy too close for melee",
        NextAction::array(0, new NextAction("move out of enemy contact", ACTION_NORMAL + 8), NULL)));
}

void SetBehindCombatStrategy::InitCombatTriggers(list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "not behind target",
        NextAction::array(0, new NextAction("set behind", 59.0f), NULL)));
}
