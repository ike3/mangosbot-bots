#pragma once

#include "../Action.h"
#include "ChooseMoveDoAction.h"

namespace ai
{
    //Finds the nearest questgiver with an available quest and takes that quest.
    class PickUpQuestAction : public ChooseMoveDoListAction<GuidPosition> {
    public:
        virtual bool getPotentialTarget();

        PickUpQuestAction(PlayerbotAI* ai, string name = "pick up quest", string targetValueName = "active quest givers") : ChooseMoveDoListAction(ai, name, targetValueName) { }

        virtual bool isUseful();                        

        virtual bool ExecuteAction(Event event);        
    };

    //Finds the nearest quest objective.
    class DoQuestObjectiveAction : public ChooseMoveDoListAction<GuidPosition> {
    public:
        DoQuestObjectiveAction(PlayerbotAI* ai, string name = "do quest objective", string targetValueName = "active quest objectives") : ChooseMoveDoListAction(ai, name, targetValueName) { }

        virtual bool ExecuteAction(Event event);
    };

    //Finds the nearest questtaker that the bot has a completed quest for and hand in quest.
    class HandInQuestAction : public ChooseMoveDoListAction<GuidPosition> {
    public:
        HandInQuestAction(PlayerbotAI* ai, string name = "hand in quest", string targetValueName = "active quest takers") : ChooseMoveDoListAction(ai, name, targetValueName) { }

        virtual bool ExecuteAction(Event event);
    };
}
