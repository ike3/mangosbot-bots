#pragma once

#include "../Action.h"
#include "InventoryAction.h"
#include "UseItemAction.h"

namespace ai
{
    class UseTrinketAction : public UseItemAction {
    public:
        UseTrinketAction(PlayerbotAI* ai) : UseItemAction(ai, "use trinket", true) {}
        virtual bool Execute(Event& event);    
        virtual bool isPossible();
        virtual bool isUseful() { return UseItemAction::isUseful() && !bot->HasStealthAura(); }
    };
}