#pragma once
#include "../Trigger.h"

namespace ai
{
    class NoRpgTargetTrigger : public Trigger
    {
    public:
        NoRpgTargetTrigger(PlayerbotAI* ai) : Trigger(ai, "no rpg target") {}

        virtual bool IsActive();
    };

    class FarFromRpgTargetTrigger : public Trigger
    {
    public:
        FarFromRpgTargetTrigger(PlayerbotAI* ai, string name = "far from rpg target") : Trigger(ai,name) {}

        virtual bool IsActive();
    };

    class NearRpgTargetTrigger : public FarFromRpgTargetTrigger
    {
    public:
        NearRpgTargetTrigger(PlayerbotAI* ai) : FarFromRpgTargetTrigger(ai, "near rpg target") {}

        virtual bool IsActive() { return !FarFromRpgTargetTrigger::IsActive(); };
    };
}
