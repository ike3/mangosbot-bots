#pragma once
#pragma once

#include "../Action.h"
#include "InventoryAction.h"
#include "MovementActions.h"
#include "../../AiFactory.h"
#include "../../PlayerbotAIConfig.h"
#include "../ItemVisitors.h"
#include "../../RandomPlayerbotMgr.h"
#include "BattleGround.h"
#include "BattleGroundMgr.h"
#include "BattleGroundWS.h"
#include "ChooseTargetActions.h"
#include "CheckMountStateAction.h"
#include "G3D/Vector3.h"
#include "GameObject.h"

using namespace ai;

class BGTacticsWS : public MovementAction
{
public:
   BGTacticsWS(PlayerbotAI* ai, string name = "bg tactics ws") : MovementAction(ai, name) {}
   virtual bool Execute(Event event);
private:
   bool moveTowardsEnemyFlag(BattleGroundWS *bg);
   bool consumeHealthy(BattleGround *bg);
   bool useBuff(BattleGround *bg);
   bool homerun(BattleGroundWS *bg);
   bool runPathTo(WorldObject *unit, BattleGround *bg);
   bool runPathTo(uint32 mapId, float x, float y, float z);
   bool wasInCombat = false;

   ObjectGuid AllianceWsgFlagStand(BattleGround *bg);
   ObjectGuid HordeWsgFlagStand(BattleGround *bg);
   //ObjectGuid FindWsHealthy(BattleGround *bg);
};

class ArenaTactics : public MovementAction
{
public:
    ArenaTactics(PlayerbotAI* ai, string name = "arena tactics") : MovementAction(ai, name) {}
    virtual bool Execute(Event event);
private:
    bool moveToCenter(BattleGround *bg);
};
