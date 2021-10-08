#pragma once
#include "../Value.h"

namespace ai
{
   
    class ItemsUsefulToGiveValue : public CalculatedValue< list<Item*>>, public Qualified
	{
	public:
        ItemsUsefulToGiveValue(PlayerbotAI* ai, string name = "useful to give") : CalculatedValue(ai, name) {}

    public:
        list<Item*> Calculate();
    };
}