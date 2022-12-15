#pragma once
#include "GenericActions.h"

namespace ai
{
    class BankAction : public ChatCommandAction
    {
    public:
        BankAction(PlayerbotAI* ai) : ChatCommandAction(ai, "bank") {}
        virtual bool Execute(Event& event) override;

    private:
        bool ExecuteCommand(string text, Unit* bank);
        void ListItems();
        bool Withdraw(const uint32 itemid);
        bool Deposit(Item* pItem);
        Item* FindItemInBank(uint32 ItemId);
    };
}
