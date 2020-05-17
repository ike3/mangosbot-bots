#include "../botpch.h"
#include "playerbot.h"
#include "FleeManager.h"
#include "PlayerbotAIConfig.h"
#include "Group.h"
#include "strategy/values/LastMovementValue.h"
#include "ServerFacade.h"

using namespace ai;
using namespace std;

void FleeManager::calculateDistanceToPlayers(FleePoint *point)
{
	Group* group = bot->GetGroup();
	if (!group)
		return;

	for (GroupReference *gref = group->GetFirstMember(); gref; gref = gref->next())
    {
		Player* player = gref->getSource();
		if(player == bot)
			continue;

		float d = sServerFacade.GetDistance2d(player, point->x, point->y);
		point->toAllPlayers.probe(d);
		switch (player->getClass()) {
			case CLASS_HUNTER:
			case CLASS_MAGE:
			case CLASS_PRIEST:
			case CLASS_WARLOCK:
				point->toRangedPlayers.probe(d);
				break;
			case CLASS_PALADIN:
			case CLASS_ROGUE:
			case CLASS_WARRIOR:
				point->toMeleePlayers.probe(d);
				break;
		}
	}
}

void FleeManager::calculateDistanceToCreatures(FleePoint *point)
{
	RangePair &distance = point->toCreatures;

	list<ObjectGuid> units = *bot->GetPlayerbotAI()->GetAiObjectContext()->GetValue<list<ObjectGuid> >("all targets");
	for (list<ObjectGuid>::iterator i = units.begin(); i != units.end(); ++i)
    {
		Unit* unit = bot->GetPlayerbotAI()->GetUnit(*i);
		if (!unit)
		    continue;

		float d = sServerFacade.GetDistance2d(unit, point->x, point->y);
		distance.probe(d);
	}
}

bool intersectsOri(float angle, list<float>& angles)
{
    for (list<float>::iterator i = angles.begin(); i != angles.end(); ++i)
    {
        float ori = *i;
        if (abs(angle - ori) < M_PI / 12) return true;
    }

    return false;
}

void FleeManager::calculatePossibleDestinations(list<FleePoint*> &points)
{
	float botPosX = bot->GetPositionX();
	float botPosY = bot->GetPositionY();
	float botPosZ = bot->GetPositionZ();

    list<float> meleeOri, enemyOri;
    Group* group = bot->GetGroup();
    if (group)
    {
        for (GroupReference *gref = group->GetFirstMember(); gref; gref = gref->next())
        {
            Player* player = gref->getSource();
            if(player == bot)
                continue;

            float ori = bot->GetAngle(player);
            switch (player->getClass()) {
                case CLASS_PALADIN:
                case CLASS_ROGUE:
                case CLASS_WARRIOR:
                    meleeOri.push_back(ori);
                    break;
            }
        }
    }

    list<ObjectGuid> units = *bot->GetPlayerbotAI()->GetAiObjectContext()->GetValue<list<ObjectGuid> >("all targets");
    for (list<ObjectGuid>::iterator i = units.begin(); i != units.end(); ++i)
    {
        Unit* unit = bot->GetPlayerbotAI()->GetUnit(*i);
        if (!unit)
            continue;

        float ori = bot->GetAngle(unit);
        enemyOri.push_back(ori);
    }

    for (float dist = maxAllowedDistance; dist > sPlayerbotAIConfig.tooCloseDistance; dist -= sPlayerbotAIConfig.followDistance)
    {
        for (float angle = 0.0f; angle <= 2 * M_PI; angle += M_PI / 12)
        {
            if (intersectsOri(angle, enemyOri)) continue;
            if (intersectsOri(angle, meleeOri)) continue;

            float x = botPosX + cos(angle) * maxAllowedDistance, y = botPosY + sin(angle) * maxAllowedDistance, z = botPosZ;
            float dist = sServerFacade.GetDistance2d(bot, x, y);

            if (forceMaxDistance && sServerFacade.IsDistanceLessThan(dist, maxAllowedDistance - sPlayerbotAIConfig.tooCloseDistance))
                continue;

            bot->UpdateAllowedPositionZ(x, y, z);

            Map* map = bot->GetMap();
            const TerrainInfo* terrain = map->GetTerrain();
            if (terrain && terrain->IsInWater(x, y, z))
                continue;

            if (!bot->IsWithinLOS(x, y, z))
                continue;

            FleePoint *point = new FleePoint(bot->GetPlayerbotAI(), x, y, z);
            calculateDistanceToPlayers(point);
            calculateDistanceToCreatures(point);
            points.push_back(point);
        }
    }
}

void FleeManager::cleanup(list<FleePoint*> &points)
{
	for (list<FleePoint*>::iterator i = points.begin(); i != points.end(); i++)
    {
		FleePoint* point = *i;
		delete point;
	}
	points.clear();
}

bool FleePoint::isReasonable()
{
	return (
	            (toCreatures.min <= 0 || toCreatures.max <= 0) ||
	            toCreatures.min >= 0 && toCreatures.max >= 0 && toCreatures.min >= ai->GetRange("flee")
	        )
	        &&
	        (
                (toAllPlayers.min <= 0 || toAllPlayers.max <= 0) ||
                (toAllPlayers.min <= ai->GetRange("spell") && toAllPlayers.max <= ai->GetRange("spell"))
            );
}

FleePoint* FleeManager::selectOptimalDestination(list<FleePoint*> &points)
{
	FleePoint* best = NULL;
	for (list<FleePoint*>::iterator i = points.begin(); i != points.end(); i++)
    {
		FleePoint* point = *i;
		if (!best || (point->toCreatures.min - best->toCreatures.min) >= 0.5f)
		{
            best = point;
		}
        else if ((point->toCreatures.min - best->toCreatures.min) >= 0)
        {
            if (point->toRangedPlayers.max >= 0 && best->toRangedPlayers.max >= 0 &&
                    (point->toRangedPlayers.max - best->toRangedPlayers.max) <= 0.5f)
            {
                best = point;
            }
            else if (point->toMeleePlayers.max >= 0 && best->toMeleePlayers.max >= 0 &&
                    (point->toMeleePlayers.min - best->toMeleePlayers.min) >= 0.5f)
            {
                best = point;
            }
        }
	}

	return best;
}

bool FleeManager::CalculateDestination(float* rx, float* ry, float* rz)
{
	list<FleePoint*> points;
	calculatePossibleDestinations(points);

    FleePoint* point = selectOptimalDestination(points);
    if (!point)
    {
        cleanup(points);
        return false;
    }

	*rx = point->x;
	*ry = point->y;
	*rz = point->z;

    cleanup(points);
	return true;
}
