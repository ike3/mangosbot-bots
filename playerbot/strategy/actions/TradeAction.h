#pragma once
#include "InventoryAction.h"

namespace ai
{
    class TradeAction : public InventoryAction 
    {
    public:
        TradeAction(PlayerbotAI* ai) : InventoryAction(ai, "trade") {}
        virtual bool ExecuteCommand(Event& event) override;

    private:
        bool TradeItem(const Item& item, int8 slot);

    private:
        static map<string, uint32> slots;
    };
}
