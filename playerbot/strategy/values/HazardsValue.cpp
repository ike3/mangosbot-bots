#include "botpch.h"
#include "../../playerbot.h"
#include "HazardsValue.h"
#include "../AiObjectContext.h"

using namespace ai;

Hazard::Hazard()
{
    expirationTime = 0;
    guid = ObjectGuid();
    position = WorldPosition();
    radius = 0;
}

Hazard::Hazard(const WorldPosition& inPosition, uint64 inExpiration, float inRadius)
{
    expirationTime = time(0) + inExpiration;
    guid = ObjectGuid();
    position = WorldPosition(inPosition);
    radius = inRadius;
}

Hazard::Hazard(const ObjectGuid& inGuid, uint64 inExpiration, float inRadius)
{
    expirationTime = time(0) + inExpiration;
    guid = inGuid;
    position = WorldPosition();
    radius = inRadius;
}

bool Hazard::GetPosition(PlayerbotAI* ai, WorldPosition& outPosition)
{
    if (!guid.IsEmpty())
    {
        WorldObject* hazardObject = nullptr;
        if (guid.IsUnit())
        {
            hazardObject = ai->GetUnit(guid);
        }
        else if (guid.IsGameObject())
        {
            hazardObject = ai->GetGameObject(guid);
        }

        if (hazardObject)
        {
            outPosition = WorldPosition(hazardObject);
            return true;
        }
    }
    else if(position.isValid())
    {
        outPosition = position;
        return true;
    }

    return false;
}

bool Hazard::IsValid() const
{
    return (expirationTime > 0) && (!guid.IsEmpty() || position.isValid());
}

void AddHazardValue::Set(Hazard hazard)
{
    if (hazard.IsValid())
    {
        AiObjectContext* context = ai->GetAiObjectContext();
        std::list<Hazard>& storedHazards = AI_VALUE(std::list<Hazard>, "stored hazards");

        // Check if the hazard already exists (only for world object hazards)
        auto it = std::find_if(storedHazards.begin(), storedHazards.end(), [&hazard](const Hazard& existingHazard)
        {
            return !existingHazard.guid.IsEmpty() && (existingHazard.guid == hazard.guid);
        });

        // Add the new hazard to the value
        if (it == storedHazards.end())
        {
            storedHazards.emplace_back(std::move(hazard));
            SET_AI_VALUE(std::list<Hazard>, "stored hazards", storedHazards);
        }
    }
}

std::list<HazardPosition> HazardsValue::Calculate()
{
    AiObjectContext* context = ai->GetAiObjectContext();
    std::list<Hazard>& storedHazards = AI_VALUE(std::list<Hazard>, "stored hazards");

    // Create an updated hazard positions list
    time_t currTime = time(0);
    bool hazardsUpdated = false;
    std::list<HazardPosition> hazards;
    for (auto it = storedHazards.begin(); it != storedHazards.end();)
    {
        Hazard& hazard = (*it);
        bool validHazard = false;

        // Check if the hazard has not expired
        if (hazard.expirationTime > currTime)
        {
            // Try to retrieve the hazard position
            WorldPosition hazardPosition;
            if (hazard.GetPosition(ai, hazardPosition))
            {
                hazards.emplace_back(std::make_pair(hazardPosition, hazard.radius));
                validHazard = true;
            }
        }

        // Remove the invalid hazards
        if (!validHazard)
        {
            it = storedHazards.erase(it);
            hazardsUpdated = true;
        }
        else
        {
            ++it;
        }
    }

    if (hazardsUpdated)
    {
        SET_AI_VALUE(std::list<Hazard>, "stored hazards", storedHazards);
    }

    return hazards;
}
