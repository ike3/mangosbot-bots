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
        bool PrintHelp();
        bool PrintActionLine(uint32 idx, string command);
        bool Print(string name);
        bool Edit(string name, uint32 idx, string command);
    };
}
