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
            if (!sRandomPlayerbotMgr.IsRandomBot(bot) || bot->GetGroup() || ai->GetMaster())
                return false;

            if (sServerFacade.IsInCombat(bot))
                return false;

            if ((double)urand(0, 100) > sPlayerbotAIConfig.randomBotPassiveChance * 100.0)
                return false;

            if (sServerFacade.isMoving(bot))
                bot->StopMoving();
            else
                bot->SetOrientation(M_PI_F * 2 * urand(0, 100) / 100.0f);

            ai->SetNextCheckDelay(sPlayerbotAIConfig.passiveDelay + sPlayerbotAIConfig.globalCoolDown);
            return true;
        }

    };

}
