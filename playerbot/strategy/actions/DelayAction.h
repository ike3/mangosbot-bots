#pragma once

#include "../../PlayerbotAIConfig.h"
#include "../../RandomPlayerbotMgr.h"
#include "../../ServerFacade.h"
#include "../Action.h"

namespace ai
{
    class DelayAction : public Action
    {
    public:
        DelayAction(PlayerbotAI* ai) : Action(ai, "delay")
        {}

        virtual bool Execute(Event event)
        {
            if (bot->InBattleGround())
                return false;

            if (!sRandomPlayerbotMgr.IsRandomBot(bot) || bot->GetGroup() || ai->GetMaster())
                return false;

            if (sServerFacade.IsInCombat(bot))
                return true;

            uint32 delay = (sPlayerbotAIConfig.passiveDelay * urand(1,100)/100) + sPlayerbotAIConfig.globalCoolDown;

            //delay = delay + ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("nearest friendly players")->Get().size() * urand(0,15);

            ai->SetNextCheckDelay(delay);
            return true;
        }
    };

}
