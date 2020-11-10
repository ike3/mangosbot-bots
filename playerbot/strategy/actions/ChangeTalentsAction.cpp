#include "botpch.h"
#include "../../playerbot.h"
#include "ChangeTalentsAction.h"

using namespace ai;

bool ChangeTalentsAction::Execute(Event event)
{
	string param = event.getParam();

    std::vector<TalentListEntry> talentList;

	ostringstream out;

    if (!param.empty())
    {
        out << "Apply talents [" << param << "] ";
        if (CheckTalentLink(param, &out))
        {
            talentList = GetTalentList(bot, param);

            if (CheckTalents(bot, talentList, &out))
            {
                ApplyTalents(bot, talentList);
            }
        }
    }
    else
    {
        talentList = GetTalentList(bot);
        out << "My current talent spec is: ";
        out << GetTalentPoints(GetTalentTree(talentList, 0)) << ":" << GetTalentPoints(GetTalentTree(talentList, 1)) << ":" << GetTalentPoints(GetTalentTree(talentList, 2));
        out << " Link: ";
        out << GetTalentLink(talentList);
    }

    ai->TellMaster(out);

	return false;
}

//Checks a talent link on basic validity.
bool ChangeTalentsAction::CheckTalentLink(string link, ostringstream* out) {

    string validChar = "-";
    string validNums = "012345";
    int nums = 0;

    for (char& c : link) {
        if (validChar.find(c) == string::npos && validNums.find(c) == string::npos)
        {
            *out << "tree link is invalid. Must be in format 0-0-0";
            return false;
        }
        if (validNums.find(c) != string::npos)
            nums++;
    }

    if (nums == 0) {
        *out << "tree link is invalid. Needs atleast one number.";
        return false;
    }

    return true;
}

//Sort a talent list by rank.
bool sortTalentRank(ChangeTalentsAction::TalentListEntry i, ChangeTalentsAction::TalentListEntry j) {
    if (i.rank < j.rank)
        return true;
    return false;
}

//Check the talentspec for errors.
bool ChangeTalentsAction::CheckTalents(Player* bot, std::vector<TalentListEntry> talentList, ostringstream* out)
{
    //Max rank - Current rank
    std::vector<TalentListEntry> errList = SubTalentList(TalentList(bot->getClassMask(), true), talentList);
    SpellEntry const* spellInfo;
    std::sort(errList.begin(), errList.end(), sortTalentRank);

    if (errList.begin()->rank < 0)
    {
        spellInfo = sSpellStore.LookupEntry(errList.begin()->talentInfo->RankID[0]);
        *out << "tree is not for this class. " << spellInfo->SpellName[0] << " has " << (errList.begin()->rank * -1) << " points above max rank.";
        return false;
    }

    if (GetTalentPoints(talentList) + 9 > bot->getLevel())
    {
        *out << "tree is for a higher level. (" << GetTalentPoints(talentList) + 9 << ")";
        return false;
    }

    for (int i = 0; i < 3; i++)
    {
        std::vector<TalentListEntry> talentTree = GetTalentTree(talentList, i);
        int points = 0;

        for (auto& entry : talentTree)
        {
            if (entry.rank > 0 && entry.talentInfo->Row * 5 > points)
            {
                spellInfo = sSpellStore.LookupEntry(entry.talentInfo->RankID[0]);
                *out << "tree is is invalid. Talent " << spellInfo->SpellName[0] << " is selected with only " << points << " in row below it.";
                return false;
            }
            if (entry.rank > 0 && entry.talentInfo->DependsOn)
            {
                bool found = false;
                uint32 spellId;
                SpellEntry const* spellInfodep;

                for (auto& dep : talentTree)
                    if (dep.talentInfo->TalentID == entry.talentInfo->DependsOn)
                    {
                        spellInfodep = sSpellStore.LookupEntry(dep.talentInfo->RankID[0]);
                        if (dep.rank >= entry.talentInfo->DependsOnRank)
                            found = true;
                    }
                if (!found)
                {
                    spellInfo = sSpellStore.LookupEntry(entry.talentInfo->RankID[0]);
                    *out << "tree is is invalid. Talent:" << spellInfo->SpellName[0] << " needs: " << spellInfodep->SpellName[0] << " at rank: " << entry.talentInfo->DependsOnRank;
                    return false;
                }
            }
            points += entry.rank;
        }

    }

    return true;
}

//Set the talents for the bots to the current spec.
void ChangeTalentsAction::ApplyTalents(Player* bot, std::vector<TalentListEntry> talentList)
{
    for (auto& entry : talentList)
        for (int rank = 0; rank < MAX_TALENT_RANK; ++rank)
        {
            uint32 spellId = entry.talentInfo->RankID[rank];

            if (!spellId)
                continue;

            if (spellId == 0)
                continue;

            if (bot->HasSpell(spellId) && entry.rank - 1 != rank)
                bot->removeSpell(spellId, false, false);
            else if (!bot->HasSpell(spellId) && entry.rank -1 == rank)
                bot->learnSpell(spellId, false);
        }
}

//Sorts a talent list by page, row, column.
bool sortTalentMap(ChangeTalentsAction::TalentListEntry i, ChangeTalentsAction::TalentListEntry j) {
    if (i.talentTabInfo->tabpage < j.talentTabInfo->tabpage)
        return true;
    if (i.talentTabInfo->tabpage > j.talentTabInfo->tabpage)
        return false;
    if (i.talentInfo->Row < j.talentInfo->Row)
        return true;
    if (i.talentInfo->Row > j.talentInfo->Row)
        return false;
    if (i.talentInfo->Col < j.talentInfo->Col)
        return true;

    return false;
}

