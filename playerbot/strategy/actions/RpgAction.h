#pragma once

#include "../Action.h"
#include "MovementActions.h"
#include "../values/LastMovementValue.h"

namespace ai
{
    class RpgAction : public MovementAction {
    public:
        RpgAction(PlayerbotAI* ai) : MovementAction(ai, "rpg") {}

        virtual bool Execute(Event event);
        virtual bool isUseful();

    private:
        typedef void (RpgAction::*RpgElement) (ObjectGuid guid);

        virtual bool AddIgnore(ObjectGuid guid);
        virtual bool RemIgnore(ObjectGuid guid);
        virtual bool HasIgnore(ObjectGuid guid);

        void stay(ObjectGuid guid);
        void cancel(ObjectGuid guid);
        void emote(ObjectGuid guid);
        void work(ObjectGuid guid);
        void taxi(ObjectGuid guid);
        void quest(ObjectGuid guid);
        void trade(ObjectGuid guid);
        void repair(ObjectGuid guid);
        void train(ObjectGuid guid);
        void heal(ObjectGuid guid);
        void use(ObjectGuid guid);
        void spell(ObjectGuid guid);

        bool CanTrain(ObjectGuid guid);
    };

}
