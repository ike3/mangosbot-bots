#include "../botpch.h"
#include "playerbot.h"
#include "PlayerbotAIConfig.h"
#include "PerformanceMonitor.h"

#include "../../modules/Bots/ahbot/AhBot.h"
#include "DatabaseEnv.h"
#include "PlayerbotAI.h"

#include "../../modules/Bots/ahbot/AhBotConfig.h"

PerformanceMonitor::PerformanceMonitor() {}
PerformanceMonitor::~PerformanceMonitor()
{
}

PerformanceMonitorOperation* PerformanceMonitor::start(PerformanceMetric metric, string name)
{
	if (!sPlayerbotAIConfig.perfMonEnabled) return NULL;

#ifdef CMANGOS
	std::lock_guard<std::mutex> guard(lock);
    PerformanceData *pd = data[metric][name];
    if (!pd)
    {
        pd = new PerformanceData();
        pd->minTime = pd->maxTime = pd->totalTime = pd->count = 0;
        data[metric][name] = pd;
    }

	return new PerformanceMonitorOperation(pd);
#endif
}

void PerformanceMonitor::PrintStats()
{
    if(data.empty())
        return;
    uint32 total = data[PERF_MON_TOTAL]["PlayerbotAI::UpdateAIInternal"]->totalTime;

    sLog.outString("--------------------------------------[TOTAL]----------------------------------------------------------");
    sLog.outString("percentage   time    |   min  ..    max (     avg  of     count ) - type : name                        ");


    for (map<PerformanceMetric, map<string, PerformanceData*> >::iterator i = data.begin(); i != data.end(); ++i)
    {
        map<string, PerformanceData*> pdMap = i->second;

        string key;
        switch (i->first)
        {
        case PERF_MON_TRIGGER: key = "T"; break;
        case PERF_MON_VALUE: key = "V"; break;
        case PERF_MON_ACTION: key = "A"; break;
        case PERF_MON_RNDBOT: key = "RndBot"; break;
        case PERF_MON_TOTAL: key = "Total"; break;
        default: key = "?";
        }

        list<string> names;

        for (map<string, PerformanceData*>::iterator j = pdMap.begin(); j != pdMap.end(); ++j)
        {
            if (key == "Total" && j->first != "PlayerbotAI::UpdateAIInternal")
                continue;
            names.push_back(j->first);
        }

        names.sort([pdMap](string i, string j) {return pdMap.at(i)->totalTime < pdMap.at(j)->totalTime; });

        for (auto& name : names)
        {
            PerformanceData* pd = pdMap[name];
            float perc = (float)pd->totalTime / (float)total * 100.0f;
            float secs = (float)pd->totalTime / 1000.0f;
            float avg = (float)pd->totalTime / (float)pd->count;
            if (avg >= 0.5f || pd->maxTime > 10)
            {
                sLog.outString("%7.3f%% %10.3fs | %6u .. %6u (%9.4f of %10u) - %s    : %s"
                    , perc
                    , secs
                    , pd->minTime
                    , pd->maxTime
                    , avg
                    , pd->count
                    , key.c_str()
                    , name.c_str());
            }
        }
        sLog.outString(" ");
    }

    float totalCount = data[PERF_MON_TOTAL]["RandomPlayerbotMgr::FullTick"]->count;
    total = data[PERF_MON_TOTAL]["RandomPlayerbotMgr::FullTick"]->totalTime;

    sLog.outString(" ");
    sLog.outString(" ");
    sLog.outString("---------------------------------------[PER TICK]------------------------------------------------------");
    sLog.outString("percentage   time    |   min  ..    max (     avg  of     count ) - type : name                        ");

    for (map<PerformanceMetric, map<string, PerformanceData*> >::iterator i = data.begin(); i != data.end(); ++i)
    {
        map<string, PerformanceData*> pdMap = i->second;

        string key;
        switch (i->first)
        {
        case PERF_MON_TRIGGER: key = "T"; break;
        case PERF_MON_VALUE: key = "V"; break;
        case PERF_MON_ACTION: key = "A"; break;
        case PERF_MON_RNDBOT: key = "RndBot"; break;
        case PERF_MON_TOTAL: key = "Total"; break;
        default: key = "?";
        }

        list<string> names;

        for (map<string, PerformanceData*>::iterator j = pdMap.begin(); j != pdMap.end(); ++j)
        {
            names.push_back(j->first);
        }

        names.sort([pdMap](string i, string j) {return pdMap.at(i)->totalTime < pdMap.at(j)->totalTime; });

        for (auto& name : names)
        {
            PerformanceData* pd = pdMap[name];
            float perc = (float)pd->totalTime / (float)total * 100.0f;
            uint32 secs = pd->totalTime / totalCount;
            float avg = (float)pd->totalTime / (float)pd->count;
            float amount = (float)pd->count / (float)totalCount;
            if (avg >= 0.5f || pd->maxTime > 10)
            {
                sLog.outString("%7.3f%% %9ums | %6u .. %6u (%9.4f of %10.3f) - %s    : %s"
                    , perc
                    , secs
                    , pd->minTime
                    , pd->maxTime
                    , avg
                    , amount
                    , key.c_str()
                    , name.c_str());
            }
        }
        sLog.outString(" ");
    }

}

void PerformanceMonitor::Reset()
{
    for (map<PerformanceMetric, map<string, PerformanceData*> >::iterator i = data.begin(); i != data.end(); ++i)
    {
        map<string, PerformanceData*> pdMap = i->second;
        for (map<string, PerformanceData*>::iterator j = pdMap.begin(); j != pdMap.end(); ++j)
        {
#ifdef CMANGOS
            PerformanceData* pd = j->second;
            std::lock_guard<std::mutex> guard(pd->lock);
            pd->minTime = pd->maxTime = pd->totalTime = pd->count = 0;
#endif
        }
    }
}

PerformanceMonitorOperation::PerformanceMonitorOperation(PerformanceData* data) : data(data)
{
#ifdef CMANGOS
    started = (std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now())).time_since_epoch();
#endif
}

void PerformanceMonitorOperation::finish()
{
#ifdef CMANGOS
    std::chrono::milliseconds finished = (std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now())).time_since_epoch();
    uint32 elapsed = (finished - started).count();

    std::lock_guard<std::mutex> guard(data->lock);
    if (elapsed > 0)
    {
        if (!data->minTime || data->minTime > elapsed) data->minTime = elapsed;
        if (!data->maxTime || data->maxTime < elapsed) data->maxTime = elapsed;
        data->totalTime += elapsed;
    }
    data->count++;
#endif
    delete this;
}

#ifdef CMANGOS
bool ChatHandler::HandlePerfMonCommand(char* args)
{
    if (!strcmp(args, "reset"))
    {
        sPerformanceMonitor.Reset();
        return true;
    }

    sPerformanceMonitor.PrintStats();
    return true;
}
#endif