#pragma once
#include "InventoryAction.h"

namespace ai
{
    class GuildBankAction : public InventoryAction 
    {
    public:
        GuildBankAction(PlayerbotAI* ai) : InventoryAction(ai, "guild bank") {}
        virtual bool ExecuteCommand(Event& event) override;

    private:
        bool ExecuteCommand(string text, GameObject* bank);
        bool MoveFromCharToBank(Item* item, GameObject* bank);
    };
}
