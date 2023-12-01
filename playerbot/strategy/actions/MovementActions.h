#pragma once

#include "../Action.h"
#include "../NamedObjectContext.h"
#include "../../PlayerbotAIConfig.h"
#include "../../TravelNode.h"
#include "WaypointMovementGenerator.h"
#include "../values/HazardsValue.h"

namespace ai
{
    class MovementAction : public Action
    {
    public:
        MovementAction(PlayerbotAI* ai, string name) : Action(ai, name) {}

    protected:
        bool ChaseTo(WorldObject *obj, float distance = 0.0f, float angle = 0.0f);
        bool MoveNear(uint32 mapId, float x, float y, float z, float distance = sPlayerbotAIConfig.contactDistance);
        bool FlyDirect(WorldPosition &startPosition,  WorldPosition &endPosition , WorldPosition& movePosition, TravelPath movePath, bool idle);
        bool MoveTo(uint32 mapId, float x, float y, float z, bool idle = false, bool react = false, bool noPath = false, bool ignoreEnemyTargets = false);
        bool MoveTo(Unit* target, float distance = 0.0f);
        bool MoveNear(WorldObject* target, float distance = sPlayerbotAIConfig.contactDistance);
        bool MoveToLOS(WorldObject* target, bool ranged = false);
        float GetFollowAngle();
        bool Follow(Unit* target, float distance = 0);
        bool Follow(Unit* target, float distance, float angle);
        float MoveDelay(float distance);
        bool FollowOnTransport(Unit* target);

        void WaitForReach(float distance);
        void WaitForReach(const Movement::PointsArray& path);

        bool IsMovingAllowed(Unit* target);
        bool IsMovingAllowed(uint32 mapId, float x, float y, float z);
        bool IsMovingAllowed();
        bool Flee(Unit *target);
        void ClearIdleState();
        void UpdateMovementState();

        virtual bool isPossible() override;

        void CreateWp(Player* wpOwner, float x, float y, float z, float o, uint32 entry, bool important = false);
        float GetAngle(const float x1, const float y1, const float x2, const float y2);

        // Used when this action is executed as a reaction
        bool ShouldReactionInterruptCast() const override { return true; }
        bool ShouldReactionInterruptMovement() const override { return true; }

    private:
        bool IsValidPosition(const WorldPosition& position, const WorldPosition& visibleFromPosition);
        bool IsHazardNearPosition(const WorldPosition& position, HazardPosition* outHazard = nullptr);
        bool GeneratePathAvoidingHazards(const WorldPosition& endPosition, bool generatePath, Movement::PointsArray& outPath);
    };

    class FleeAction : public MovementAction
    {
    public:
        FleeAction(PlayerbotAI* ai, float distance = sPlayerbotAIConfig.spellDistance) : MovementAction(ai, "flee"), distance(distance) {}
        virtual bool Execute(Event& event);
        virtual bool isUseful();

    private:
        float distance;
    };

    class FleeWithPetAction : public MovementAction
    {
    public:
        FleeWithPetAction(PlayerbotAI* ai) : MovementAction(ai, "flee with pet") {}
        virtual bool Execute(Event& event);
    };

    class RunAwayAction : public MovementAction
    {
    public:
        RunAwayAction(PlayerbotAI* ai) : MovementAction(ai, "runaway") {}
        virtual bool Execute(Event& event);
    };

    class MoveToLootAction : public MovementAction
    {
    public:
        MoveToLootAction(PlayerbotAI* ai) : MovementAction(ai, "move to loot") {}
        virtual bool Execute(Event& event);
    };

    class MoveOutOfEnemyContactAction : public MovementAction
    {
    public:
        MoveOutOfEnemyContactAction(PlayerbotAI* ai) : MovementAction(ai, "move out of enemy contact") {}
        virtual bool Execute(Event& event);
        virtual bool isUseful();
    };

    class SetFacingTargetAction : public Action
    {
    public:
        SetFacingTargetAction(PlayerbotAI* ai) : Action(ai, "set facing") {}
        virtual bool Execute(Event& event);
        virtual bool isUseful();
        virtual bool isPossible();
    };

    class SetBehindTargetAction : public MovementAction
    {
    public:
        SetBehindTargetAction(PlayerbotAI* ai) : MovementAction(ai, "set behind") {}
        virtual bool Execute(Event& event);
        virtual bool isUseful();
        virtual bool isPossible();
    };

    class MoveOutOfCollisionAction : public MovementAction
    {
    public:
        MoveOutOfCollisionAction(PlayerbotAI* ai) : MovementAction(ai, "move out of collision") {}
        virtual bool Execute(Event& event);
        virtual bool isUseful();
    };

    class MoveRandomAction : public MovementAction
    {
    public:
        MoveRandomAction(PlayerbotAI* ai) : MovementAction(ai, "move random") {}
        virtual bool Execute(Event& event);
        virtual bool isUseful();
    };

    class MoveToAction : public MovementAction, public Qualified
    {
    public:
        MoveToAction(PlayerbotAI* ai, string name = "move to") : MovementAction(ai, "name"), Qualified() {}
        virtual bool Execute(Event& event);
    };
}
