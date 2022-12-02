#pragma once
#include "../Strategy.h"

namespace ai
{
    class EmoteStrategy : public Strategy
    {
    public:
        EmoteStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "emote"; }

    private:
        void InitNonCombatTriggers(std::list<TriggerNode*> &triggers) override;
    };
}
