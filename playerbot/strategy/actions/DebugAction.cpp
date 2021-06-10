#include "botpch.h"
#include "../../playerbot.h"
#include "DebugAction.h"
#include "../../PlayerbotAIConfig.h"

using namespace ai;

bool DebugAction::Execute(Event event)
{
    Player* master = GetMaster();
    if (!master)
        return false;

    string text = event.getParam();
    if (text == "scan")
    {
        vector<WorldPosition> apos, ipos;
        for (auto p : WorldPosition().getCreaturesNear())
        {
            Unit* unit = ai->GetUnit(p);
            if (unit)
                apos.push_back(WorldPosition(p));
            else
                ipos.push_back(WorldPosition(p));
        }
        ostringstream out; 
        out << "1,";
        WorldPosition().printWKT(apos, out);
        out << "\n0,";
        WorldPosition().printWKT(ipos, out);

        sPlayerbotAIConfig.log("active.csv", out.str().c_str());

    }
    string response = ai->HandleRemoteCommand(text);
    ai->TellMaster(response);
    return true;
}
