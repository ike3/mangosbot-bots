#include "botpch.h"
#include "../../playerbot.h"
#include "SeeSpellAction.h"
#ifdef MANGOS
#include "luaEngine.h"
#endif



using namespace ai;

bool SeeSpellAction::Execute(Event event)
{
    WorldPacket p(event.getPacket()); // 
    uint32 spellId;

    p.rpos(0);
    p >> spellId;   
    
    //ai->TellMaster(to_string(spellId));

    if (spellId != 30758)
        return false;

    SpellCastTargets targets;

#ifdef BUILD_PLAYERBOT
    recvPacket >> targets.ReadForCaster(mover);
#else
    p >> targets.ReadForCaster(ai->GetMaster());
#endif

    Position dest = targets.m_destPos;

    if (bot->IsWithinLOS(dest.GetPositionX(), dest.GetPositionY(), dest.GetPositionZ())) return MoveNear(bot->GetMapId(), dest.GetPositionX(), dest.GetPositionY(), dest.GetPositionZ(), 0);

    return true;
}