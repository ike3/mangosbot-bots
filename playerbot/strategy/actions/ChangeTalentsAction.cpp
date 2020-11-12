#include "botpch.h"
#include "../../playerbot.h"
#include "ChangeTalentsAction.h"

using namespace ai;
using namespace std::placeholders;

bool ChangeTalentsAction::Execute(Event event)
{
	string param = event.getParam();

	ostringstream out;

    if (!param.empty())
    {
        if (param.find("auto") != string::npos)
        {
            AutoSelectTalents(&out);
        }
        else
        {
            bool crop = false;
            bool shift = false;
            if (param.find("do ") != string::npos)
            {
                crop = true;
                param = param.substr(3);
            }
            else if (param.find("shift ") != string::npos)
            {
                shift = true;
                param = param.substr(6);
            }

            out << "Apply talents [" << param << "] ";
            if (CheckTalentLink(param, &out))
            {
                TalentSpec newSpec(bot, param);

                if (crop)
                {
                    newSpec.CropTalents(bot->getLevel() - 9);
                    out << "becomes: " << newSpec.GetTalentLink();
                }
                if (shift)
                {
                    TalentSpec botSpec(bot);
                    newSpec.ShiftTalents(&botSpec, bot->getLevel() - 9);
                    out << "becomes: " << newSpec.GetTalentLink();
                }

                if (newSpec.CheckTalents(bot->getLevel() - 9, &out))
                {
                    newSpec.ApplyTalents(bot, &out);
                }
            }
        }
    }
    else
    {
        TalentSpec botSpec(bot);

        out << "My current talent spec is: ";
        out << botSpec.GetTalentPoints(0) << ":" << botSpec.GetTalentPoints(1) << ":" << botSpec.GetTalentPoints(2);
        out << " Link: ";
        out << botSpec.GetTalentLink();
    }

    ai->TellMaster(out);

	return false;
}

bool ChangeTalentsAction::AutoSelectTalents(ostringstream* out)
{
    //Does the bot have talentpoints?
    if (bot->getLevel() < 10)
        return false;

    TalentSpec botSpec(bot);

    int totSpecs = 0;
    bool pickSpec[10] = { false };

    //Does the bot already has a spec to compare to?
    if (botSpec.GetTalentPoints() > 0)
    {
        string bestLink = "";
        int bestPoints = 0;
        TalentSpec bestSpec = botSpec;

        //Look at the specs that look the most like the current spec.
        for (int i = 0; i < 10; i++)
        {
            TalentSpec newSpec = GetBestPremadeSpec(bot, i, true); 
            newSpec.CropTalents(bot->getLevel() - 9);
            int newPoints = newSpec.GetTalentPoints();          

            if (newPoints == 0)
                continue;
            
            if (newPoints > bestPoints)
            {
                std::fill(pickSpec, pickSpec + 10, false);
                bestSpec = newSpec;
                bestPoints = newPoints;
                totSpecs = 1;
                pickSpec[i] = true;
                continue;
            }
            if (newPoints == bestPoints)
            {
                totSpecs++;
                pickSpec[i] = true;
                continue;
            }
        }


        //If one is found. Apply that one.
        if (totSpecs == 1)
        {
            bestSpec.ShiftTalents(&botSpec, bot->getLevel() - 9);
            bestSpec.ApplyTalents(bot, out);
            *out << "Auto apply talents: " << bestSpec.GetTalentLink();
            return true;
        }
    }

    //We have multiple options. Only continue if on full automation.
    if (sPlayerbotAIConfig.autoPickTalents != "full")
        return false;

    //If we have no options all options are valid.
    if (totSpecs == 0)
    {
        for (int i = 0; i < 10; i++)
        {
            TalentSpec newSpec = GetBestPremadeSpec(bot, i, false);
            if (newSpec.GetTalentPoints() == 0)
                continue;
            
            totSpecs++;
            pickSpec[i] = true;
        }
    }
    
    //Determine sum of probabilties.
    uint32 totProbability = 0;    
    for (int i = 0; i < 10; i++)
    {
        if (!pickSpec[i])
            continue;
        totProbability += sPlayerbotAIConfig.specProbability[bot->getClass()][i];
    }

    //Pick a spec based on probabilities.
    uint32 curProbability = 0;
    uint32 point = urand(0, totProbability);
    for (int i = 0; i < 10; i++)
    {
        if (!pickSpec[i])
            continue;

        if (sPlayerbotAIConfig.specProbability[bot->getClass()][i] + curProbability > point)
        {
            TalentSpec newSpec = GetBestPremadeSpec(bot, i, false);
            newSpec.ShiftTalents(&botSpec, bot->getLevel() - 9);
            newSpec.ApplyTalents(bot, out);
            *out << "Auto apply talents: " << newSpec.GetTalentLink();
            return true;
        }

        curProbability += sPlayerbotAIConfig.specProbability[bot->getClass()][i];
    }

    *out << "No predefined talents found for this class.";

    return false;
}

