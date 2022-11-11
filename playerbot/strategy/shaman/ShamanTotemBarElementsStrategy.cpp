#include "botpch.h"
#include "../../playerbot.h"
#include "ShamanMultipliers.h"
#include "ShamanTotemBarElementsStrategy.h"

using namespace ai;

ShamanTotemBarElementsStrategy::ShamanTotemBarElementsStrategy(PlayerbotAI* ai) : GenericShamanStrategy(ai)
{
}

void ShamanTotemBarElementsStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    GenericShamanStrategy::InitTriggers(triggers);
    triggers.push_back(new TriggerNode(
        "call of the elements",
        NextAction::array(0, new NextAction("call of the elements", 16.0f), NULL)));
}
