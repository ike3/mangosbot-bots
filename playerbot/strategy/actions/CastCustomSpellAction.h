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
        CastRandomSpellAction(PlayerbotAI* ai, string name = "cast random spell") : CastCustomSpellAction(ai, name) {}
        virtual bool AcceptSpell(const SpellEntry* pSpellInfo) {return true; }
        virtual bool Execute(Event event);
    protected:
        bool MultiCast = false;
    };

    class CraftRandomItemAction : public CastRandomSpellAction
    {
    public:
        CraftRandomItemAction(PlayerbotAI* ai) : CastRandomSpellAction(ai, "craft random item") { MultiCast = true; }
        virtual bool AcceptSpell(const SpellEntry* pSpellInfo) { return pSpellInfo->Effect[0] == 24; }        
    };
}
