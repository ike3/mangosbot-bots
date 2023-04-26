#pragma once

#include "MemoryMonitor.h"
#include <iostream>     // std::cout, std::ios
#include <sstream>
#include <time.h>
#include <log.h>
#define BOOST_STACKTRACE_LINK
#include <boost/stacktrace.hpp>

void MemoryMonitor::Add(std::string objectType, uint64 object, int level)
{
    objectnumbers[std::this_thread::get_id()][objectType]++;
    if (level == 1 && (int)object % 1000000 == 0)
    {
        std::ostringstream out; out << boost::stacktrace::stacktrace();
        adds[std::this_thread::get_id()][objectType][object] = make_pair(out.str(), time(0));
    }
}

void MemoryMonitor::Rem(std::string objectType, uint64 object, int level)
{
    objectnumbers[std::this_thread::get_id()][objectType]--;
    
    if (level == 1 && (int)object % 1000000 == 0)
    {
        if (adds[std::this_thread::get_id()][objectType].find(object) != adds[std::this_thread::get_id()][objectType].end())
            adds[std::this_thread::get_id()][objectType].erase(object);
        else
            rems[std::this_thread::get_id()][objectType][object] = std::make_pair("rem", time(0));
    }
}

void MemoryMonitor::Print()
{
    int div = 0;
    for (auto& t : objectnumbers)
        div += t.second["PlayerbotAI"];

    if (!div) div = 1;

    std::map<std::string, int> nums;

    for (auto& t : objectnumbers)
        for (auto& object : t.second)
            nums[object.first]+=object.second;

    objectnumbersHist.push_back(nums);

    if (objectnumbersHist.size() > 10)
        objectnumbersHist.pop_front();

    for (auto& num : nums)
        sLog.outString("%s : %dk (%dk)", num.first, num.second / 1000, (objectnumbersHist.back()[num.first] - objectnumbersHist.front()[num.first]) / 1000);

    Browse();
}

void MemoryMonitor::Browse()
{
    std::unordered_map<std::string,std::unordered_map<std::string, double>> here;
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<time_t>>> hereTime;

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