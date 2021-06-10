#pragma once
#include "../Value.h"
#include "TravelNode.h"

namespace ai
{
    class LastMovement
    {
    public:
        LastMovement()
        {
            clear();
        }

        LastMovement(LastMovement& other)
        {
            taxiNodes = other.taxiNodes;
            taxiMaster = other.taxiMaster;
            lastFollow = other.lastFollow;
            lastAreaTrigger = other.lastAreaTrigger;
            lastMoveShort = other.lastMoveShort;
            lastPath = other.lastPath;
            nextTeleport = other.nextTeleport;
            /*
            lastMoveToMapId = other.lastMoveToMapId;
            lastMoveToX = other.lastMoveToX;
            lastMoveToY = other.lastMoveToY;
            lastMoveToZ = other.lastMoveToZ;
            lastMoveToOri = other.lastMoveToOri;
            */
        }

        void clear()
        {
            lastMoveShort = WorldPosition();
            lastPath.clear();
            /*
            lastMoveToMapId = 0;
            lastMoveToX = 0;
            lastMoveToY = 0;
            lastMoveToZ = 0;
            lastMoveToOri = 0;
            */
            lastFollow = NULL;
            lastAreaTrigger = 0;
            lastFlee = 0;
            nextTeleport = 0;
        }

        void Set(Unit* lastFollow)
        {
            //Set(0, 0.0f, 0.0f, 0.0f, 0.0f);
            setShort(WorldPosition());
            this->lastFollow = lastFollow;
        }

        /*
        void Set(uint32 mapId, float x, float y, float z, float ori)
        {
            lastMoveToMapId = mapId;
            lastMoveToX = x;
            lastMoveToY = y;
            lastMoveToZ = z;
            lastMoveToOri = ori;
            lastMoveShort = WorldPosition(mapId, x, y, z, ori);
            lastFollow = NULL;
        }
        */

        void setShort(WorldPosition point) {lastMoveShort = point; lastFollow = NULL;
        }
        void setPath(TravelPath path) { lastPath = path; }
    public:
        vector<uint32> taxiNodes;
        ObjectGuid taxiMaster;
        Unit* lastFollow;
        uint32 lastAreaTrigger;
        time_t lastFlee;
        //uint32 lastMoveToMapId;
        //float lastMoveToX, lastMoveToY, lastMoveToZ, lastMoveToOri;
        WorldPosition lastMoveShort;
        TravelPath lastPath;
        time_t nextTeleport;
    };

    class LastMovementValue : public ManualSetValue<LastMovement&>
	{
	public:
        LastMovementValue(PlayerbotAI* ai) : ManualSetValue<LastMovement&>(ai, data) {}

    private:
        LastMovement data;
    };

    class StayTimeValue : public ManualSetValue<time_t>
	{
	public:
        StayTimeValue(PlayerbotAI* ai) : ManualSetValue<time_t>(ai, 0) {}
    };
}
