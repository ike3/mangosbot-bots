#pragma once

namespace ai
{
    class ReactionStrategy : public Strategy
    {
    public:
        ReactionStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
        virtual int GetType() { return STRATEGY_TYPE_REACTION; }
    };
}
