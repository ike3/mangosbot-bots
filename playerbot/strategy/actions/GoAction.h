#pragma once
#include "MovementActions.h"

namespace ai
{
	class GoAction : public MovementAction
	{
	public:
		GoAction(PlayerbotAI* ai) : MovementAction(ai, "Go") {}
		virtual bool Execute(Event& event) override;
	};
}
