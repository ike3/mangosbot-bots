#pragma once

#include "../Action.h"
#include "MovementActions.h"
#include "../values/LastMovementValue.h"
#include "UseItemAction.h"

namespace ai
{
    class CheckMountStateAction : public UseItemAction {
    public:
        CheckMountStateAction(PlayerbotAI* ai) : UseItemAction(ai, "check mount state", true) {}

        virtual bool Execute(Event event);
        virtual bool isPossible() { return true; }
        virtual bool isUseful();

    //private:
        bool Mount();

        static uint32 MountSpeed(const ItemPrototype* proto, const bool isFlying = false);
    };

}
