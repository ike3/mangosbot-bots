#pragma once
#include "GenericActions.h"

namespace ai
{
    class CustomStrategyEditAction : public ChatCommandAction
    {
    public:
        CustomStrategyEditAction(PlayerbotAI* ai) : ChatCommandAction(ai, "cs") {}
        virtual bool Execute(Event& event) override;

    private:
        bool PrintHelp(Player* requester);
        bool PrintActionLine(uint32 idx, string command, Player* requester);
        bool Print(string name, Player* requester);
        bool Edit(string name, uint32 idx, string command, Player* requester);
    };
}
