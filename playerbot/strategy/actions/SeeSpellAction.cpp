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
#include "RtscAction.h"
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

    if (!master)
        return false;

    if (!ai->HasStrategy("RTSC", ai->GetState()))
        return false;

    if (spellId != RTSC_MOVE_SPELL)
        return false;

    SpellEntry const* pSpellInfo = sServerFacade.LookupSpellInfo(spellId);

    SpellCastTargets targets;

#ifdef BUILD_PLAYERBOT
    recvPacket >> targets.ReadForCaster(mover);
#else
    p >> targets.ReadForCaster(ai->GetMaster());
#endif

    WorldPosition spellPosition(master->GetMapId(), targets.m_destPos);
    SET_AI_VALUE(WorldPosition, "see spell location", spellPosition);

    if (ai->HasStrategy("debug", BOT_STATE_NON_COMBAT))
    {
        PathFinder path(bot);

        float x = spellPosition.getX();
        float y = spellPosition.getY();
        float z = spellPosition.getZ();

        ostringstream out;

        out << " area = ";

        out << path.getArea(bot->GetMapId(), x, y, z);

        unsigned short flags = path.getFlags(bot->GetMapId(), x, y, z);

        out << " flags = " << flags;

        if (flags & NAV_GROUND)
            out << ", ground";
        if (flags & NAV_EMPTY)
            out << ", empty";
        if (flags & NAV_GROUND_STEEP)
            out << ", slope";
        if (flags & NAV_WATER)
            out << ", water";
        if (flags & NAV_MAGMA_SLIME)
            out << ", magma slime";

        ai->TellMaster(out);
    }


    bool selected = AI_VALUE(bool, "RTSC selected");
    bool inRange = spellPosition.distance(bot) <= 10;
    string nextAction = AI_VALUE(string, "RTSC next spell action");

    if (nextAction.empty())
    {
        if (!inRange && selected)
            master->GetSession()->SendPlaySpellVisual(bot->GetObjectGuid(), 6372);
        else if (inRange && !selected)
            master->GetSession()->SendPlaySpellVisual(bot->GetObjectGuid(), 5036);

        SET_AI_VALUE(bool, "RTSC selected", inRange);

        if (selected)
            return MoveToSpell(spellPosition);

        return inRange;
    }
    else if (nextAction == "move")
    {
        return MoveToSpell(spellPosition);
    }
    else if (nextAction.find("save ") != std::string::npos)
    {
        string locationName;
        if (nextAction.find("save selected ") != std::string::npos)
        {
            if (!selected)
                return false;
            locationName = nextAction.substr(14);
        }
        else
            locationName = nextAction.substr(5);

        SetFormationOffset(spellPosition);

        SET_AI_VALUE2(WorldPosition, "RTSC saved location", locationName, spellPosition);
        
        Creature* wpCreature = bot->SummonCreature(15631, spellPosition.getX(), spellPosition.getY(), spellPosition.getZ(), spellPosition.getO(), TEMPSPAWN_TIMED_DESPAWN, 2000.0f);
        wpCreature->SetObjectScale(0.5f);
        RESET_AI_VALUE(string, "RTSC next spell action");

        return true;
    }

    return false;        
}

bool SeeSpellAction::SelectSpell(WorldPosition& spellPosition)
{
    Player* master = ai->GetMaster();
    if (spellPosition.distance(bot) <= 5 || AI_VALUE(bool, "RTSC selected"))
    {
        SET_AI_VALUE(bool, "RTSC selected", true);
        master->GetSession()->SendPlaySpellVisual(bot->GetObjectGuid(), 5036);
    }
    return true;
}

bool SeeSpellAction::MoveToSpell(WorldPosition& spellPosition, bool inFormation)
{
    if(inFormation)
        SetFormationOffset(spellPosition);

    if (bot->IsWithinLOS(spellPosition.getX(), spellPosition.getY(), spellPosition.getZ()))
        return MoveNear(spellPosition.getMapId(), spellPosition.getX(), spellPosition.getY(), spellPosition.getZ(), 0);
    else
        return MoveTo(spellPosition.getMapId(), spellPosition.getX(), spellPosition.getY(), spellPosition.getZ(), false, false);
}

void SeeSpellAction::SetFormationOffset(WorldPosition& spellPosition)
{
    Player* master = ai->GetMaster();

    Formation* formation = AI_VALUE(Formation*, "formation");

    WorldLocation formationLocation = formation->GetLocation();

    if (formationLocation.coord_x != 0 || formationLocation.coord_y != 0)
    {
        spellPosition -= WorldPosition(master);
        spellPosition += formationLocation;
    }
}

