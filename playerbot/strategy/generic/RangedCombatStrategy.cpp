#include "botpch.h"
#include "../../playerbot.h"
#include "RangedCombatStrategy.h"

using namespace ai;


void RangedCombatStrategy::InitTriggers(list<TriggerNode*> &triggers)
{
    CombatStrategy::InitTriggers(triggers);

    /*triggers.push_back(new TriggerNode(
        "enemy too close for shoot",
        NextAction::array(0, new NextAction("flee", ACTION_HIGH), NULL)));*/

    triggers.push_back(new TriggerNode(
        "enemy too close for spell",
        NextAction::array(0, new NextAction("flee", ACTION_HIGH), NULL)));
}
