#pragma once

#include "../Value.h"
#include "../NamedObjectContext.h"

class PlayerbotAI;

namespace ai
{      
    class VendorHasUsefulItemValue : public BoolCalculatedValue, public Qualified
    {
    public:
        VendorHasUsefulItemValue(PlayerbotAI* ai) : BoolCalculatedValue(ai, "vendor has useful item",2) {}
        virtual bool Calculate();
    };
}

