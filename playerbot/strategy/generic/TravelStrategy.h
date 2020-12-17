#pragma once

namespace ai
{
    class TravelStrategy : public Strategy
    {
    public:
        TravelStrategy(PlayerbotAI* ai);
        virtual string getName() { return "travel"; }

    public:
        virtual NextAction** getDefaultActions();
        virtual void InitTriggers(std::list<TriggerNode*> &triggers);
    };

}
