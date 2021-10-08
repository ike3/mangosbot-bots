#pragma once

#include "../Action.h"
#include "RpgAction.h"
#include "../values/LastMovementValue.h"

namespace ai
{
    class ChooseRpgTargetAction : public Action {
    public:
        ChooseRpgTargetAction(PlayerbotAI* ai, string name = "choose rpg target") : Action(ai, name) {}

        virtual bool Execute(Event event);
        virtual bool isUseful();

        static bool isFollowValid(Player* bot, WorldObject* target);
        static bool isFollowValid(Player* bot, WorldPosition pos);
    private:        
        float getMaxRelevance(GuidPosition guidP);
        virtual uint32 HasSameTarget(ObjectGuid guid);
    };

    class ClearRpgTargetAction : public ChooseRpgTargetAction {
    public:
        ClearRpgTargetAction(PlayerbotAI* ai) : ChooseRpgTargetAction(ai, "clear rpg target") {}

        virtual bool Execute(Event event);
        virtual bool isUseful();
    };

}
