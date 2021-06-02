#include "botpch.h"
#include "../../playerbot.h"
#include "PvpValues.h"
#include "BattleGroundWS.h"
#include "ServerFacade.h"

using namespace ai;

list<CreatureDataPair const*> BgMastersValue::Calculate()
{
    BattleGroundTypeId bgTypeId;
    switch (stoi(qualifier))
    {
    case 0:
        bgTypeId = BATTLEGROUND_TYPE_NONE;
        break;
    case 1:
        bgTypeId = BATTLEGROUND_AV;
        break;
    case 2:
        bgTypeId = BATTLEGROUND_WS;
        break;
    case 3:
        bgTypeId = BATTLEGROUND_AB;
        break;
    }

    vector<uint32> entries;
    map<Team,map<BattleGroundTypeId, list<uint32>>> battleMastersCache = sRandomPlayerbotMgr.getBattleMastersCache();
    entries.insert(entries.end(), battleMastersCache[TEAM_BOTH_ALLOWED][bgTypeId].begin(), battleMastersCache[TEAM_BOTH_ALLOWED][bgTypeId].end());
    entries.insert(entries.end(), battleMastersCache[ALLIANCE][bgTypeId].begin(), battleMastersCache[ALLIANCE][bgTypeId].end());
    entries.insert(entries.end(), battleMastersCache[HORDE][bgTypeId].begin(), battleMastersCache[HORDE][bgTypeId].end());

    list<CreatureDataPair const*> bmGuids;

    for (auto entry : entries)
    {
        for(auto creaturePair : WorldPosition().getCreaturesNear(0, entry))
        {
            bmGuids.push_back(creaturePair);
        }        
    }

    return bmGuids;
}

CreatureDataPair const* BgMasterValue::Calculate()
{
    CreatureDataPair const* bmPair = NearestBm(false);

    if (!bmPair)
        bmPair = NearestBm(true);
   
    return bmPair;
}

CreatureDataPair const* BgMasterValue::NearestBm(bool allowDead)
{
    WorldPosition botPos(bot);

    list<CreatureDataPair const*> bmPairs = AI_VALUE2(list<CreatureDataPair const*>, "bg masters", qualifier);

    float rDist;
    CreatureDataPair const* rbmPair = nullptr;

    for (auto& bmPair : bmPairs)
    {
        ObjectGuid bmGuid(HIGHGUID_UNIT, bmPair->second.id, bmPair->first);

        if (!bmPair)
            continue;

        WorldPosition bmPos(bmPair);

        float dist = botPos.distance(bmPos); //This is the aproximate travel distance.

        //Did we already find a closer unit that is not dead?
        if (rbmPair && rDist <= dist)
            continue;

        CreatureInfo const* bmTemplate = ObjectMgr::GetCreatureTemplate(bmPair->second.id);

        if (!bmTemplate)
            continue;

        FactionTemplateEntry const* bmFactionEntry = sFactionTemplateStore.LookupEntry(bmTemplate->Faction);

        //Is the unit hostile?
        if (ai->getReaction(bmFactionEntry) < REP_NEUTRAL)
            continue;

        AreaTableEntry const* area = bmPos.getArea();

        if (!area)
            continue;

        //Is the area hostile?
        if (area->team == 4 && bot->GetTeam() == ALLIANCE)
            continue;
        if (area->team == 2 && bot->GetTeam() == HORDE)
            continue;

        if (!allowDead)
        {
            Unit* unit = ai->GetUnit(bmPair);

            if (!unit)
                continue;

            //Is the unit dead?
            if (unit->GetDeathState() == DEAD)
                continue;
        }

        rbmPair = bmPair;
        rDist = dist;
    }

    return rbmPair;
}

Unit* FlagCarrierValue::Calculate()
{
    Unit* carrier = nullptr;

    if (ai->GetBot()->InBattleGround())
    {
        if (ai->GetBot()->GetBattleGroundTypeId() == BattleGroundTypeId::BATTLEGROUND_WS)
        {
            BattleGroundWS *bg = (BattleGroundWS*)ai->GetBot()->GetBattleGround();

            if ((!sameTeam && bot->GetTeam() == HORDE || (sameTeam && bot->GetTeam() == ALLIANCE)) && !bg->GetFlagCarrierGuid(TEAM_INDEX_HORDE).IsEmpty())
                carrier = bg->GetBgMap()->GetPlayer(bg->GetFlagCarrierGuid(TEAM_INDEX_HORDE));

            if ((!sameTeam && bot->GetTeam() == ALLIANCE || (sameTeam && bot->GetTeam() == HORDE)) && !bg->GetFlagCarrierGuid(TEAM_INDEX_ALLIANCE).IsEmpty())
                carrier = bg->GetBgMap()->GetPlayer(bg->GetFlagCarrierGuid(TEAM_INDEX_ALLIANCE));

            if (carrier)
            {
                if (ignoreRange || bot->IsWithinDistInMap(carrier, sPlayerbotAIConfig.sightDistance))
                {
                    return carrier;
                }
            }
        }
    }
    return carrier;
}
