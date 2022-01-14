#include "botpch.h"
#include "../../playerbot.h"
#include "../../talentspec.h"
#include "ChangeTalentsAction.h"

using namespace ai;

bool ChangeTalentsAction::Execute(Event event)
{
    string param = event.getParam();

    ostringstream out;

    TalentSpec botSpec(bot);

    if (!param.empty())
    {

        if (param.find("auto") != string::npos)
        {
            AutoSelectTalents(&out);
        }
        else  if (param.find("list ") != string::npos)
        {
            listPremadePaths(getPremadePaths(param.substr(5)), &out);
        }
        else  if (param.find("list") != string::npos)
        {
            listPremadePaths(getPremadePaths(""), &out);
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
            if (botSpec.CheckTalentLink(param, &out))
            {
                TalentSpec newSpec(bot, param);
                string specLink = newSpec.GetTalentLink();

                if (crop)
                {
                    newSpec.CropTalents(bot->GetLevel());
                    out << "becomes: " << newSpec.GetTalentLink();
                }
                if (shift)
                {
                    TalentSpec botSpec(bot);
                    newSpec.ShiftTalents(&botSpec, bot->GetLevel());
                    out << "becomes: " << newSpec.GetTalentLink();
                }

                if (newSpec.CheckTalents(bot->GetLevel(), &out))
                {
                    newSpec.ApplyTalents(bot, &out);
                    sRandomPlayerbotMgr.SetValue(bot->GetGUIDLow(), "specNo", 0);
                    sRandomPlayerbotMgr.SetValue(bot->GetGUIDLow(), "specLink", 1, specLink);
                }
            }
            else
            {
                std::vector<TalentPath*> paths = getPremadePaths(param);
                if (paths.size() > 0)
                {
                    out.str("");
                    out.clear();

                    if (paths.size() > 1 && sPlayerbotAIConfig.autoPickTalents != "full")
                    {
                        out << "Found multiple specs: ";
                        listPremadePaths(paths, &out);
                    }
                    else
                    {
                        if (paths.size() > 1)
                            out << "Found " << paths.size() << " possible specs to choose from. ";
                        TalentPath* path = PickPremadePath(paths, sRandomPlayerbotMgr.IsRandomBot(bot));
                        TalentSpec newSpec = *GetBestPremadeSpec(path->id);
                        string specLink = newSpec.GetTalentLink();
                        newSpec.CropTalents(bot->GetLevel());
                        newSpec.ApplyTalents(bot, &out);
                        if (newSpec.GetTalentPoints() > 0)
                        {
                            out << "Apply spec " << "|h|cffffffff" << path->name << " " << newSpec.formatSpec(bot);
                            sRandomPlayerbotMgr.SetValue(bot->GetGUIDLow(), "specNo", path->id + 1);
                            sRandomPlayerbotMgr.SetValue(bot->GetGUIDLow(), "specLink", 0);
                        }
                    }
                }
            }
        }
    }
    else
    {
        uint32 specId = sRandomPlayerbotMgr.GetValue(bot->GetGUIDLow(), "specNo") - 1;
        string specName = "";
        TalentPath* specPath;
        if (specId)
        {
            specPath = getPremadePath(specId);
            if (specPath->id == specId)
                specName = specPath->name;
        }
        out << "My current talent spec is: " << "|h|cffffffff";
        if (specName != "")
            out << specName << " (" << botSpec.formatSpec(bot) << ")";
        else
            out << chat->formatClass(bot, botSpec.highestTree());
        out << " Link: ";
        out << botSpec.GetTalentLink();
    }

    ai->TellMaster(out);

    return true;
}

std::vector<TalentPath*> ChangeTalentsAction::getPremadePaths(string findName)
{
    std::vector<TalentPath*> ret;
    for (auto& path : sPlayerbotAIConfig.classSpecs[bot->getClass()].talentPath)
    {
        if (findName.empty() || path.name.find(findName) != string::npos)
        {
            ret.push_back(&path);
        }
    }

    return ret;
}

std::vector<TalentPath*> ChangeTalentsAction::getPremadePaths(TalentSpec *oldSpec)
{
    std::vector<TalentPath*> ret;
    
    for (auto& path : sPlayerbotAIConfig.classSpecs[bot->getClass()].talentPath)
    {
        TalentSpec newSpec = *GetBestPremadeSpec(path.id);
        newSpec.CropTalents(bot->GetLevel());        
        if (oldSpec->isEarlierVersionOf(newSpec))
        {
            ret.push_back(&path);
        }
    }

    return ret;
}

TalentPath* ChangeTalentsAction::getPremadePath(int id)
{
    for (auto& path : sPlayerbotAIConfig.classSpecs[bot->getClass()].talentPath)
    {
        if (id == path.id)
        {
            return &path;
        }
    }
    return &sPlayerbotAIConfig.classSpecs[bot->getClass()].talentPath[0];
}

