#pragma once
#include "MovementActions.h"

namespace ai
{
    class MoveAwayFromGameObject : public MovementAction
    {
    public:
        MoveAwayFromGameObject(PlayerbotAI* ai, string name, uint32 gameObjectID, float range, float targetRange = 0.0f) : MovementAction(ai, name), gameObjectID(gameObjectID), range(range) {}
        bool Execute(Event& event) override;
        bool isPossible() override;

    private:
        bool HasGameObjectsNearby(const WorldPosition& point, const list<GameObject*>& gameObjects) const;

    private:
        uint32 gameObjectID;
        float range;
    };

    class MoveAwayFromCreature : public MovementAction
    {
    public:
        MoveAwayFromCreature(PlayerbotAI* ai, string name, uint32 creatureID, float range) : MovementAction(ai, name), creatureID(creatureID), range(range) {}
        bool Execute(Event& event) override;
        bool isPossible() override;

    private:
        bool HasCreaturesNearby(const WorldPosition& point, const list<Creature*>& creatures) const;

    private:
        uint32 creatureID;
        float range;
    };
}