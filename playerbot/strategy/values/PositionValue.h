#pragma once
#include "../Value.h"

namespace ai
{
    class PositionEntry
    {
    public:
        PositionEntry() : valueSet(false), x(0), y(0), z(0), mapId(0) {}
        PositionEntry(const PositionEntry &other) : valueSet(other.valueSet), x(other.x), y(other.y), z(other.z), mapId(other.mapId) {}
        void Set(float x, float y, float z, uint32 mapId) { this->x = x; this->y = y; this->z = z; this->mapId = mapId; this->valueSet = true; }
        void Reset() { valueSet = false; }
        bool isSet() { return valueSet; }

        float x, y, z;
        bool valueSet;
        uint32 mapId;
    };

    typedef map<string, PositionEntry> PositionMap;

    class PositionValue : public ManualSetValue<PositionMap&>
	{
	public:
        PositionValue(PlayerbotAI* ai, string name = "position");

        virtual string Save();
        virtual bool Load(string value);

	private:
        PositionMap positions;
    };
    
    class CurrentPositionValue : public LogCalculatedValue<WorldPosition>
    {
    public:
        CurrentPositionValue(PlayerbotAI* ai, string name = "current position", uint32 checkInterval = 1) : LogCalculatedValue<WorldPosition>(ai, name, checkInterval) { minChangeInterval = 60;  logLength = 30; };
        virtual bool EqualToLast(WorldPosition value) { return value.fDist(lastValue) < sPlayerbotAIConfig.tooCloseDistance; }

        virtual WorldPosition Calculate() {return WorldPosition(bot);};
    };  

    class CustomPositionValue : public ManualSetValue<WorldPosition>, public Qualified
    {
    public:
        CustomPositionValue(PlayerbotAI* ai, string name = "custom position") : ManualSetValue<WorldPosition>(ai, WorldPosition(), name) { };

        virtual WorldPosition Calculate() { return WorldPosition(bot); };
    };
}
