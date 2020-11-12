#pragma once
#include "botpch.h"
#include "../../playerbot.h"
#include "../Action.h"

namespace ai
{
    class TalentSpec {
    public:
        #define SORT_BY_DEFAULT 0
        #define SORT_BY_POINTS_TREE 1

        #define ABSOLUTE_DIST 0
        #define SUBSTRACT_OLD_NEW 1
        #define SUBSTRACT_NEW_OLD -1
        #define ADDED_POINTS 2
        #define REMOVED_POINTS -2

        struct TalentListEntry
        {
            int entry;
            int rank;
            int maxRank;
            TalentEntry const* talentInfo;
            TalentTabEntry const* talentTabInfo;
        };

        std::vector<TalentListEntry> talents;

        TalentSpec(uint32 classMask) { GetTalents(classMask); }
        TalentSpec(Player* bot) { GetTalents(bot->getClassMask()); ReadTalents(bot); }
        TalentSpec(Player* bot, string link) { GetTalents(bot->getClassMask()); ReadTalents(link); }

        virtual bool CheckTalents(int maxPoints, ostringstream* out);
        void CropTalents(int maxPoints);
        void ShiftTalents(TalentSpec* oldTalents, int maxPoints);
        void ApplyTalents(Player* bot, ostringstream* out);

        int GetTalentPoints(std::vector<TalentListEntry>& talents, int tabpage = -1);
        int GetTalentPoints(int tabpage = -1) { return GetTalentPoints(talents, tabpage); };
        bool isEarlierVersionOf(TalentSpec& newSpec);
        string GetTalentLink();
    protected:
        void GetTalents(uint32 classMask);
        void SortTalents(std::vector<TalentListEntry>& talents, int sortBy);
        void SortTalents(int sortBy) { SortTalents(talents, sortBy); }

        void ReadTalents(Player* bot);
        void ReadTalents(string link);

        std::vector<TalentListEntry> GetTalentTree(int tabpage);
        std::vector<TalentListEntry> SubTalentList(std::vector<TalentListEntry>& oldList, std::vector<TalentListEntry>& newList, int reverse);
    };

    class ChangeTalentsAction : public Action {
    public:
        ChangeTalentsAction(PlayerbotAI* ai) : Action(ai, "talents") {}

    public:
        virtual bool Execute(Event event);
        virtual bool AutoSelectTalents(ostringstream* out);
    private:
        bool CheckTalentLink(string link, ostringstream* out);
        TalentSpec GetBestPremadeSpec(Player* bot, int spec, bool oldSpec);
    };
}