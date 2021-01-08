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
        typedef void (RpgAction::*RpgElement) (Unit*);

        virtual bool AddIgnore(ObjectGuid guid);
        virtual bool RemIgnore(ObjectGuid guid);
        virtual bool HasIgnore(ObjectGuid guid);

        void stay(Unit* unit);
        void cancel(Unit* unit);
        void emote(Unit* unit);
        void work(Unit* unit);
        void taxi(Unit* unit);
        void quest(Unit* unit);
        void trade(Unit* unit);
        void repair(Unit* unit);
        void train(Unit* unit);
        void heal(Unit* unit);

        bool CanTrain(ObjectGuid guid);
        bool needRepair();
    };

}
