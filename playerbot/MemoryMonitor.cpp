#pragma once
#include "../botpch.h"
#include "playerbot.h"
#include "PlayerbotAIConfig.h"
#include "MemoryMonitor.h"

#include "../../modules/Bots/ahbot/AhBot.h"
#include "DatabaseEnv.h"
#include "PlayerbotAI.h"

#include "../../modules/Bots/ahbot/AhBotConfig.h"
#define BOOST_STACKTRACE_LINK
#include <boost/stacktrace.hpp>

MemoryMonitor::MemoryMonitor() {}
MemoryMonitor::~MemoryMonitor()
{
}

void MemoryMonitor::Add(string objectType, uint64 object, uint32 level)
{
    objectnumbers[std::this_thread::get_id()][objectType]++;
    if (level == 1 && object % 1000000 == 0)
    {
        ostringstream out; out << boost::stacktrace::stacktrace();
        adds[std::this_thread::get_id()][objectType][object] = make_pair(out.str(), time(0));
    }
}

void MemoryMonitor::Rem(string objectType, uint64 object, uint32 level)
{
    objectnumbers[std::this_thread::get_id()][objectType]--;
    
    if (level == 1 && object % 1000000 == 0)
    {
        if (adds[std::this_thread::get_id()][objectType].find(object) != adds[std::this_thread::get_id()][objectType].end())
            adds[std::this_thread::get_id()][objectType].erase(object);
        else
            rems[std::this_thread::get_id()][objectType][object] = make_pair("rem", time(0));
    }
}

void MemoryMonitor::Print()
{
    int32 div = 0;
    for (auto& t : objectnumbers)
        div += t.second["PlayerbotAI"];

    if (!div) div = 1;

    map<string, int32> nums;

    for (auto& t : objectnumbers)
        for (auto& object : t.second)
            nums[object.first]+=object.second;

    objectnumbersHist.push_back(nums);

    if (objectnumbersHist.size() > 10)
        objectnumbersHist.pop_front();

    for (auto& num : nums)
        sLog.outString("%s : %d (%d)", num.first, num.second / (num.first == "PlayerbotAI" ? 1 : div), (objectnumbersHist.back()[num.first] - objectnumbersHist.front()[num.first]) / (num.first == "PlayerbotAI" ? 1 : div));

    Browse();
}

void MemoryMonitor::Browse()
{
    unordered_map<string,unordered_map<string, uint64>> here;
    unordered_map<string, unordered_map<string, vector<time_t>>> hereTime;

    for (auto& add : adds)
        for(auto t : add.second)
        {
            for (auto& a : t.second)
            {
                bool stillHere = true;
                for (auto& rem : rems)
                {
                    for (auto& r : rem.second[t.first])
                        if (r.first == a.first)
                        {
                            stillHere = false;
                            break;
                        }

                    if (!stillHere)
                        break;
                }

                if (stillHere)
                {
                    here[t.first][a.second.first]++;
                    hereTime[t.first][a.second.first].push_back(a.second.second);
                }
            }           
        }

    for (auto& t : here)
    {
        sLog.outError("[%s] = %d", t.first.c_str(), t.second.size());

        std::vector<std::pair<std::string, int>> top(10);
        std::partial_sort_copy(here[t.first].begin(),
            here[t.first].end(),
            top.begin(),
            top.end(),
            [](std::pair<const std::string, int> const& l,
                std::pair<const std::string, int> const& r)
            {
                return l.second > r.second;
            });

        for (auto& p : top)
        {
            if (hereTime[t.first][p.first].empty())
                continue;

            uint32 f, l, a = 0;

            f = time(0) - hereTime[t.first][p.first].front();
            l = time(0) - hereTime[t.first][p.first].back();

            for (auto p : hereTime[t.first][p.first])
                a += (time(0) - p);

            if (hereTime[t.first][p.first].size())
                a /= hereTime[t.first][p.first].size();

            sLog.outError("<%d> \nfrom: %d\navg: %d\nto: %d\n%s", p.second, f, a, l, p.first.c_str());
        }
    }
}