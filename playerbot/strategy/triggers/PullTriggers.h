#pragma once
#include "../Trigger.h"

namespace ai
{
	class PullStartTrigger : public Trigger
	{
	public:
		PullStartTrigger(PlayerbotAI* ai, string name = "pull start") : Trigger(ai, name) {}
		bool IsActive() override;
	};

    class PullEndTrigger : public Trigger
    {
    public:
        PullEndTrigger(PlayerbotAI* ai, string name = "pull end") : Trigger(ai, name) {}
        bool IsActive() override;
    };
}
