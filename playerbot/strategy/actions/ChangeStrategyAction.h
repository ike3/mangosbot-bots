#pragma once
#include "GenericActions.h"

namespace ai
{
	class ChangeCombatStrategyAction : public ChatCommandAction
    {
	public:
		ChangeCombatStrategyAction(PlayerbotAI* ai, string name = "co") : ChatCommandAction(ai, name) {}
        virtual bool ExecuteCommand(Event& event) override;
    };

    class ChangeNonCombatStrategyAction : public ChatCommandAction
    {
    public:
        ChangeNonCombatStrategyAction(PlayerbotAI* ai) : ChatCommandAction(ai, "nc") {}
        virtual bool ExecuteCommand(Event& event) override;
    };

    class ChangeDeadStrategyAction : public ChatCommandAction
    {
    public:
        ChangeDeadStrategyAction(PlayerbotAI* ai) : ChatCommandAction(ai, "de") {}
        virtual bool ExecuteCommand(Event& event) override;
    };

    class ChangeReactionStrategyAction : public ChatCommandAction
    {
    public:
        ChangeReactionStrategyAction(PlayerbotAI* ai) : ChatCommandAction(ai, "react") {}
        virtual bool ExecuteCommand(Event& event) override;
    };

    class ChangeAllStrategyAction : public ChatCommandAction
    {
    public:
        ChangeAllStrategyAction(PlayerbotAI* ai, string name = "change strategy from all", string strategy = "") : ChatCommandAction(ai, name), strategy(strategy) {}
        virtual bool ExecuteCommand(Event& event) override;

    private:
        string strategy;
    };
}
