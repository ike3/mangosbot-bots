#pragma once
#include "InventoryAction.h"

namespace ai
{
    class SendMailAction : public InventoryAction
    {
    public:
        SendMailAction(PlayerbotAI* ai) : InventoryAction(ai, "sendmail") {}
        virtual bool ExecuteCommand(Event& event) override;
    };
}