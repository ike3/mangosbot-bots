#include "PlayerbotAIConfig.h"
#include "BotTests.h"

void LogAnalysis::AnalysePid()
{
    string m_logsDir = sConfig.GetStringDefault("LogsDir");
    if (!m_logsDir.empty())
    {
        if ((m_logsDir.at(m_logsDir.length() - 1) != '/') && (m_logsDir.at(m_logsDir.length() - 1) != '\\'))
            m_logsDir.append("/");
    }
    std::ifstream in(m_logsDir+"activity_pid.csv", std::ifstream::in);

    vector<uint32> activeBots, totalBots, avgDiff;
    
    uint32 runTime, maxBots;

    if (in.fail())
        return;

    do
    {
        std::string line;
        std::getline(in, line);

        if (!line.length())
            continue;

        Tokens tokens = StrSplit(line, ",");
        
        if (tokens[0] == "Timestamp") //Header line
            continue;

        activeBots.push_back(stoi(tokens[6]));
        totalBots.push_back(stoi(tokens[7]));
        avgDiff.push_back(stoi(tokens[2]));

        runTime = stoi(tokens[0]);
        maxBots = stoi(tokens[7]);
    }
    while (in.good());

    uint32 aDiff =0, aBots =0;
    for (uint32 i = 0; i < activeBots.size(); i++)
    {
        aDiff += avgDiff[i];
        aBots += activeBots[i];
    }

    aDiff /= activeBots.size();
    aBots /= activeBots.size();   

    using namespace std::chrono;
    std::chrono::milliseconds ms(runTime);
    auto secs = duration_cast<seconds>(ms);
    ms -= duration_cast<milliseconds>(secs);
    auto mins = duration_cast<minutes>(secs);
    secs -= duration_cast<seconds>(mins);
    auto hour = duration_cast<hours>(mins);
    mins -= duration_cast<minutes>(hour);

    std::stringstream ss;
    ss << hour.count() << " Hours : " << mins.count() << " Minutes : " << secs.count() << " Seconds ";
    

    ostringstream out;

    out << sPlayerbotAIConfig.GetTimestampStr()  << "," << "PID" << ", " << ss.str().c_str() << ", " << aDiff << ", " << aBots;

    sPlayerbotAIConfig.log("log_analysis.csv", out.str().c_str());

    sLog.outString("========= SERVER RUNTIME: %s ==========", ss.str().c_str());
    sLog.outString("========= AVG DIFF [%d] & AVG ACTIVE BOTS [%d]", aDiff, aBots);
}