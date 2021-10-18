#pragma once
#include "../Value.h"

namespace ai
{
    class ThreatValue : public Uint8CalculatedValue, public Qualified
	{
	public:
        ThreatValue(PlayerbotAI* ai, string name = "threat") : Uint8CalculatedValue(ai, name) {}

    public:
    	virtual uint8 Calculate();

    protected:
    	uint8 Calculate(Unit* target);
    };
}
