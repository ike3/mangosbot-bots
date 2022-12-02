#include "botpch.h"
#include "../../playerbot.h"
#include "DruidMultipliers.h"
#include "MeleeDruidStrategy.h"

using namespace ai;

MeleeDruidStrategy::MeleeDruidStrategy(PlayerbotAI* ai) : CombatStrategy(ai)
{
}

NextAction** MeleeDruidStrategy::GetDefaultCombatActions()
{
    return NextAction::array(0,
            new NextAction("faerie fire", ACTION_NORMAL + 1),
            new NextAction("melee", ACTION_NORMAL),
            NULL);
}

void MeleeDruidStrategy::InitCombatTriggers(std::list<TriggerNode*> &triggers)
{
    CombatStrategy::InitCombatTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "omen of clarity",
        NextAction::array(0, new NextAction("omen of clarity", ACTION_HIGH + 9), NULL)));
}
