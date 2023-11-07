#pragma once

#include "../Action.h"
#include "MovementActions.h"

namespace ai
{
    class TravelTarget;
    class TravelDestination;

    class ChooseTravelTargetAction : public MovementAction {
    public:
        ChooseTravelTargetAction(PlayerbotAI* ai, string name = "choose travel target") : MovementAction(ai, name) {}

        virtual bool Execute(Event& event);
        virtual bool isUseful();

        void getNewTarget(TravelTarget* newTarget, TravelTarget* oldTarget);

    protected:
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
        bool SetNpcFlagTarget(TravelTarget* target, vector<NPCFlags> flags, string name = "", vector<uint32> items = {}, bool force = true);
        bool SetGOTypeTarget(TravelTarget* target, GameobjectTypes type, string name = "", bool force = true);
        bool SetNullTarget(TravelTarget* target);

    public:
        static TravelDestination* FindDestination(Player* bot, string name, bool zones = true, bool npcs = true, bool quests = true, bool mobs = true, bool bosses = true);
    private:
        virtual bool needForQuest(Unit* target);
        virtual bool needItemForQuest(uint32 itemId, const Quest* questTemplate, const QuestStatusData* questStatus);

#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "move to rpg target"; } //Must equal iternal name
        virtual string GetHelpDescription()
        {
            return "This action is used to select a target for the bots to travel to.\n"
                "Bots will travel to specific places for specific reasons.\n"
                "For example bots will travel to a vendor if they need to sell stuff\n"
                "The current destination or those of group members are also options.";
        }
        virtual vector<string> GetUsedActions() { return {}; }
        virtual vector<string> GetUsedValues() { return { "travel target", "group or", "should sell","can sell","can ah sell","should repair","can repair","following party","near leader","should get money","can fight equal","can fight elite","can fight boss","can free move to","rpg target","attack target",}; }
#endif 
    };
}
