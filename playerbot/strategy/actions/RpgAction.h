#pragma once

#include "../Action.h"
#include "ChooseRpgTargetAction.h"
#include "../values/LastMovementValue.h"

namespace ai
{
    class RpgAction : public ChooseRpgTargetAction {
    public:
        RpgAction(PlayerbotAI* ai, string name = "rpg") : ChooseRpgTargetAction(ai, name) {}

        virtual bool Execute(Event event);
        virtual bool isUseful();

    private:
        typedef void (RpgAction::*RpgElement) (ObjectGuid guid);

        virtual bool AddIgnore(ObjectGuid guid);
        virtual bool RemIgnore(ObjectGuid guid);
        virtual bool HasIgnore(ObjectGuid guid);

        void setDelay(bool important = false);

        void stay(ObjectGuid guid);
        void cancel(ObjectGuid guid);
        void emote(ObjectGuid guid);
        void work(ObjectGuid guid);
        void discover(ObjectGuid guid);
        void taxi(ObjectGuid guid);
        void quest(ObjectGuid guid);
        void trade(ObjectGuid guid);
        void repair(ObjectGuid guid);
        void train(ObjectGuid guid);
        void heal(ObjectGuid guid);
        void use(ObjectGuid guid);
        void spell(ObjectGuid guid);
        void craft(ObjectGuid guid);
        void homebind(ObjectGuid guid);
        void queuebg(ObjectGuid guid);
        void buyPetition(ObjectGuid guid);
    };

    class CRpgAction : public RpgAction {
    public:
        CRpgAction(PlayerbotAI* ai) : RpgAction(ai, "crpg") {}

        virtual bool isUseful() { context->GetValue<ObjectGuid>("rpg target")->Set(ObjectGuid()); return true; };
    };
}
