#pragma once
#include "../Value.h"

namespace ai
{
    class AoeHealValue : public Uint8CalculatedValue, public Qualified
	{
	public:
    	AoeHealValue(PlayerbotAI* ai, string name = "aoe heal") : Uint8CalculatedValue(ai, name) {}

    public:
    	virtual uint8 Calculate();
    };
}
