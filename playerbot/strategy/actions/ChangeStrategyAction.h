#pragma once
#include "GenericActions.h"

namespace ai
{
	class ChangeCombatStrategyAction : public ChatCommandAction
    {
	public:
		ChangeCombatStrategyAction(PlayerbotAI* ai, string name = "co") : ChatCommandAction(ai, name) {}
        virtual bool Execute(Event& event) override;
    };

    class ChangeNonCombatStrategyAction : public ChatCommandAction
    {
    public:
        ChangeNonCombatStrategyAction(PlayerbotAI* ai, string name = "nc") : ChatCommandAction(ai, name) {}
        virtual bool Execute(Event& event) override;
    };

    class ChangeDeadStrategyAction : public ChatCommandAction
    {
    public:
        ChangeDeadStrategyAction(PlayerbotAI* ai, string name = "de") : ChatCommandAction(ai, name) {}
        virtual bool Execute(Event& event) override;
    };

    class ChangeReactionStrategyAction : public ChatCommandAction
    {
    public:
        ChangeReactionStrategyAction(PlayerbotAI* ai, string name = "react") : ChatCommandAction(ai, name) {}
        virtual bool Execute(Event& event) override;
    };

    class ChangeAllStrategyAction : public ChatCommandAction
    {
    public:
        ChangeAllStrategyAction(PlayerbotAI* ai, string name = "change strategy from all", string strategy = "") : ChatCommandAction(ai, name), strategy(strategy) {}
        virtual bool Execute(Event& event) override;

    private:
        string strategy;
    };
}
