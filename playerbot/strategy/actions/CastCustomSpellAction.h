#pragma once

#include "../Action.h"
#include "InventoryAction.h"

namespace ai
{
    class CastCustomSpellAction : public InventoryAction
    {
    public:
        CastCustomSpellAction(PlayerbotAI* ai, string name = "cast custom spell") : InventoryAction(ai, name) {}
        virtual bool Execute(Event event);
    };

    class CastRandomSpellAction : public CastCustomSpellAction
    {
    public:
        CastRandomSpellAction(PlayerbotAI* ai) : CastCustomSpellAction(ai, "cast random spell") {}
        virtual bool Execute(Event event);
    };
}
