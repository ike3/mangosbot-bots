#include "botpch.h"
#include "../../playerbot.h"
#include "TravelAction.h"
#include "../../PlayerbotAIConfig.h"
#include "../../ServerFacade.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "World/WorldState.h"


using namespace ai;
using namespace MaNGOS;

bool TravelAction::Execute(Event event)
{    
    TravelTarget * target = AI_VALUE(TravelTarget *, "travel target");
    
    if (sServerFacade.isMoving(bot))
        return false;

    target->setStatus(TRAVEL_STATUS_WORK);

     Unit* newTarget;
    list<Unit*> targets;
    AnyUnitInObjectRangeCheck u_check(bot, sPlayerbotAIConfig.sightDistance * 2);
    UnitListSearcher<AnyUnitInObjectRangeCheck> searcher(targets, u_check);
    Cell::VisitAllObjects(bot, searcher, sPlayerbotAIConfig.sightDistance * 2);

    for (auto& i : targets)
    {
        newTarget = i;

        if (!newTarget)
            continue;

        if (newTarget->GetMapId() != bot->GetMapId())
            continue;

        if (!newTarget->IsAlive())
            continue;

        if (!newTarget->GetEntry() != target->getDestination()->getEntry())
            continue;

        if (newTarget->IsInCombat())
            continue;

        if (sServerFacade.IsHostileTo(bot, newTarget))
            context->GetValue<ObjectGuid>("pull target")->Set(newTarget->GetObjectGuid());
        else
            context->GetValue<ObjectGuid>("rpg target")->Set(newTarget->GetObjectGuid());

        break;
    }

    return true;
}

bool TravelAction::isUseful()
{
    return false && context->GetValue<TravelTarget *>("travel target")->Get()->isActive() && (!context->GetValue<ObjectGuid>("rpg target")->Get() || !context->GetValue<ObjectGuid>("pull target")->Get());
}

bool MoveToDarkPortalAction::Execute(Event event)
{
    if (bot->GetGroup())
        if (bot->GetGroup()->GetLeaderGuid() != bot->GetObjectGuid() && !bot->GetPlayerbotAI()->GetGroupMaster()->GetPlayerbotAI())
            return false;

#ifndef MANGOSBOT_ZERO
    if (sWorldState.GetExpansion() == EXPANSION_TBC && bot->getLevel() > 57)
    {
        if ((bot->GetTeam() == ALLIANCE && bot->GetQuestStatus(10119) == QUEST_STATUS_NONE) || (bot->GetTeam() == HORDE && bot->GetQuestStatus(9407) == QUEST_STATUS_NONE))
        {
            if (!sServerFacade.IsInCombat(bot))
            {
                if (bot->GetTeam() == ALLIANCE)
                {
                    Quest const* quest = sObjectMgr.GetQuestTemplate(10119);
                    CreatureDataPair const* dataPair = sRandomPlayerbotMgr.GetCreatureDataByEntry(16841);
                    if (quest && dataPair)
                    {
                        ObjectGuid npcGuid = ObjectGuid(HIGHGUID_UNIT, 16841, dataPair->first);
                        Creature* npc = bot->GetMap()->GetCreature(npcGuid);
                        bot->AddQuest(quest, npc);
                    }
                }
                else
                {
                    Quest const* quest = sObjectMgr.GetQuestTemplate(9407);
                    CreatureDataPair const* dataPair = sRandomPlayerbotMgr.GetCreatureDataByEntry(19254);
                    if (quest && dataPair)
                    {
                        ObjectGuid npcGuid = ObjectGuid(HIGHGUID_UNIT, 19254, dataPair->first);
                        Creature* npc = bot->GetMap()->GetCreature(npcGuid);
                        bot->AddQuest(quest, npc);
                    }
                }
            }
            return MoveTo(0, -11815.1f, -3190.39f, -30.7338f, false, true);
        }
        else
        {
            return MoveTo(0, -11906.9f, -3208.53f, -14.8616f, false, true);
        }
    }
    else if (sWorldState.GetExpansion() == EXPANSION_NONE)
    {
        if (urand(0, 1))
            return MoveTo(0, -11891.500f, -3207.010f, -14.798f, false, true);
        else
            return MoveTo(0, -11815.1f, -3190.39f, -30.7338f, false, true);
    }
#endif
    return false;
}

bool MoveToDarkPortalAction::isUseful()
{
#ifndef MANGOSBOT_ZERO
    return bot->getLevel() > 54;
#endif
    return false;
}

bool DarkPortalAzerothAction::Execute(Event event)
{
#ifndef MANGOSBOT_ZERO
    if (sWorldState.GetExpansion() == EXPANSION_TBC && bot->getLevel() > 57)
    {
        WorldPacket packet(CMSG_AREATRIGGER);
        packet << 4354;

        return bot->GetPlayerbotAI()->DoSpecificAction("reach area trigger", Event("travel action", packet));
    }
#endif
    return false;
}

bool DarkPortalAzerothAction::isUseful()
{
#ifndef MANGOSBOT_ZERO
    return bot->getLevel() > 57;
#endif
    return false;
}

bool MoveFromDarkPortalAction::Execute(Event event)
{
    context->GetValue<ObjectGuid>("rpg target")->Set(ObjectGuid());

    if (bot->GetTeam() == ALLIANCE)
        return MoveTo(530, -319.261f, 1027.213, 54.172638f, false, true);
    else
        return MoveTo(530, -180.444f, 1027.947, 54.181538f, false, true);

    return false;
}
