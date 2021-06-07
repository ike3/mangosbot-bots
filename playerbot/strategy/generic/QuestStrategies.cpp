#include "botpch.h"
#include "../../playerbot.h"
#include "QuestStrategies.h"

using namespace ai;

QuestStrategy::QuestStrategy(PlayerbotAI* ai) : PassTroughStrategy(ai)
{
    supported.push_back("accept quest");
}

void QuestStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    PassTroughStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "quest share",
        NextAction::array(0, new NextAction("accept quest share", relevance), NULL)));
}


void DefaultQuestStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    QuestStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "use game object",
        NextAction::array(0,
            new NextAction("talk to quest giver", relevance), NULL)));

    triggers.push_back(new TriggerNode(
        "gossip hello",
        NextAction::array(0,
            new NextAction("talk to quest giver", relevance), NULL)));

    triggers.push_back(new TriggerNode(
        "complete quest",
        NextAction::array(0, new NextAction("talk to quest giver", relevance), NULL)));
}

DefaultQuestStrategy::DefaultQuestStrategy(PlayerbotAI* ai) : QuestStrategy(ai)
{
}



void AcceptAllQuestsStrategy::InitTriggers(std::list<TriggerNode*> &triggers)
{
    QuestStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "use game object",
        NextAction::array(0,
            new NextAction("talk to quest giver", relevance), new NextAction("accept all quests", relevance), NULL)));

    triggers.push_back(new TriggerNode(
        "gossip hello",
        NextAction::array(0,
            new NextAction("talk to quest giver", relevance), new NextAction("accept all quests", relevance), NULL)));

    triggers.push_back(new TriggerNode(
        "complete quest",
        NextAction::array(0, 
            new NextAction("talk to quest giver", relevance), new NextAction("accept all quests", relevance), NULL)));
}

void ActiveQuestStrategy::InitTriggers(std::list<TriggerNode*>& triggers)
{
    QuestStrategy::InitTriggers(triggers);

    triggers.push_back(new TriggerNode(
        "completed quests",
        NextAction::array(0,
            new NextAction("hand in quest", 1), NULL)));

    triggers.push_back(new TriggerNode(
        "active objectives",
        NextAction::array(0,
            new NextAction("do quest objective", 1), NULL)));

    triggers.push_back(new TriggerNode(
        "free quest log slots",
        NextAction::array(0,
            new NextAction("pick up quest", 1), NULL)));      
}