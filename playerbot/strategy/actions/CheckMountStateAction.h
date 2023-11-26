#pragma once

#include "../Action.h"
#include "MovementActions.h"
#include "../values/LastMovementValue.h"
#include "UseItemAction.h"

namespace ai
{
    class CheckMountStateAction : public UseItemAction 
    {
    public:
        CheckMountStateAction(PlayerbotAI* ai) : UseItemAction(ai, "check mount state", true) {}

        virtual bool Execute(Event& event) override;
        virtual bool isPossible() { return true; }
        virtual bool isUseful();

    private:
        virtual bool CanFly() const;
        bool CanMountInBg() const;
        float GetAttackDistance() const;

    private:
        bool Mount(Player* requester);
        bool UnMount() const;
    };
}
