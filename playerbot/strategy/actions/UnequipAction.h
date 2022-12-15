#pragma once
#include "GenericActions.h"

namespace ai
{
    class UnequipAction : public ChatCommandAction
    {
    public:
        UnequipAction(PlayerbotAI* ai) : ChatCommandAction(ai, "unequip") {}
        virtual bool ExecuteCommand(Event& event) override;

    private:
        void UnequipItem(Item& item);
        void UnequipItem(FindItemVisitor* visitor);
    };
}