#include "botpch.h"
#include "../../playerbot.h"
#include "ReviveFromCorpseAction.h"
#include "../../PlayerbotFactory.h"
#include "../../PlayerbotAIConfig.h"

using namespace ai;

bool ReviveFromCorpseAction::Execute(Event event)
{
    Corpse* corpse = bot->GetCorpse();
    if (!corpse)
        return false;

    if (corpse->GetGhostTime() + bot->GetCorpseReclaimDelay(corpse->GetType() == CORPSE_RESURRECTABLE_PVP) > time(nullptr))
        return false;

    Player* master = GetMaster();
    if (master)
    {
        if (!master->GetPlayerbotAI() && sServerFacade.UnitIsDead(master) && master->GetCorpse()
            && sServerFacade.IsDistanceLessThan(AI_VALUE2(float, "distance", "master target"), sPlayerbotAIConfig.farDistance))
            return false;
    }
    
    WorldPacket packet(CMSG_RECLAIM_CORPSE);
    packet << bot->GetObjectGuid();
    bot->GetSession()->HandleReclaimCorpseOpcode(packet);

    sLog.outDetail("Bot #%d %s:%d <%s> revived", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName());

    context->GetValue<Unit*>("current target")->Set(NULL);
    bot->SetSelectionGuid(ObjectGuid());
    ai->ChangeEngine(BOT_STATE_NON_COMBAT);
    return true;
}

bool FindCorpseAction::Execute(Event event)
{
    if (bot->InBattleGround())
        return false;

    Corpse* corpse = bot->GetCorpse();
    if (!corpse)
        return false;

    Player* master = GetMaster();
    if (master)
    {
        if (!master->GetPlayerbotAI()
            && sServerFacade.IsDistanceLessThan(AI_VALUE2(float, "distance", "master target"), sPlayerbotAIConfig.farDistance))
            return false;
    }

    if (!corpse->IsWithinDistInMap(bot, CORPSE_RECLAIM_RADIUS - 5, true))
    {
        float x = corpse->GetPositionX();
        float y = corpse->GetPositionY();
        float z = corpse->GetPositionZ();;

        sLog.outDetail("Bot #%d %s:%d <%s> moves to body", bot->GetGUIDLow(), bot->GetTeam() == ALLIANCE ? "A" : "H", bot->getLevel(), bot->GetName());

        if (!ai->AllowActive(ALL_ACTIVITY))
        {
            float delay = 1000.0f * bot->GetDistance(corpse) / bot->GetSpeed(MOVE_RUN) + sPlayerbotAIConfig.reactDelay;
            if ((6 * MINUTE * IN_MILLISECONDS - bot->GetDeathTimer()) > delay)
            {
                bot->GetMotionMaster()->Clear();
                bot->TeleportTo(corpse->GetMapId(), x, y, z, 0);
            }
        }
        else
        {
            if (bot->IsWithinLOS(x, y, z))
                return MoveNear(bot->GetMapId(), x, y, z, 0);
            else
                return MoveTo(bot->GetMapId(), x, y, z, false, false);
        }
        return false;
    }
    return true;
}

bool SpiritHealerAction::Execute(Event event)
{
    Corpse* corpse = bot->GetCorpse();
    if (!corpse)
    {
        ai->TellError("I am not a spirit");
        return false;
    }

    list<ObjectGuid> npcs = AI_VALUE(list<ObjectGuid>, "nearest npcs");
    for (list<ObjectGuid>::iterator i = npcs.begin(); i != npcs.end(); i++)
    {
        Unit* unit = ai->GetUnit(*i);
        if (unit && unit->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPIRITHEALER))
        {
            PlayerbotChatHandler ch(bot);
            bot->ResurrectPlayer(0.5f);
            bot->SpawnCorpseBones();
            bot->SaveToDB();
            context->GetValue<Unit*>("current target")->Set(NULL);
            bot->SetSelectionGuid(ObjectGuid());
            ai->TellMaster("Hello");
            return true;
        }
    }

    ai->TellError("Cannot find any spirit healer nearby");
    return false;
}
