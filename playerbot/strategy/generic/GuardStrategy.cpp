#include "botpch.h"
#include "../../playerbot.h"
#include "GuardStrategy.h"

using namespace ai;

NextAction** GuardStrategy::GetDefaultNonCombatActions()
{
    return NextAction::array(0, new NextAction("guard", 4.0f), NULL);
}