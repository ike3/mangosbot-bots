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

        protected:

            void getNewTarget(TravelTarget* newTarget, TravelTarget* oldTarget);

        void setNewTarget(TravelTarget* newTarget, TravelTarget* oldTarget);
        void ReportTravelTarget(TravelTarget* newTarget, TravelTarget* oldTarget);

        vector<WorldPosition*> getLogicalPoints(vector<WorldPosition*>& travelPoints);
        bool SetBestTarget(TravelTarget* target, vector<TravelDestination*>& activeDestinations);

        bool SetGroupTarget(TravelTarget* target);
        bool SetCurrentTarget(TravelTarget* target, TravelTarget* oldTarget);
        bool SetQuestTarget(TravelTarget* target, bool newQuests = true, bool activeQuests = true, bool completedQuests = true);
        bool SetRpgTarget(TravelTarget* target);
        bool SetGrindTarget(TravelTarget* target);
        bool SetBossTarget(TravelTarget* target);
        bool SetExploreTarget(TravelTarget* target);
        bool SetNpcFlagTarget(TravelTarget* target, vector<NPCFlags> flags, string name = "", vector<uint32> items = {});
        bool SetNullTarget(TravelTarget* target);

    public:
        static TravelDestination* FindDestination(Player* bot, string name);
    private:
        virtual bool needForQuest(Unit* target);
        virtual bool needItemForQuest(uint32 itemId, const Quest* questTemplate, const QuestStatusData* questStatus);
    };
}