void ChangeTalentsAction::listPremadePaths(std::vector<TalentPath*> paths, ostringstream* out)
{
    if (paths.size() == 0)
    {
        *out << "No predefined talents found..";
    }

    *out << "|h|cffffffff";
    for (auto path : paths)
    {
        *out << path->name << " (" << path->talentSpec.back().formatSpec(bot) << "), ";
    }
    out->seekp(-2, out->cur);
    *out << ".";
}

TalentPath* ChangeTalentsAction::PickPremadePath(std::vector<TalentPath*> paths, bool useProbability)
{
    int totProbability = 0;
    int curProbability = 0;

    if(paths.size() == 1)
        return paths[0];

    for (auto path : paths)
    {
        totProbability += useProbability ? path->probability : 1;
    }

    totProbability = irand(0, totProbability);

    for (auto path : paths)
    {
        curProbability += (useProbability ? path->probability : 1);
        if (curProbability >= totProbability)
            return path;
    }

    return paths[0];
}

bool ChangeTalentsAction::AutoSelectTalents(ostringstream* out)
{
    //Does the bot have talentpoints?
    if (bot->GetLevel() < 10)
    {
        *out << "No free talent points.";
        return false;
    }

    uint32 specNo = sRandomPlayerbotMgr.GetValue(bot->GetGUIDLow(), "specNo");
    uint32 specId = specNo - 1;
    string specLink = sRandomPlayerbotMgr.GetData(bot->GetGUIDLow(), "specLink");

    //Continue the current spec
    if (specNo > 0)
    {
        TalentSpec newSpec = *GetBestPremadeSpec(specId);
        newSpec.CropTalents(bot->GetLevel());
        newSpec.ApplyTalents(bot, out);
        if (newSpec.GetTalentPoints() > 0)
        {
            *out << "Upgrading spec " << "|h|cffffffff" << getPremadePath(specId)->name << " (" << newSpec.formatSpec(bot) << ")";
        }
    }
    else if (!specLink.empty())
    {
        TalentSpec newSpec(bot, specLink);
        newSpec.CropTalents(bot->GetLevel());
        newSpec.ApplyTalents(bot, out);
        if (newSpec.GetTalentPoints() > 0)
        {
            *out << "Upgrading saved spec " << "|h|cffffffff" << chat->formatClass(bot, newSpec.highestTree()) << " (" << newSpec.formatSpec(bot) << ")";
        }
    }

    //Spec was not found or not sufficient
    if (bot->GetFreeTalentPoints() > 0 || (!specNo && specLink.empty()))
    {
        TalentSpec oldSpec(bot);
        std::vector<TalentPath*> paths = getPremadePaths(&oldSpec);

        if (paths.size() == 0) //No spec like the old one found. Pick any.
        {
            if (bot->CalculateTalentsPoints() > 0)
                *out << "No specs like the current spec found. ";
            paths = getPremadePaths("");
        }

        if (paths.size() == 0)
        {
            *out << "No predefined talents found for this class.";
            specId = -1;
            //specLink = "";
        }
        else if (paths.size() > 1 && sPlayerbotAIConfig.autoPickTalents != "full" && !sRandomPlayerbotMgr.IsRandomBot(bot))
        {

            *out << "Found multiple specs: ";
            listPremadePaths(paths, out);
        }
        else
        {
            specId = PickPremadePath(paths, sRandomPlayerbotMgr.IsRandomBot(bot))->id;
            TalentSpec newSpec = *GetBestPremadeSpec(specId);
            specLink = newSpec.GetTalentLink();
            newSpec.CropTalents(bot->GetLevel());
            newSpec.ApplyTalents(bot, out);

            if (paths.size() > 1)
                *out << "Found " << paths.size() << " possible specs to choose from. ";

            *out << "Apply spec " << "|h|cffffffff" << getPremadePath(specId)->name << " " << newSpec.formatSpec(bot);
        }
    }

    sRandomPlayerbotMgr.SetValue(bot->GetGUIDLow(), "specNo", specId + 1);
    if (!specLink.empty() && specId == -1)
        sRandomPlayerbotMgr.SetValue(bot->GetGUIDLow(), "specLink", 1, specLink);
    else
        sRandomPlayerbotMgr.SetValue(bot->GetGUIDLow(), "specLink", 0);

    return (specNo == 0) ? false : true;
}

//Returns a pre-made talentspec that best suits the bots current talents. 
TalentSpec* ChangeTalentsAction::GetBestPremadeSpec(int specId)
{
    TalentPath* path = getPremadePath(specId);
    for (auto& spec : path->talentSpec)
    {
        if (spec.points >= bot->CalculateTalentsPoints())
            return &spec;
    }
    if (path->talentSpec.size())
        return &path->talentSpec.back();

    return &sPlayerbotAIConfig.classSpecs[bot->getClassMask()].baseSpec;
}

bool AutoSetTalentsAction::Execute(Event event)
{
    ostringstream out;

    if (sPlayerbotAIConfig.autoPickTalents == "no" && !sRandomPlayerbotMgr.IsRandomBot(bot))
        return false;

    if (bot->GetFreeTalentPoints() <= 0)
        return false;

    AutoSelectTalents(&out);

    ai->TellMaster(out);

    return true;
}


