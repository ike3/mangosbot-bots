#include "../botpch.h"
#include "playerbot.h"
#include "FleeManager.h"
#include "PlayerbotAIConfig.h"
#include "Group.h"
#include "ServerFacade.h"

using namespace ai;
using namespace std;

void FleeManager::calculateDistanceToCreatures(FleePoint *point)
{
    point->minDistance = -1.0f;
    point->sumDistance = 0.0f;
    list<ObjectGuid> units = *bot->GetPlayerbotAI()->GetAiObjectContext()->GetValue<list<ObjectGuid> >("possible targets no los");
	for (list<ObjectGuid>::iterator i = units.begin(); i != units.end(); ++i)
    {
		Unit* unit = bot->GetPlayerbotAI()->GetUnit(*i);
		if (!unit)
		    continue;

		float d = sServerFacade.GetDistance2d(unit, point->x, point->y);
        point->sumDistance += d;
        if (point->minDistance < 0 || point->minDistance > d) point->minDistance = d;
	}
}

bool intersectsOri(float angle, list<float>& angles, float angleIncrement)
{
    for (list<float>::iterator i = angles.begin(); i != angles.end(); ++i)
    {
        float ori = *i;
        if (abs(angle - ori) < angleIncrement) return true;
    }

    return false;
}

void FleeManager::calculatePossibleDestinations(list<FleePoint*> &points)
{
    Unit *target = *bot->GetPlayerbotAI()->GetAiObjectContext()->GetValue<Unit*>("current target");

    float botPosX = startPosition.getX();
    float botPosY = startPosition.getY();
    float botPosZ = startPosition.getZ();
    
    FleePoint start(bot->GetPlayerbotAI(), botPosX, botPosY, botPosZ);
    calculateDistanceToCreatures(&start);

    list<float> enemyOri;
    list<ObjectGuid> units = *bot->GetPlayerbotAI()->GetAiObjectContext()->GetValue<list<ObjectGuid> >("possible targets no los");
    for (list<ObjectGuid>::iterator i = units.begin(); i != units.end(); ++i)
    {
        Unit* unit = bot->GetPlayerbotAI()->GetUnit(*i);
        if (!unit)
            continue;

        float ori = bot->GetAngle(unit);
        enemyOri.push_back(ori);
    }

    float distIncrement = max(sPlayerbotAIConfig.followDistance, (maxAllowedDistance - sPlayerbotAIConfig.tooCloseDistance) / 10.0f);
    for (float dist = maxAllowedDistance; dist >= sPlayerbotAIConfig.tooCloseDistance; dist -= distIncrement)
    {
        float angleIncrement = max(M_PI / 20, M_PI / 4 / (1.0 + dist - sPlayerbotAIConfig.tooCloseDistance));
        for (float add = 0.0f; add < M_PI / 4 + angleIncrement; add += angleIncrement)
        {
            for (float angle = add; angle < add + 2 * M_PI_F + angleIncrement; angle += M_PI_F / 4)
            {
                if (intersectsOri(angle, enemyOri, angleIncrement)) continue;

                float x = botPosX + cos(angle) * maxAllowedDistance, y = botPosY + sin(angle) * maxAllowedDistance, z = botPosZ + CONTACT_DISTANCE;

                if (forceMaxDistance && sServerFacade.IsDistanceLessThan(sServerFacade.GetDistance2d(bot, x, y), maxAllowedDistance - sPlayerbotAIConfig.tooCloseDistance))
                    continue;

                bot->UpdateAllowedPositionZ(x, y, z);

                Map* map = startPosition.getMap();
                const TerrainInfo* terrain = map->GetTerrain();
                if (terrain && terrain->IsInWater(x, y, z))
                    continue;

                if (!bot->IsWithinLOS(x, y, z) || (target && !target->IsWithinLOS(x, y, z)))
                    continue;

                FleePoint *point = new FleePoint(bot->GetPlayerbotAI(), x, y, z);
                calculateDistanceToCreatures(point);

                if (sServerFacade.IsDistanceGreaterOrEqualThan(point->minDistance - start.minDistance, sPlayerbotAIConfig.followDistance))
                    points.push_back(point);
                else
                    delete point;
            }
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

bool FleeManager::isBetterThan(FleePoint* point, FleePoint* other)
{
    return point->sumDistance - other->sumDistance > 0;
}

FleePoint* FleeManager::selectOptimalDestination(list<FleePoint*> &points)
{
	FleePoint* best = NULL;
	for (list<FleePoint*>::iterator i = points.begin(); i != points.end(); i++)
    {
		FleePoint* point = *i;
        if (!best || isBetterThan(point, best))
            best = point;
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

bool FleeManager::isUseful()
{
    list<ObjectGuid> units = *bot->GetPlayerbotAI()->GetAiObjectContext()->GetValue<list<ObjectGuid> >("possible targets no los");
    for (list<ObjectGuid>::iterator i = units.begin(); i != units.end(); ++i)
    {
        Unit* unit = bot->GetPlayerbotAI()->GetUnit(*i);
        if (!unit)
            continue;

        if (startPosition.sqDistance(WorldPosition(unit)) < unit->GetAttackDistance(bot) * unit->GetAttackDistance(bot))
            return true;

        //float d = sServerFacade.GetDistance2d(unit, bot);
        //if (sServerFacade.IsDistanceLessThan(d, sPlayerbotAIConfig.aggroDistance)) return true;
    }
    return false;
}
