#pragma once
#include "GenericActions.h"

namespace ai
{
    class DropQuestAction : public ChatCommandAction
    {
    public:
        DropQuestAction(PlayerbotAI* ai) : ChatCommandAction(ai, "drop quest") {}
        virtual bool Execute(Event& event) override;
    };

    class CleanQuestLogAction : public ChatCommandAction
    {
    public:
        CleanQuestLogAction(PlayerbotAI* ai) : ChatCommandAction(ai, "clean quest log") {}
        virtual bool Execute(Event& event) override;

        virtual bool isUseful() { return ai->HasStrategy("rpg quest", BotState::BOT_STATE_NON_COMBAT); }

        void DropQuestType(Player* requester, uint8& numQuest, uint8 wantNum = 100, bool isGreen = false, bool hasProgress = false, bool isComplete = false);

        static bool HasProgress(Player* bot, Quest const* quest);
    };
}