#pragma once
#include "MovementActions.h"

namespace ai
{
    class TravelDestination;

	class GoAction : public MovementAction
	{
	public:
		GoAction(PlayerbotAI* ai) : MovementAction(ai, "go") {}
		virtual bool Execute(Event& event) override;
        virtual bool isPossible() override { return true; }

#ifdef GenerateBotHelp
        virtual string GetHelpName() { return "go"; } //Must equal iternal name
        virtual string GetHelpDescription()
        {
            return "This will make the bot move to a specific location.\n"
                "Examples:\n"
                "go [game object] : Move to the specified game object.\n"
                "go <player or npc name> : Move to specified player or npc.\n"
                "go x;y : Move to gps location.\n"
                "go where : Ask the bot where it wants to go.\n"
                "go to <npc, zone or creature name> : Travel up to specified object\n or while following explain how to get there.\n"
                "go travel <npc, zone or creature name> : Set the bots current travel destination\n to specified object and travel (up to) that location.\n";
        }
        virtual vector<string> GetUsedActions() { return {}; }
        virtual vector<string> GetUsedValues() { return { "travel target" ,  "nearest npcs" ,  "nearest friendly players" , "position" }; }
#endif 

    private:
        bool TellWhereToGo(string& param) const;
        bool LeaderAlreadyTraveling(TravelDestination* dest) const;
        bool TellHowToGo(TravelDestination* dest) const;
        bool TravelTo(TravelDestination* dest) const;
        bool MoveToGo(string& param);
        bool MoveToUnit(string& param);
        bool MoveToGps(string& param);
        bool MoveToMapGps(string& param);
        bool MoveToPosition(std::string& param);
	};
}
