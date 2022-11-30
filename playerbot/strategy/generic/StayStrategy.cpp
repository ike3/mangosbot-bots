#include "botpch.h"
#include "../../playerbot.h"
#include "../values/PositionValue.h"
#include "StayStrategy.h"

using namespace ai;

NextAction** StayStrategy::getDefaultActions()
{
    return NextAction::array(0, new NextAction("stay", 1.0f), NULL);
}

void StayStrategy::InitTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "return to stay position",
        NextAction::array(0, new NextAction("return to stay position", 1.5f), NULL)));
}

void StayStrategy::OnStrategyAdded()
{
    // Set the stay position to current position
    AiObjectContext* context = ai->GetAiObjectContext();
    
    Player* bot = ai->GetBot();
    PositionMap& posMap = AI_VALUE(PositionMap&, "position");
    PositionEntry stayPosition = posMap["stay position"];

    stayPosition.Set(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(), bot->GetMapId());
    posMap["stay position"] = stayPosition;
}

void StayStrategy::OnStrategyRemoved()
{
    // Remove the saved stay position
    AiObjectContext* context = ai->GetAiObjectContext();
    PositionMap& posMap = AI_VALUE(PositionMap&, "position");
    PositionEntry stayPosition = posMap["stay position"];
    if (stayPosition.isSet())
    {
        posMap.erase("stay position");
    }
}

void SitStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "sit",
        NextAction::array(0, new NextAction("sit", 1.5f), NULL)));
}
