#pragma once
#include "../Trigger.h"

namespace ai
{
    class EnterDungeonTrigger : public Trigger
    {
    public:
        // You can get the mapID from worlddb > instance_template > map column
        // or from here https://wow.tools/dbc/?dbc=map&build=1.12.1.5875
        EnterDungeonTrigger(PlayerbotAI* ai, string name, string dungeonStrategy, uint32 mapID)
        : Trigger(ai, name, 5)
        , dungeonStrategy(dungeonStrategy)
        , mapID(mapID) {}

        bool IsActive() override;

    private:
        string dungeonStrategy;
        uint32 mapID;
    };

    class LeaveDungeonTrigger : public Trigger
    {
    public:
        // You can get the mapID from worlddb > instance_template > map column
        // or from here https://wow.tools/dbc/?dbc=map&build=1.12.1.5875
        LeaveDungeonTrigger(PlayerbotAI* ai, string name, string dungeonStrategy, uint32 mapID)
        : Trigger(ai, name, 5)
        , dungeonStrategy(dungeonStrategy)
        , mapID(mapID) {}

        bool IsActive() override;

    private:
        string dungeonStrategy;
        uint32 mapID;
    };

    class StartBossFightTrigger : public Trigger
    {
    public:
        StartBossFightTrigger(PlayerbotAI* ai, string name, string bossStrategy, uint64 bossID)
        : Trigger(ai, name, 1)
        , bossStrategy(bossStrategy)
        , bossID(bossID) {}

        bool IsActive() override;

    private:
        string bossStrategy;
        uint64 bossID;
    };

    class EndBossFightTrigger : public Trigger
    {
    public:
        EndBossFightTrigger(PlayerbotAI* ai, string name, string bossStrategy, uint64 bossID)
        : Trigger(ai, name, 5)
        , bossStrategy(bossStrategy)
        , bossID(bossID) {}

        bool IsActive() override;

    private:
        string bossStrategy;
        uint64 bossID;
    };

    class CloseToHazard : public Trigger
    {
    public:
        CloseToHazard(PlayerbotAI* ai, string name, int checkInterval, float radius, time_t expirationTime)
        : Trigger(ai, name, checkInterval)
        , radius(radius)
        , expirationTime(expirationTime) {}

        bool IsActive() override = 0;

    protected:
        float radius;
        time_t expirationTime;
    };

    class CloseToGameObjectHazard : public CloseToHazard
    {
    public:
        CloseToGameObjectHazard(PlayerbotAI* ai, string name, uint32 gameObjectID, float radius, time_t expirationTime)
        : CloseToHazard(ai, name, 1, radius, expirationTime)
        , gameObjectID(gameObjectID) {}

        bool IsActive() override;

    private:
        uint32 gameObjectID;
    };

    class CloseToCreature : public Trigger
    {
    public:
        CloseToCreature(PlayerbotAI* ai, string name, uint32 creatureID, float range)
        : Trigger(ai, name, 1)
        , creatureID(creatureID)
        , range(range) {}

        bool IsActive() override;

    private:
        uint32 creatureID;
        float range;
    };
}