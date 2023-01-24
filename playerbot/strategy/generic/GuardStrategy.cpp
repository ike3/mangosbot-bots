#include "botpch.h"
#include "../../playerbot.h"
#include "../values/PositionValue.h"
#include "GuardStrategy.h"

using namespace ai;

NextAction** GuardStrategy::GetDefaultNonCombatActions()
{
    return NextAction::array(0, new NextAction("check mount state", 4.0f), new NextAction("guard", 4.0f), NULL);
}

void GuardStrategy::OnStrategyAdded(BotState state)
{
    // Set the stay position to current position
    AiObjectContext* context = ai->GetAiObjectContext();

    Player* bot = ai->GetBot();
    PositionMap& posMap = AI_VALUE(PositionMap&, "position");
    PositionEntry guardPosition = posMap["guard"];

    guardPosition.Set(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(), bot->GetMapId());
    posMap["guard"] = guardPosition;
}

void GuardStrategy::OnStrategyRemoved(BotState state)
{
    // Remove the saved stay position
    AiObjectContext* context = ai->GetAiObjectContext();
    PositionMap& posMap = AI_VALUE(PositionMap&, "position");
    PositionEntry stayPosition = posMap["guard"];
    if (stayPosition.isSet())
    {
        posMap.erase("guard");
    }
}