#pragma once
#include "GenericActions.h"

namespace ai
{
    class SaveManaAction : public ChatCommandAction
    {
    public:
        SaveManaAction(PlayerbotAI* ai) : ChatCommandAction(ai, "save mana") {}
        virtual bool ExecuteCommand(Event& event) override;

    private:
        string format(double value);
    };
}
