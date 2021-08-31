#pragma once

#include "../Action.h"

namespace ai
{
    class DropQuestAction : public Action {
    public:
        DropQuestAction(PlayerbotAI* ai) : Action(ai, "drop quest") {}
        virtual bool Execute(Event event);
    };

    class CleanQuestLogAction : public Action {
    public:
        CleanQuestLogAction(PlayerbotAI* ai) : Action(ai, "clean quest log") {}
        virtual bool Execute(Event event);

        void DropQuestType(uint8& numQuest, uint8 wantNum = 100, bool isGreen = false, bool hasProgress = false, bool isComplete = false);

        static bool HasProgress(Player* bot, Quest const* quest);
    };
}