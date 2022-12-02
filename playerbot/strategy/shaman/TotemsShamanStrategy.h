#pragma once
#include "GenericShamanStrategy.h"

namespace ai
{
    class TotemsShamanStrategy : public GenericShamanStrategy
    {
    public:
        TotemsShamanStrategy(PlayerbotAI* ai) : GenericShamanStrategy(ai) {}
        string getName() override { return "totems"; }

    private:
        void InitCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
