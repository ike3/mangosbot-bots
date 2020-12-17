#pragma once

#include "../Action.h"
#include "MovementActions.h"
#include "../values/LastMovementValue.h"

namespace ai
{
    class ChooseTravelTargetAction : public MovementAction {
    public:
        ChooseTravelTargetAction(PlayerbotAI* ai, string name = "choose travel target") : MovementAction(ai, name) {}

        virtual bool Execute(Event event);
        virtual bool isUseful();

        private:
        virtual bool needForQuest(Unit* target);
        virtual bool needItemForQuest(uint32 itemId, const Quest* questTemplate, const QuestStatusData* questStatus);
    };
}
