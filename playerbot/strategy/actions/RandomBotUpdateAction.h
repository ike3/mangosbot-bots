#pragma once

#include "../../RandomPlayerbotMgr.h"
#include "../Action.h"

namespace ai
{
    class RandomBotUpdateAction : public Action
    {
    public:
        RandomBotUpdateAction(PlayerbotAI* ai) : Action(ai, "random bot update")
        {}

        virtual bool Execute(Event event)
        {
            if (!sRandomPlayerbotMgr.IsRandomBot(bot->GetGUIDLow()))
                return false;

            if (bot->GetGroup() && ai->GetGroupMaster() && (!ai->GetGroupMaster()->GetPlayerbotAI() || ai->GetGroupMaster()->GetPlayerbotAI()->IsRealPlayer()))
                return true;

            if (ai->HasPlayerNearby(sPlayerbotAIConfig.grindDistance))
                return true;

            return sRandomPlayerbotMgr.ProcessBot(bot);
        }

        virtual bool isUseful()
        {
            return AI_VALUE(bool, "random bot update");
        }
    };

}
