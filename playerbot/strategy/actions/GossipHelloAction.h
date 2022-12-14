#pragma once
#include "GenericActions.h"

namespace ai
{
    class GossipHelloAction : public ChatCommandAction
    {
    public:
        GossipHelloAction(PlayerbotAI* ai) : ChatCommandAction(ai, "gossip hello") {}
        virtual bool ExecuteCommand(Event& event) override;

    private:
        void TellGossipMenus();
        bool ProcessGossip(int menuToSelect);
        void TellGossipText(uint32 textId);
    };
}
