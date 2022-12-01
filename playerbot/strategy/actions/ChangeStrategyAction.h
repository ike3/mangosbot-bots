#pragma once

#include "../Action.h"

namespace ai
{
	class ChangeCombatStrategyAction : public Action 
    {
	public:
		ChangeCombatStrategyAction(PlayerbotAI* ai, string name = "co") : Action(ai, name) {}

    public:
        virtual bool Execute(Event& event);
    };

    class ChangeNonCombatStrategyAction : public Action 
    {
    public:
        ChangeNonCombatStrategyAction(PlayerbotAI* ai) : Action(ai, "nc") {}

    public:
        virtual bool Execute(Event& event);
    };

    class ChangeDeadStrategyAction : public Action 
    {
    public:
        ChangeDeadStrategyAction(PlayerbotAI* ai) : Action(ai, "de") {}

    public:
        virtual bool Execute(Event& event);
    };

    class ChangeReactionStrategyAction : public Action 
    {
    public:
        ChangeReactionStrategyAction(PlayerbotAI* ai) : Action(ai, "react") {}

    public:
        virtual bool Execute(Event& event);
    };

    class ChangeAllStrategyAction : public Action 
    {
    public:
        ChangeAllStrategyAction(PlayerbotAI* ai, string name = "change strategy from all", string strategy = "") : Action(ai, name), strategy(strategy) {}

    public:
        virtual bool Execute(Event& event);

    private:
        string strategy;
    };
}
