#pragma once
#include "MovementActions.h"

namespace ai
{
    class MoveAwayFromGameObject : public MovementAction
    {
    public:
        MoveAwayFromGameObject(PlayerbotAI* ai, string name, uint32 gameObjectID, float range) : MovementAction(ai, name), gameObjectID(gameObjectID), range(range) {}
        bool Execute(Event& event) override;

    private:
        bool HasGameObjectNearby(const WorldPosition& point, const list<GameObject*>& gameObjects) const;

    private:
        uint32 gameObjectID;
        float range;
    };
}