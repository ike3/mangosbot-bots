#pragma once

namespace ai
{
    class ReactionStrategy : public Strategy
    {
    public:
        ReactionStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        virtual int GetType() override { return STRATEGY_TYPE_REACTION; }

    protected:
        virtual void InitReactionTriggers(std::list<TriggerNode*>& triggers) override;
    };
}
