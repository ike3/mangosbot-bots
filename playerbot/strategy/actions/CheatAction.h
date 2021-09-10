#pragma once

#include "../Action.h"

namespace ai
{
    class CheatAction : public Action
    {
    public:
        CheatAction(PlayerbotAI* ai) : Action(ai, "cheat") {}
        virtual bool Execute(Event event);
    private:
        static BotCheatMask GetCheatMask(string cheat);
        static string GetCheatName(BotCheatMask cheatMask);
        void ListCheats();
    };
}
