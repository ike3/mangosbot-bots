#include "botpch.h"
#include "../../playerbot.h"
#include "../values/PositionValue.h"
#include "StayStrategy.h"

using namespace ai;

NextAction** StayStrategy::GetDefaultNonCombatActions()
{
    return NextAction::array(0, new NextAction("stay", 1.0f), NULL);
}

ai::NextAction** StayStrategy::GetDefaultCombatActions()
{
    return GetDefaultNonCombatActions();
}

void StayStrategy::InitNonCombatTriggers(std::list<TriggerNode*>& triggers)
{
    triggers.push_back(new TriggerNode(
        "return to stay position",
        NextAction::array(0, new NextAction("return to stay position", 1.5f), NULL)));
}

void StayStrategy::InitCombatTriggers(std::list<TriggerNode*>& triggers)
{
    InitNonCombatTriggers(triggers);
}

void StayStrategy::OnStrategyAdded(BotState state)
{
    // Set the stay position to current position
    AiObjectContext* context = ai->GetAiObjectContext();
    
    Player* bot = ai->GetBot();
    PositionMap& posMap = AI_VALUE(PositionMap&, "position");
    PositionEntry stayPosition = posMap["stay position"];

    stayPosition.Set(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(), bot->GetMapId());
    posMap["stay position"] = stayPosition;
}

void StayStrategy::OnStrategyRemoved(BotState state)
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

void SitStrategy::InitNonCombatTriggers(std::list<TriggerNode*> &triggers)
{
    triggers.push_back(new TriggerNode(
        "sit",
        NextAction::array(0, new NextAction("sit", 1.5f), NULL)));
}
