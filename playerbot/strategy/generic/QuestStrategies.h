#pragma once
#include "PassTroughStrategy.h"

namespace ai
{
    class QuestStrategy : public PassTroughStrategy
    {
    public:
        QuestStrategy(PlayerbotAI* ai);

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };

    class DefaultQuestStrategy : public QuestStrategy
    {
    public:
        DefaultQuestStrategy(PlayerbotAI* ai);

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "quest"; }
    };

    class AcceptAllQuestsStrategy : public QuestStrategy
    {
    public:
        AcceptAllQuestsStrategy(PlayerbotAI* ai) : QuestStrategy(ai) {};

    public:
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual string getName() { return "accept all quests"; }
    };

    class ActiveQuestStrategy : public QuestStrategy
    {
    public:
        ActiveQuestStrategy(PlayerbotAI* ai) : QuestStrategy(ai) {};

    public:
        virtual void InitTriggers(std::list<TriggerNode*>& triggers);
        virtual string getName() { return "active quest"; }
    };
}