//Returns a pre-made talentspec that best suits the bots current talents. 
TalentSpec ChangeTalentsAction::GetBestPremadeSpec(Player* bot, int spec, bool oldSpec)
{
    TalentSpec bestSpec(bot->getClassMask());
    TalentSpec botSpec(bot);


    ostringstream out;
    for (int level = 10; level <= 100; level++)
    {
        string link = sPlayerbotAIConfig.premadeLevelSpec[bot->getClass()][spec][level - 10];
        if (link.empty())
            continue;

        //Ignore bad specs.
        if (!CheckTalentLink(link, &out))
        {       
            sLog.outErrorDb("Error with link %s :", link.c_str(),  out.str());
            out.str("");
            out.clear();
            continue;
        }

        TalentSpec newSpec(bot, sPlayerbotAIConfig.premadeLevelSpec[bot->getClass()][spec][level - 10]);

        if (!newSpec.CheckTalents(100, &out))
        {
            sLog.outErrorDb("Error with link %s :", link.c_str(), out.str());
            out.str("");
            out.clear();
            continue;
        }

        //Spec has to look like old spec.
        if (oldSpec)
        {      
            if (botSpec.GetTalentPoints() <= newSpec.GetTalentPoints())
            {
                TalentSpec proSpec = newSpec;
                proSpec.CropTalents(bot->getLevel() - 9);
                if (!botSpec.isEarlierVersionOf(proSpec))
                    continue;
            }
            if (botSpec.GetTalentPoints() >= newSpec.GetTalentPoints() && !newSpec.isEarlierVersionOf(botSpec))
                continue;
        }
        
        bestSpec = newSpec;
        
        if (level >= bot->getLevel())
        {            
            return bestSpec;
        }
    }
    
    return bestSpec;
}

//Checks a talent link on basic validity.
bool ChangeTalentsAction::CheckTalentLink(string link, ostringstream* out) {

    string validChar = "-";
    string validNums = "012345";
    int nums = 0;

    for (char& c : link) {
        if (validChar.find(c) == string::npos && validNums.find(c) == string::npos)
        {
            *out << "talent link is invalid. Must be in format 0-0-0";
            return false;
        }
        if (validNums.find(c) != string::npos)
            nums++;
    }

    if (nums == 0) {
        *out << "talent link is invalid. Needs atleast one number.";
        return false;
    }

    return true;
}

