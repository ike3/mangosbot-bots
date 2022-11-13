#include "botpch.h"
#include "../../playerbot.h"
#include "NearestGameObjects.h"

#include "../../ServerFacade.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"

using namespace ai;
using namespace MaNGOS;

class AnyGameObjectInObjectRangeCheck
{
public:
    AnyGameObjectInObjectRangeCheck(WorldObject const* obj, float range) : i_obj(obj), i_range(range) {}
    WorldObject const& GetFocusObject() const { return *i_obj; }
    bool operator()(GameObject* u)
    {
        if (u && i_obj->IsWithinDistInMap(u, i_range) && sServerFacade.isSpawned(u) && u->GetGOInfo())
            return true;

        return false;
    }

private:
    WorldObject const* i_obj;
    float i_range;
};

class AnyDynamicObjectInObjectRangeCheck
{
public:
    AnyDynamicObjectInObjectRangeCheck(WorldObject const* obj, float range) : i_obj(obj), i_range(range) {}
    WorldObject const& GetFocusObject() const { return *i_obj; }
    bool operator()(DynamicObject* u)
    {
        if (u && i_obj->IsWithinDistInMap(u, i_range))
            return true;

        return false;
    }

private:
    WorldObject const* i_obj;
    float i_range;
};

list<ObjectGuid> NearestGameObjects::Calculate()
{
    list<GameObject*> targets;

    AnyGameObjectInObjectRangeCheck u_check(bot, range);
    GameObjectListSearcher<AnyGameObjectInObjectRangeCheck> searcher(targets, u_check);
    Cell::VisitAllObjects((const WorldObject*)bot, searcher, range);

    list<ObjectGuid> result;
    for(list<GameObject*>::iterator tIter = targets.begin(); tIter != targets.end(); ++tIter)
    {
		GameObject* go = *tIter;
        if(ignoreLos || sServerFacade.IsWithinLOSInMap(bot, go))
			result.push_back(go->GetObjectGuid());
    }

    return result;
}

list<ObjectGuid> NearestDynamicObjects::Calculate()
{
    list<DynamicObject*> targets;
#ifdef MANGOSBOT_ZERO
    AnyDynamicObjectInObjectRangeCheck u_check(bot, range);
    MaNGOS::DynamicObjectListSearcher<AnyDynamicObjectInObjectRangeCheck> searcher(targets, u_check);
    Cell::VisitAllObjects((const WorldObject*)bot, searcher, range);
#endif

    list<ObjectGuid> result;
    for (list<DynamicObject*>::iterator tIter = targets.begin(); tIter != targets.end(); ++tIter)
    {
        DynamicObject* go = *tIter;
        if (ignoreLos || sServerFacade.IsWithinLOSInMap(bot, go))
            result.push_back(go->GetObjectGuid());
    }

    return result;
}