//Returns a base talentlist for a class. Optionally with all ranks at max.
std::vector<ChangeTalentsAction::TalentListEntry> ChangeTalentsAction::TalentList(uint32 classMask, bool maxRank) {
    TalentListEntry entry;
    std::vector<TalentListEntry> retList;

    for (uint32 i = 0; i < sTalentStore.GetNumRows(); ++i)
    {
        TalentEntry const* talentInfo = sTalentStore.LookupEntry(i);
        if (!talentInfo)
            continue;

        TalentTabEntry const* talentTabInfo = sTalentTabStore.LookupEntry(talentInfo->TalentTab);
        if (!talentTabInfo)
            continue;

        if ((classMask & talentTabInfo->ClassMask) == 0)
            continue;

        entry.entry = i;
        entry.rank = 0;
        entry.talentInfo = talentInfo;
        entry.talentTabInfo = talentTabInfo;

        if (maxRank)
            for (int rank = 0; rank < MAX_TALENT_RANK; ++rank)
            {
                uint32 spellId = talentInfo->RankID[rank];
                if (!spellId)
                    continue;

                if (spellId == 0)
                    continue;

                entry.rank = rank + 1;
            }
        retList.push_back(entry);
    }
    std::sort(retList.begin(), retList.end(), sortTalentMap);

    return retList;
}

//Fills the current ranks a player has for each talent.
std::vector<ChangeTalentsAction::TalentListEntry> ChangeTalentsAction::GetTalentList(std::vector<TalentListEntry> talentList, Player* bot) {
    for (auto &entry : talentList)
        for (int rank = 0; rank < MAX_TALENT_RANK; ++rank)
        {
            uint32 spellId = entry.talentInfo->RankID[rank];

            if (!spellId)
                continue;

            if (spellId == 0)
                continue;

            if (bot->HasSpell(spellId))
            {
                entry.rank = rank + 1;
            }
        }
    return talentList;
}

//Fill the selected rank from a wow-head link.
std::vector<ChangeTalentsAction::TalentListEntry> ChangeTalentsAction::GetTalentList(std::vector<TalentListEntry> talentList, string link) {
    int rank = 0;
    int pos = 0;
    int tab = 0;
    std::string chr;

    if (link.substr(pos, 1) == "-") {
        pos++;
        tab++;
    }

    if (link.substr(pos, 1) == "-") {
        pos++;
        tab++;
    }

    for (auto& entry : talentList)
    {
        if (entry.talentTabInfo->tabpage == tab)
        {
            chr = link.substr(pos, 1);
            entry.rank = stoi(chr);
            pos++;
            if (pos <= link.size())
                if (link.substr(pos, 1) == "-")
                {
                    pos++;
                    tab++;
                }
        }
        if (pos > link.size() -1)
            break;
    }

    return talentList;
}

//Returns the current talentlist of a player.
std::vector<ChangeTalentsAction::TalentListEntry> ChangeTalentsAction::GetTalentList(Player* bot) {
    return GetTalentList(TalentList(bot->getClassMask()), bot);
}

//Returns the current talentlist given a link and a player. Relative means relative to current rank.
std::vector<ChangeTalentsAction::TalentListEntry> ChangeTalentsAction::GetTalentList(Player* bot, string link, bool relative) {
    if (relative)
        return GetTalentList(GetTalentList(bot), link);
    else
        return GetTalentList(TalentList(bot->getClassMask()), link);
}

//Returns only a specific tree from a talent list.
std::vector<ChangeTalentsAction::TalentListEntry> ChangeTalentsAction::GetTalentTree(std::vector<TalentListEntry> talentList, int tree)
{
    std::vector<TalentListEntry> retList;

    for (auto &entry : talentList)
        if(entry.talentTabInfo->tabpage == tree)
            retList.push_back(entry);   

    return retList;
}

//Substracts ranks 
std::vector<ChangeTalentsAction::TalentListEntry> ChangeTalentsAction::SubTalentList(std::vector<TalentListEntry> baseList, std::vector<TalentListEntry> deltaList) {

    for (auto& entry : baseList)
        for (auto& subentry : deltaList)
            if(entry.entry == subentry.entry)
                entry.rank = entry.rank - subentry.rank;

    return baseList;
}

//Counts the point in a talent list.
int ChangeTalentsAction::GetTalentPoints(std::vector<TalentListEntry> talentList)
{
    int points = 0;

    for (auto &entry : talentList)
        points = points + entry.rank;

    return points;
}

//Generates a wow-head link from a talent list.
string ChangeTalentsAction::GetTalentLink(std::vector<TalentListEntry> talentList)
{
    string link = "";
    string treeLink[3];
    int points[3];
    int curPoints = 0;

    for (int i = 0; i < 3; i++) {
        std::vector<TalentListEntry> treeList = GetTalentTree(talentList, i);
        points[i] = GetTalentPoints(treeList);
        for (auto &entry : treeList)
        {
            curPoints += entry.rank;
            treeLink[i] += to_string(entry.rank);
            if (curPoints >= points[i])
            {
                curPoints = 0;
                break;
            }
        }
    }

    link = treeLink[0];
    if (treeLink[1] != "0" || treeLink[2] != "0")
        link = link + "-" + treeLink[1];
    if (treeLink[2] != "0")
        link = link + "-" + treeLink[2];

    return link;
}