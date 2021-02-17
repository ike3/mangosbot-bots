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


class BGJoinAction : public Action
{
public:
    BGJoinAction(PlayerbotAI* ai, string name = "bg join") : Action(ai, name) {}
    virtual bool Execute(Event event);

protected:
    bool JoinQueue(uint32 type);
};

class BGLeaveAction : public Action
{
public:
    BGLeaveAction(PlayerbotAI* ai, string name = "bg leave") : Action(ai) {}
    virtual bool Execute(Event event);
};

class BGStatusAction : public BGJoinAction
{
public:
    BGStatusAction(PlayerbotAI* ai) : BGJoinAction(ai, "bg status") {}
    virtual bool Execute(Event event);
};

class BGStatusCheckAction : public Action
{
public:
    BGStatusCheckAction(PlayerbotAI* ai, string name = "bg status check") : Action(ai, name) {}
    virtual bool Execute(Event event);
};
