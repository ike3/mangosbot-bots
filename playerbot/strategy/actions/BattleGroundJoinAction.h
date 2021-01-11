#pragma once
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

/*class BGJoinAction : public Action
{
public:
    BGJoinAction(PlayerbotAI* ai, string name) : Action(ai, name) {}
    //bool BGjoin(uint32 type);
    //virtual bool Execute(Event event);
    //virtual bool BGjoin();

protected:
    bool BGjoin(uint32 type);
};

  class WsGJoinAction : public BGJoinAction
  {
  public:
    WsGJoinAction(PlayerbotAI* ai, string name = "wsg join") : BGJoinAction(ai, name) {}
     virtual bool Execute(Event event);

  //protected:
    //bool BGjoin(uint32 type);
  };*/

class BGJoinAction : public InventoryAction
{
public:
    BGJoinAction(PlayerbotAI* ai, string name = "bg join") : InventoryAction(ai, name) {}
    virtual bool Execute(Event event);

protected:
    bool JoinProposal(uint32 type);
};

  class BGStatusAction : public BGJoinAction
  {
  public:
      BGStatusAction(PlayerbotAI* ai) : BGJoinAction(ai, "bg status") {}
      virtual bool Execute(Event event);
  };
