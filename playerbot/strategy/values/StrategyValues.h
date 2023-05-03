#pragma once
#include "../Value.h"

class Unit;

namespace ai
{
    class IsUsefulValue : public BoolCalculatedValue, public Qualified
    {
    public:
        IsUsefulValue(PlayerbotAI* ai, string name = "isUseful") : BoolCalculatedValue(ai, name) {}
        
        virtual bool Calculate();
    };

    
}
