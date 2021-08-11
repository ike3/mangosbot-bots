#pragma once

#include "../Action.h"
#include "MovementActions.h"
#include "../values/LastMovementValue.h"

namespace ai
{
    class ChooseRpgTargetAction : public MovementAction {
    public:
        ChooseRpgTargetAction(PlayerbotAI* ai, string name = "choose rpg target") : MovementAction(ai, name) {}

        virtual bool Execute(Event event);
        virtual bool isUseful();
        static bool isFollowValid(Player* bot, WorldObject* target);
        static bool isFollowValid(Player* bot, WorldLocation location);
    protected:
        virtual bool CanTrain(ObjectGuid guid);
        virtual BattleGroundTypeId CanQueueBg(ObjectGuid guid);
        virtual bool CanDiscover(ObjectGuid guid);
    private:        
        virtual uint32 HasSameTarget(ObjectGuid guid);
    };

    class ClearRpgTargetAction : public ChooseRpgTargetAction {
    public:
        ClearRpgTargetAction(PlayerbotAI* ai) : ChooseRpgTargetAction(ai, "clear rpg target") {}

        virtual bool Execute(Event event);
        virtual bool isUseful();
    };

}
