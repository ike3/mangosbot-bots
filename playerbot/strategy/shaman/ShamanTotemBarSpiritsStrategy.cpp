#include "botpch.h"
#include "../../playerbot.h"
#include "ShamanMultipliers.h"
#include "ShamanTotemBarSpiritsStrategy.h"

using namespace ai;

void ShamanTotemBarSpiritsStrategy::InitCombatTriggers(std::list<TriggerNode*> &triggers)
{
    GenericShamanStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "call of the ancestors",
        NextAction::array(0, new NextAction("call of the spirits", 16.0f), NULL)));
}
