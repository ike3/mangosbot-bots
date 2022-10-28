#pragma once

#include "../Action.h"

namespace ai
{
    enum QuestListFilter {
        QUEST_LIST_FILTER_SUMMARY = 0,
        QUEST_LIST_FILTER_COMPLETED = 1,
        QUEST_LIST_FILTER_INCOMPLETED = 2,
        QUEST_LIST_FILTER_ALL = QUEST_LIST_FILTER_COMPLETED | QUEST_LIST_FILTER_INCOMPLETED
    };

    enum QuestTravelDetail {
        QUEST_TRAVEL_DETAIL_NONE = 0,
        QUEST_TRAVEL_DETAIL_SUMMARY = 1,
        QUEST_TRAVEL_DETAIL_FULL = 2
    };

    class ListQuestsAction : public Action {
    public:
        ListQuestsAction(PlayerbotAI* ai) : Action(ai, "quests") {}
        virtual bool Execute(Event& event);

    private:
        void ListQuests(QuestListFilter filter, QuestTravelDetail travelDetail = QUEST_TRAVEL_DETAIL_NONE);
        int ListQuests(bool completed, bool silent, QuestTravelDetail travelDetail);

    };

}