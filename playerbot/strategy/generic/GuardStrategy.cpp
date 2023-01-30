#include "botpch.h"
#include "../../playerbot.h"
#include "../values/PositionValue.h"
#include "GuardStrategy.h"

using namespace ai;

NextAction** GuardStrategy::GetDefaultNonCombatActions()
{
    return NextAction::array(0, new NextAction("check mount state", 4.0f), new NextAction("guard", 4.0f), NULL);
}


NextAction** GuardStrategy::GetDefaultCombatActions()
{
    return GuardStrategy::GetDefaultNonCombatActions();
}
