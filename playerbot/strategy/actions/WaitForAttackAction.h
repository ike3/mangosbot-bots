#pragma once

#include "MovementActions.h"

namespace ai
{
   class WaitForAttackSetTimeAction : public Action
   {
   public:
       WaitForAttackSetTimeAction(PlayerbotAI* ai, string name = "wait for attack time") : Action(ai, name) {}

   public:
      virtual bool Execute(Event& event);
   };

   class WaitForAttackKeepSafeDistanceAction : public MovementAction
   {
   public:
       WaitForAttackKeepSafeDistanceAction(PlayerbotAI* ai) : MovementAction(ai, "wait for attack keep safe distance") {}
       virtual bool Execute(Event& event);

   private:
       void GeneratePointsAroundTarget(Unit* target, float angleIncrement, float radius, std::vector<WorldPosition>& outPoints);
       const WorldPosition* GetBestPoint(Unit* target, float safeDistance, const std::vector<WorldPosition>& points) const;
       bool IsEnemyClose(const WorldPosition& point, const std::list<ObjectGuid>& enemies) const;
   };
}
