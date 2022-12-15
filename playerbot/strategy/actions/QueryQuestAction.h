#pragma once
#include "GenericActions.h"

namespace ai
{
    class QueryQuestAction : public ChatCommandAction
    {
    public:
        QueryQuestAction(PlayerbotAI* ai) : ChatCommandAction(ai, "query quest") {}

    private:
        bool ExecuteCommand(Event& event) override;
        void TellObjectives(uint32 questId);
        void TellObjective(string name, int available, int required);
    };
}