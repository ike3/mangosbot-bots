#pragma once
#include "MovementActions.h"
#include "GenericActions.h"

namespace ai
{
   class WaitForAttackSetTimeAction : public ChatCommandAction
   {
   public:
       WaitForAttackSetTimeAction(PlayerbotAI* ai, string name = "wait for attack time") : ChatCommandAction(ai, name) {}
      virtual bool Execute(Event& event) override;
   };

   class WaitForAttackKeepSafeDistanceAction : public MovementAction
   {
   public:
       WaitForAttackKeepSafeDistanceAction(PlayerbotAI* ai) : MovementAction(ai, "wait for attack keep safe distance") {}
       virtual bool Execute(Event& event);

   private:
       const WorldPosition GetBestPoint(Unit* target, float minDistance, float maxDistance) const;
       bool IsEnemyClose(const WorldPosition& point, const std::list<ObjectGuid>& enemies) const;
   };
}
