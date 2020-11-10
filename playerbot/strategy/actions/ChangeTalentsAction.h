#pragma once
#include "botpch.h"
#include "../../playerbot.h"
#include "../Action.h"

namespace ai
{
	class ChangeTalentsAction : public Action {
	public:
		ChangeTalentsAction(PlayerbotAI* ai) : Action(ai, "talents") {}

    public:
        struct TalentListEntry
        {
            int rank;
            int entry;
            TalentEntry const* talentInfo;
            TalentTabEntry const* talentTabInfo;
        };

        virtual bool Execute(Event event);

        virtual bool CheckTalentLink(string link, ostringstream* out);
        virtual bool CheckTalents(Player* bot, std::vector<TalentListEntry> talentList, ostringstream* out);        
        void ApplyTalents(Player* bot, std::vector<TalentListEntry> talentList);

    protected:
        std::vector<TalentListEntry> TalentList(uint32 classMask, bool maxRank = false);
        std::vector<TalentListEntry> GetTalentList(std::vector<TalentListEntry> talentList, Player* bot);
        std::vector<TalentListEntry> GetTalentList(std::vector<TalentListEntry> talentList, string link);
        std::vector<TalentListEntry> GetTalentList(Player* bot);
        std::vector<TalentListEntry> GetTalentList(Player* bot, string link, bool relative = false);
        std::vector<TalentListEntry> GetTalentTree(std::vector<TalentListEntry> talentList, int tree);

        std::vector<TalentListEntry> SubTalentList(std::vector<TalentListEntry> baseList, std::vector<TalentListEntry> deltaList);

        int GetTalentPoints(std::vector<TalentListEntry> talentList);
        string GetTalentLink(std::vector<TalentListEntry> talentList);
    };

}