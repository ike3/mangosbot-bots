#include "botpch.h"
#include "../../playerbot.h"
#include "CheckValuesAction.h"

#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"

#include "../../TravelMgr.h"
#include "../../TravelNode.h"
#include "../values/LastMovementValue.h"
using namespace ai;

CheckValuesAction::CheckValuesAction(PlayerbotAI* ai) : Action(ai, "check values")
{
}

bool CheckValuesAction::Execute(Event event)
{
    if (ai->HasStrategy("debug move", BOT_STATE_NON_COMBAT))
    {
        ai->Ping(bot->GetPositionX()-7.5, bot->GetPositionY()+7.5);

        LastMovement& lastMove = *context->GetValue<LastMovement&>("last movement");

        if(lastMove.lastMoveShort)
            ai->Ping(lastMove.lastMoveShort.getX() - 7.5, lastMove.lastMoveShort.getY() + 7.5);
    }

    if (ai->HasStrategy("map", BOT_STATE_NON_COMBAT) || ai->HasStrategy("map full", BOT_STATE_NON_COMBAT))
    {
        sTravelNodeMap.manageNodes(bot, ai->HasStrategy("map full", BOT_STATE_NON_COMBAT));
    }

    list<ObjectGuid> possible_targets = *context->GetValue<list<ObjectGuid> >("possible targets");
    list<ObjectGuid> all_targets = *context->GetValue<list<ObjectGuid> >("all targets");
    list<ObjectGuid> npcs = *context->GetValue<list<ObjectGuid> >("nearest npcs");
    list<ObjectGuid> corpses = *context->GetValue<list<ObjectGuid> >("nearest corpses");
    list<ObjectGuid> gos = *context->GetValue<list<ObjectGuid> >("nearest game objects");
    list<ObjectGuid> nfp = *context->GetValue<list<ObjectGuid> >("nearest friendly players");
    return true;
}
