#pragma once
#include "InventoryAction.h"

namespace ai
{
    class ReadyCheckAction : public InventoryAction
    {
    public:
        ReadyCheckAction(PlayerbotAI* ai, string name = "ready check") : InventoryAction(ai, name) {}
        virtual bool ExecuteCommand(Event& event) override;

    protected:
        bool ReadyCheck();
    };

    class FinishReadyCheckAction : public ReadyCheckAction
    {
    public:
        FinishReadyCheckAction(PlayerbotAI* ai) : ReadyCheckAction(ai, "finish ready check") {}
        virtual bool ExecuteCommand(Event& event) override;
    };
}
