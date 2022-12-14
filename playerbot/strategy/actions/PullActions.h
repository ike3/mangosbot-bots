#pragma once
#include "GenericSpellActions.h"
#include "GenericActions.h"

namespace ai
{
    class PullMyTargetAction : public ChatCommandAction
    {
    public:
        PullMyTargetAction(PlayerbotAI* ai, string name = "pull my target") : ChatCommandAction(ai, name) {}
    
    private:
        bool ExecuteCommand(Event& event) override;
    };

    class PullStartAction : public Action
    {
    public:
        PullStartAction(PlayerbotAI* ai, string name = "pull start") : Action(ai, name) {}
        bool Execute(Event& event) override;
    };

    class PullAction : public CastSpellAction
    {
    public:
        PullAction(PlayerbotAI* ai, string name = "pull action");
        bool Execute(Event& event) override;

    private:
        string GetTargetName() override { return "pull target"; }
        string GetReachActionName() override { return "reach pull"; }
    };

    class PullEndAction : public Action
    {
    public:
        PullEndAction(PlayerbotAI* ai, string name = "pull end") : Action(ai, name) {}
        bool Execute(Event& event) override;
    };
}
