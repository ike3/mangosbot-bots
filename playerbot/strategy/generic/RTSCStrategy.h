#pragma once
#include "../Strategy.h"

namespace ai
{
    class RTSCStrategy : public Strategy
    {
    public:
        RTSCStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "RTSC"; }
    };
}
