#pragma once
#include "../Value.h"
#include "WorldPosition.h"

namespace ai
{
    struct Hazard
    {
        Hazard();
        Hazard(const WorldPosition& position, uint64 expiration, float radius);
        Hazard(const ObjectGuid& guid, uint64 expiration, float radius);

        bool GetPosition(PlayerbotAI* ai, WorldPosition& outPosition);
        bool IsValid() const;

        WorldPosition position;
        time_t expirationTime;
        ObjectGuid guid;
        float radius;
    };

    // Hazard position, Hazard radius
    typedef std::pair<WorldPosition, float> HazardPosition;

    // DO NOT USE. Value to store hazards which the bot must avoid when moving around
    class StoredHazardsValue : public ManualSetValue<std::list<Hazard>>
	{
	public:
        StoredHazardsValue(PlayerbotAI* ai, string name = "stored hazards") : ManualSetValue<std::list<Hazard>>(ai, {}, name) {}
	};

    // Value to add hazards which the bot must avoid when moving around
    class AddHazardValue : public ManualSetValue<Hazard>
    {
    public:
        AddHazardValue(PlayerbotAI* ai, string name = "add hazard") : ManualSetValue<Hazard>(ai, Hazard(), name) {}

    private:
        void Set(Hazard hazard) override;
    };

    // Value to get hazards position and radius which the bot must avoid when moving around
    class HazardsValue : public CalculatedValue<std::list<HazardPosition>>
    {
    public:
        HazardsValue(PlayerbotAI* ai, string name = "hazards") : CalculatedValue<std::list<HazardPosition>>(ai, name, 1) {}

    private:
        std::list<HazardPosition> Calculate() override;
    };
}