//Check the talentspec for errors.
bool TalentSpec::CheckTalents(int maxPoints, ostringstream* out)
{
    for (auto& entry : talents)
    {
        if (entry.rank > entry.maxRank)
        {
            SpellEntry const* spellInfo = sSpellStore.LookupEntry(entry.talentInfo->RankID[0]);
            *out << "spec is not for this class. " << spellInfo->SpellName[0] << " has " << (entry.rank - entry.maxRank) << " points above max rank.";
            return false;
        }

        if (entry.rank > 0 && entry.talentInfo->DependsOn)
        {
            bool found = false;
            uint32 spellId;
            SpellEntry const* spellInfodep;

            for (auto& dep : talents)
                if (dep.talentInfo->TalentID == entry.talentInfo->DependsOn)
                {
                    spellInfodep = sSpellStore.LookupEntry(dep.talentInfo->RankID[0]);
                    if (dep.rank >= entry.talentInfo->DependsOnRank)
                        found = true;
                }
            if (!found)
            {
                SpellEntry const* spellInfo = sSpellStore.LookupEntry(entry.talentInfo->RankID[0]);
                *out << "spec is is invalid. Talent:" << spellInfo->SpellName[0] << " needs: " << spellInfodep->SpellName[0] << " at rank: " << entry.talentInfo->DependsOnRank;
                return false;
            }
        }
    }

    for (int i = 0; i < 3; i++)
    {
        std::vector<TalentListEntry> talentTree = GetTalentTree(i);
        int points = 0;

        for (auto& entry : talentTree)
        {
            if (entry.rank > 0 && entry.talentInfo->Row * 5 > points)
            {
                SpellEntry const* spellInfo = sSpellStore.LookupEntry(entry.talentInfo->RankID[0]);
                *out << "spec is is invalid. Talent " << spellInfo->SpellName[0] << " is selected with only " << points << " in row below it.";
                return false;
            }
            points += entry.rank;
        }
    }

    if (GetTalentPoints() > maxPoints)
    {
        *out << "spec is for a higher level. (" << GetTalentPoints() + 9 << ")";
        return false;
    }

    return true;
}

//Set the talents for the bots to the current spec.
void TalentSpec::ApplyTalents(Player* bot, ostringstream* out)
{
    for (auto& entry : talents)
        for (int rank = 0; rank < MAX_TALENT_RANK; ++rank)
        {
            uint32 spellId = entry.talentInfo->RankID[rank];

            if (!spellId)
                continue;

            if (spellId == 0)
                continue;

            if (bot->HasSpell(spellId) && entry.rank - 1 != rank)
            {
                bot->removeSpell(spellId, false, false);
            }
            else if (!bot->HasSpell(spellId) && entry.rank - 1 == rank)
            {
                bot->learnSpell(spellId, false);
            }
        }
}

//Returns a base talentlist for a class.
void TalentSpec::GetTalents(uint32 classMask) {
    TalentListEntry entry;

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

        for (int rank = 0; rank < MAX_TALENT_RANK; ++rank)
        {
            uint32 spellId = talentInfo->RankID[rank];
            if (!spellId)
                continue;

            if (spellId == 0)
                continue;

            entry.maxRank = rank + 1;
        }
        talents.push_back(entry);
    }
    SortTalents(talents, SORT_BY_DEFAULT);
}

//Sorts a talent list by page, row, column.
bool sortTalentMap(TalentSpec::TalentListEntry i, TalentSpec::TalentListEntry j, int *tabSort) {
    if (tabSort[i.talentTabInfo->tabpage] < tabSort[j.talentTabInfo->tabpage])
        return true;
    if (tabSort[i.talentTabInfo->tabpage] > tabSort[j.talentTabInfo->tabpage])
        return false;
    if (i.talentInfo->Row < j.talentInfo->Row)
        return true;
    if (i.talentInfo->Row > j.talentInfo->Row)
        return false;
    if (i.talentInfo->Col < j.talentInfo->Col)
        return true;

    return false;
}

//Sort the talents.
void TalentSpec::SortTalents(std::vector<TalentListEntry> &talents, int sortBy)
{
    switch (sortBy)
    {
    case SORT_BY_DEFAULT:
    {
        int tabSort[] = { 0,1,2 };
        std::sort(talents.begin(), talents.end(), std::bind(sortTalentMap, _1, _2, tabSort));
        break;
    }
    case SORT_BY_POINTS_TREE:
    {
        int tabSort[] = { GetTalentPoints(talents, 0) * -100 - irand(0, 99),GetTalentPoints(talents, 1) * -100 - irand(0, 99),GetTalentPoints(talents, 2) * -100 - irand(0, 99) };
        std::sort(talents.begin(), talents.end(), std::bind(sortTalentMap, _1, _2, tabSort));
        break;
    }
    }
}

//Set the talent ranks to the current rank of the player.
void TalentSpec::ReadTalents(Player* bot) {
    for (auto &entry : talents)
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
}

