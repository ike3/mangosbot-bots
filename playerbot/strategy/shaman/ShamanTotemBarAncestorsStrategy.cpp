#include "botpch.h"
#include "../../playerbot.h"
#include "ShamanMultipliers.h"
#include "ShamanTotemBarAncestorsStrategy.h"

using namespace ai;

ShamanTotemBarAncestorsStrategy::ShamanTotemBarAncestorsStrategy(PlayerbotAI* ai) : GenericShamanStrategy(ai)
{
}

void ShamanTotemBarAncestorsStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    GenericShamanStrategy::InitTriggers(triggers);
    triggers.push_back(new TriggerNode(
        "call of the ancestors",
        NextAction::array(0, new NextAction("call of the ancestors", 16.0f), NULL)));
}
