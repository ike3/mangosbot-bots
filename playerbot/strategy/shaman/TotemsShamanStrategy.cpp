#include "botpch.h"
#include "../../playerbot.h"
#include "ShamanMultipliers.h"
#include "TotemsShamanStrategy.h"

using namespace ai;

void TotemsShamanStrategy::InitCombatTriggers(std::list<TriggerNode*> &triggers)
{
    GenericShamanStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "windfury totem",
        NextAction::array(0, new NextAction("windfury totem", 16.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "mana spring totem",
        NextAction::array(0, new NextAction("mana spring totem", 19.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "strength of earth totem",
        NextAction::array(0, new NextAction("strength of earth totem", 18.0f), NULL)));

    triggers.push_back(new TriggerNode(
        "searing totem",
        NextAction::array(0, new NextAction("searing totem", 17.0f), NULL)));
}