//Set the talent ranks to the ranks of the link.
void TalentSpec::ReadTalents(string link) {
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

    for (auto& entry : talents)
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
    };
}

//Returns only a specific tree from a talent list.
std::vector<TalentSpec::TalentListEntry> TalentSpec::GetTalentTree(int tabpage)
{
    std::vector<TalentListEntry> retList;

    for (auto &entry : talents)
        if(entry.talentTabInfo->tabpage == tabpage)
            retList.push_back(entry);   

    return retList;
}

//Counts the point in a talent list.
int TalentSpec::GetTalentPoints(std::vector<TalentListEntry> &talents, int tabpage)
{
    int points = 0;

    for (auto& entry : talents)
        if (tabpage == -1 || entry.talentTabInfo->tabpage == tabpage)
            points = points + entry.rank;

    return points;
}

//Generates a wow-head link from a talent list.
string TalentSpec::GetTalentLink()
{
    string link = "";
    string treeLink[3];
    int points[3];
    int curPoints = 0;

    for (int i = 0; i < 3; i++) {
        points[i] = GetTalentPoints(i);
        for (auto &entry : GetTalentTree(i))
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

//Removes talentpoints to match the level
void TalentSpec::CropTalents(int maxPoints)
{
    if (GetTalentPoints() <= maxPoints)
        return;

    SortTalents(talents, SORT_BY_POINTS_TREE);
    
    int points = 0;

    for (auto& entry : talents)
    {
        if (points + entry.rank > maxPoints)
            entry.rank = max(0, maxPoints - points);
        points += entry.rank;
    }

    SortTalents(talents, SORT_BY_DEFAULT);
}

//Substracts ranks. Follows the sorting of the newList.
std::vector<TalentSpec::TalentListEntry> TalentSpec::SubTalentList(std::vector<TalentListEntry>& oldList, std::vector<TalentListEntry>& newList, int reverse = SUBSTRACT_OLD_NEW) {
    std::vector<TalentSpec::TalentListEntry> deltaList = newList;
    for (auto& newentry : deltaList)
        for (auto& oldentry : oldList)
            if (oldentry.entry == newentry.entry)
                if(reverse == ABSOLUTE_DIST)
                    newentry.rank = abs(newentry.rank - oldentry.rank);
                else if(reverse == ADDED_POINTS || reverse == REMOVED_POINTS)
                    newentry.rank = max(0,(newentry.rank - oldentry.rank) * (reverse/2));
                else
                    newentry.rank = (newentry.rank - oldentry.rank) * reverse;

    return deltaList;
}

bool TalentSpec::isEarlierVersionOf(TalentSpec& newSpec)
{
    for (auto& newentry : newSpec.talents)
        for (auto& oldentry : talents)
            if (oldentry.entry == newentry.entry)
                if (oldentry.rank > newentry.rank)
                    return false;
    return true;
}


//Modifies current talents towards new talents up to a maxium of points.
void TalentSpec::ShiftTalents(TalentSpec *currentSpec, int maxPoints)
{
    int currentPoints = currentSpec->GetTalentPoints();

    if (currentPoints >= maxPoints) //We have no more points to spend. Better reset and crop
    {
        CropTalents(maxPoints);
        return;
    }

    SortTalents(SORT_BY_POINTS_TREE); //Apply points first to the largest new tree.
    
    std::vector<TalentSpec::TalentListEntry> deltaList = SubTalentList(currentSpec->talents, talents);

    for (auto& entry : deltaList)
    {
        if (entry.rank < 0) //We have to remove talents. Might as well reset and crop the new list.
        {
            CropTalents(maxPoints);
            return;
        }
    }

    //Start from the current spec.
    talents = currentSpec->talents;

    for (auto& entry : deltaList)
    {
        if (entry.rank + currentPoints > maxPoints) //Running out of points. Only apply what we have left.
            entry.rank = max(0, maxPoints - currentPoints);

        for (auto& subentry : talents)
            if (entry.entry == subentry.entry)
                subentry.rank = subentry.rank + entry.rank;

        currentPoints = currentPoints + entry.rank;
    }
}