#include "botpch.h"
#include "../../playerbot.h"
#include "ShamanMultipliers.h"
#include "ShamanTotemBarElementsStrategy.h"

using namespace ai;

void ShamanTotemBarElementsStrategy::InitCombatTriggers(std::list<TriggerNode*> &triggers)
{
    GenericShamanStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "call of the elements",
        NextAction::array(0, new NextAction("call of the elements", 16.0f), NULL)));
}
