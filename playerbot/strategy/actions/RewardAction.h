#pragma once
#include "GenericActions.h"

namespace ai
{
    class RewardAction : public ChatCommandAction
    {
    public:
        RewardAction(PlayerbotAI* ai) : ChatCommandAction(ai, "reward") {}
        virtual bool ExecuteCommand(Event& event) override;

    private:
        bool Reward(uint32 itemId, Object* pNpc);
    };
}