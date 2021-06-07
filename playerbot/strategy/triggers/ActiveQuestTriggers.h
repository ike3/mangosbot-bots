#pragma once
#include "../Trigger.h"

namespace ai
{
    class FreeQuestLogSlotTrigger : public Trigger
    {
    public:
        FreeQuestLogSlotTrigger(PlayerbotAI* ai) : Trigger(ai, "free quest log slot") {}

        virtual bool IsActive()
        {
            return AI_VALUE(uint8, "free quest log slots") > 1;
        }
    };

    class ActiveObjectivesTrigger : public Trigger
    {
    public:
        ActiveObjectivesTrigger(PlayerbotAI* ai) : Trigger(ai, "active objectives") {}

        virtual bool IsActive()
        {
            return !AI_VALUE(list<GuidPosition>, "active quest objectives").empty();
        }
    };

    class CompletedQuestsTrigger : public Trigger
    {
    public:
        CompletedQuestsTrigger(PlayerbotAI* ai) : Trigger(ai, "completed quests") {}

        virtual bool IsActive()
        {
            return !AI_VALUE(list<GuidPosition>, "active quest takers").empty();
        }
    };
}
