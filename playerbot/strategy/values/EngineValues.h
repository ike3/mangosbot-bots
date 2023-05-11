#pragma once
#include "../Value.h"

namespace ai
{
    class ActionPossibleValue : public BoolCalculatedValue, public Qualified
	{
	public:
        ActionPossibleValue(PlayerbotAI* ai, string name = "action possible") : BoolCalculatedValue(ai, name), Qualified() {}

        virtual bool Calculate();
    };

    class ActionUsefulValue : public BoolCalculatedValue, public Qualified
    {
    public:
        ActionUsefulValue(PlayerbotAI* ai, string name = "action useful") : BoolCalculatedValue(ai, name), Qualified() {}

        virtual bool Calculate();
    };

    class TriggerActiveValue : public BoolCalculatedValue, public Qualified
    {
    public:
        TriggerActiveValue(PlayerbotAI* ai, string name = "trigger active") : BoolCalculatedValue(ai, name), Qualified() {}

        virtual bool Calculate();
    };
}
