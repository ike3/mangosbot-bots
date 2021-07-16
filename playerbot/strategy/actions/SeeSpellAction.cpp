#include "botpch.h"
#include "../../playerbot.h"
#include "SeeSpellAction.h"
#include "../values/Formations.h"
#include "../../ServerFacade.h"
#include "MovementGenerator.h"
#ifdef MANGOS
#include "luaEngine.h"
#endif

#include <MotionGenerators/PathFinder.h>

#include "../../TravelMgr.h"


using namespace ai;

Creature* SeeSpellAction::CreateWps(Player* wpOwner, float x, float y, float z, float o, uint32 entry, Creature* lastWp, bool important)
{
    float dist = wpOwner->GetDistance(x, y, z);
    float delay = 1000.0f * dist / wpOwner->GetSpeed(MOVE_RUN) + sPlayerbotAIConfig.reactDelay;

    if (!important)
        delay *= 0.25;
    Creature* wpCreature = wpOwner->SummonCreature(entry, x, y, z - 1, o, TEMPSPAWN_TIMED_DESPAWN, delay);

    if (!important)
        wpCreature->SetObjectScale(0.2f);

    return wpCreature;
}

float GetAngle(const float x1, const float y1, const float x2, const float y2)
{
    float dx = x1 - x2;
    float dy = y1 - y2;

    float ang = atan2(dy, dx);                              // returns value between -Pi..Pi
    ang = (ang >= 0) ? ang : 2 * M_PI_F + ang;
    return ang;
}

bool SeeSpellAction::Execute(Event event)
{
    WorldPacket p(event.getPacket()); // 
    uint32 spellId;
    Player* master = ai->GetMaster();

    p.rpos(0);
    p >> spellId;

    //ai->TellMaster(to_string(spellId));

    //SpellEntry const* spell = sServerFacade.LookupSpellInfo(spellId);

    //ai->TellMaster(to_string(spell->SpellVisual));

    //Spell* Tspell = new Spell(bot, spell, false);

    //delete Tspell;

    if (spellId != 30758)
        return false;

    SpellCastTargets targets;

#ifdef BUILD_PLAYERBOT
    recvPacket >> targets.ReadForCaster(mover);
#else
    p >> targets.ReadForCaster(ai->GetMaster());
#endif

    Position spellPosition = targets.m_destPos;

    if (bot->GetDistance(spellPosition.GetPositionX(), spellPosition.GetPositionY(), spellPosition.GetPositionZ()) <= 10)
    {
        //ai->TellMaster("In range!");
        if(master)
            master->GetSession()->SendPlaySpellVisual(bot->GetObjectGuid(), 5036);
    }
    else
    {
        //ai->TellMaster("Out of range!");
    }

    float x = spellPosition.GetPositionX();
    float y = spellPosition.GetPositionY();
    float z = spellPosition.GetPositionZ();

    Formation* formation = AI_VALUE(Formation*, "formation");
    WorldLocation formationLocation = formation->GetLocation();
    if (formationLocation.coord_x != 0 || formationLocation.coord_y != 0)
    {
        x = x - master->GetPositionX() + formationLocation.coord_x;
        y = y - master->GetPositionY() + formationLocation.coord_y;
        z = z - master->GetPositionZ() + formationLocation.coord_z;
    }

    if (ai->HasStrategy("debug move", BOT_STATE_NON_COMBAT))
    {                
        
        PathFinder path(bot);

        path.calculate(x, y, z, false);

        Vector3 end = path.getEndPosition();
        Vector3 aend = path.getActualEndPosition();

        PointsArray& points = path.getPath();
        PathType type = path.getPathType();

        ostringstream out;

        out << x << ";" << y << ";" << z << " =";

        out << "path is: ";

        out << type;

        out << " of length ";

        out << points.size();

        out << " with offset ";

        out << (end - aend).length();

        Creature* lastWp;

        for (auto i : points)
        {
            lastWp = CreateWps(bot, i.x, i.y, i.z, 0.0, 11144, lastWp);
        }


        ai->TellMaster(out);

        /*
        PathFinder path(bot);

        ostringstream out;

        out << " area = ";

        out << path.getArea(bot->GetMapId(), x, y, z);

        unsigned short flags = path.getFlags(bot->GetMapId(), x, y, z);

        if (flags & NAV_GROUND)
            out << ", ground";
        if (flags & NAV_MAGMA)
            out << ", magma";
        if (flags & NAV_SLIME)
            out << ", slime";
        if (flags & NAV_WATER)
            out << ", water";
        if (flags & NAV_SLOPE)
            out << ", slope";
        if (flags & NAV_UNUSED1)
            out << ", unused1";
        if (flags & NAV_UNUSED2)
            out << ", unsued2";
        if (flags & NAV_UNUSED3)
            out << ", unsued3";
        if (flags & NAV_UNUSED4)
            out << ", unused4";

        ai->TellMaster(out);
        */

        /*
        WorldPosition pos = WorldPosition(bot->GetMapId(), x, y, z, 0);

        sTravelNodeMap.m_nMapMtx.lock();
        TravelNode* node = sTravelNodeMap.getNode(&pos,NULL, 20);

        if (!node)
        {
            node = sTravelNodeMap.addNode(&pos,"User Node", false, true, false);

            if (node)
            {
                ai->TellMaster("node added");
            }
        }
        else
        {
            if (!node->isImportant())
            {
                sTravelNodeMap.removeNode(node);
                ai->TellMaster("node removed");
            }
            else
            {
                ostringstream out;
                out << "node found" << node->getName();
                ai->TellMaster(out);
            }
        }

        sTravelNodeMap.m_nMapMtx.unlock();
        */
    }

 
    if (bot->IsWithinLOS(x, y, z))
        return MoveNear(bot->GetMapId(), x, y, z, 0);
    else
        return MoveTo(bot->GetMapId(), x, y, z, false, false);
}