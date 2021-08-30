#pragma once

#include "../Action.h"
#include "InventoryAction.h"

namespace ai
{
    class DestroyItemAction : public InventoryAction {
    public:
        DestroyItemAction(PlayerbotAI* ai, string name = "destroy") : InventoryAction(ai, name) {}
        virtual bool Execute(Event event);

    protected:
        void DestroyItem(FindItemVisitor* visitor);
    };

    class SmartDestroyItemAction : public DestroyItemAction {
    public:
        SmartDestroyItemAction(PlayerbotAI* ai) : DestroyItemAction(ai, "smart destroy") {}
        virtual bool Execute(Event event);
        virtual bool isUseful() { return !ai->HasActivePlayerMaster(); }
    };

}
