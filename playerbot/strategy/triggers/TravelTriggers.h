#pragma once
#include "../Trigger.h"

namespace ai
{
    class NoTravelTargetTrigger : public Trigger
    {
    public:
        NoTravelTargetTrigger(PlayerbotAI* ai) : Trigger(ai, "no travel target") {}

        virtual bool IsActive();
    };

    class FarFromTravelTargetTrigger : public Trigger
    {
    public:
        FarFromTravelTargetTrigger(PlayerbotAI* ai) : Trigger(ai, "far from travel target") {}

        virtual bool IsActive();
    };
}
