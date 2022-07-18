#include "botpch.h"
#include "../../playerbot.h"
#include "ShamanMultipliers.h"
#include "ShamanTotemBarSpiritsStrategy.h"

using namespace ai;

ShamanTotemBarSpiritsStrategy::ShamanTotemBarSpiritsStrategy(PlayerbotAI* ai) : GenericShamanStrategy(ai)
{
}

void ShamanTotemBarSpiritsStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    GenericShamanStrategy::InitTriggers(triggers);
    triggers.push_back(new TriggerNode(
        "call of the ancestors",
        NextAction::array(0, new NextAction("call of the spirits", 16.0f), NULL)));
}
