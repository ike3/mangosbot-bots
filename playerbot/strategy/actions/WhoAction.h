#pragma once
#include "InventoryAction.h"

namespace ai
{
    class WhoAction : public InventoryAction 
    {
    public:
        WhoAction(PlayerbotAI* ai) : InventoryAction(ai, "who") {}
        virtual bool ExecuteCommand(Event& event) override;

    private:
        string QueryTrade(string text);
        string QuerySkill(string text);
        string QuerySpec(string text);
    };

}
