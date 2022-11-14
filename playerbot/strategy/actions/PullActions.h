#pragma once

#include "../Action.h"

namespace ai
{
    class PullMyTargetAction : public Action
    {
    public:
        PullMyTargetAction(PlayerbotAI* ai, string name = "pull my target") : Action(ai, name) {}

    public:
        bool Execute(Event& event) override;
        bool isPossible() override;
    };

    class PullStartAction : public Action
    {
    public:
        PullStartAction(PlayerbotAI* ai, string name = "pull start") : Action(ai, name) {}

    public:
        bool Execute(Event& event) override;
    };

    class PullAction : public Action
    {
    public:
        PullAction(PlayerbotAI* ai, string name = "pull action") : Action(ai, name) {}

    public:
        bool Execute(Event& event) override;
    };

    class PullEndAction : public Action
    {
    public:
        PullEndAction(PlayerbotAI* ai, string name = "pull end") : Action(ai, name) {}

    public:
        bool Execute(Event& event) override;
    };
}
