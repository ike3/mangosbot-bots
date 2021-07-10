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
        virtual string castString(WorldObject* target) { return "cast"; }
    protected:
        bool ncCast = false;
    };

    class CastCustomNcSpellAction : public CastCustomSpellAction
    {
    public:
        CastCustomNcSpellAction(PlayerbotAI* ai, string name = "cast custom nc spell") : CastCustomSpellAction(ai, name) {}
        virtual bool isUseful() { return !bot->IsInCombat(); }
        virtual string castString(WorldObject* target) { return "castnc " + chat->formatWorldobject(target); }
    };

    class CastRandomSpellAction : public CastCustomSpellAction
    {
    public:
        CastRandomSpellAction(PlayerbotAI* ai, string name = "cast random spell") : CastCustomSpellAction(ai, name) {}
        virtual bool AcceptSpell(const SpellEntry* pSpellInfo) { return true; }
        virtual bool Execute(Event event);
    protected:
        bool MultiCast = false;
    };
    
    

    class CraftRandomItemAction : public CastRandomSpellAction
    {
    public:
        CraftRandomItemAction(PlayerbotAI* ai) : CastRandomSpellAction(ai, "craft random item") { MultiCast = true; }
        virtual bool AcceptSpell(const SpellEntry* pSpellInfo) 
        { 
            return pSpellInfo->Effect[0] == SPELL_EFFECT_CREATE_ITEM && pSpellInfo->ReagentCount > 0 && 
#ifdef MANGOSBOT_ZERO
            pSpellInfo->School == 0; 
#else
            pSpellInfo->SchoolMask == 1;
#endif
        }
    };
}
