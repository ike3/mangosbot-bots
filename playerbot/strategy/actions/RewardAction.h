#pragma once
#include "InventoryAction.h"

namespace ai
{
    class RewardAction : public InventoryAction 
    {
    public:
        RewardAction(PlayerbotAI* ai) : InventoryAction(ai, "reward") {}
        virtual bool ExecuteCommand(Event& event) override;

    private:
        bool Reward(uint32 itemId, Object* pNpc);
    };
}