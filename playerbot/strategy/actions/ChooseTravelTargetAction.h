#pragma once

#include "../Action.h"
#include "MovementActions.h"
#include "../../Travelmgr.h"

namespace ai
{
    class ChooseTravelTargetAction : public MovementAction {
    public:
        ChooseTravelTargetAction(PlayerbotAI* ai, string name = "choose travel target") : MovementAction(ai, name) {}

        virtual bool Execute(Event event);
        virtual bool isUseful();

        private:


        bool SetTarget(TravelTarget* target, TravelTarget* oldTarget);

        bool getBestDestination(vector<TravelDestination*>* activeDestinations, vector<WorldPosition*>* activePoints);

        bool SetGroupTarget(TravelTarget* target);
        bool SetCurrentTarget(TravelTarget* target, TravelTarget* oldTarget);
        bool SetQuestTarget(TravelTarget* target, bool onlyCompleted = false);
        bool SetNewQuestTarget(TravelTarget* target);
        bool SetRpgTarget(TravelTarget* target);
        bool SetGrindTarget(TravelTarget* target);
        bool SetBossTarget(TravelTarget* target);
        bool SetExploreTarget(TravelTarget* target);
        bool SetCapitalTarget(TravelTarget* target);
        bool SetNullTarget(TravelTarget* target);

    public:
        static TravelDestination* FindDestination(Player* bot, string name);
    private:

        void ReportTravelTarget(TravelTarget* newTarget, TravelTarget* oldTarget);

        virtual bool needForQuest(Unit* target);
        virtual bool needItemForQuest(uint32 itemId, const Quest* questTemplate, const QuestStatusData* questStatus);
    };
}
