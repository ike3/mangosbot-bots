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
    virtual bool isUseful();
    virtual bool canJoinBg(BattleGroundQueueTypeId queueTypeId, BattleGroundBracketId bracketId);
    virtual bool shouldJoinBg(BattleGroundQueueTypeId queueTypeId, BattleGroundBracketId bracketId);
#ifndef MANGOSBOT_ZERO
    virtual bool gatherArenaTeam(ArenaType type);
#endif
protected:
    bool JoinQueue(uint32 type);
    vector<uint32> bgList;
    vector<uint32> ratedList;
};

class FreeBGJoinAction : public BGJoinAction
{
public:
    FreeBGJoinAction(PlayerbotAI* ai, string name = "free bg join") : BGJoinAction(ai, name) {}
    virtual bool shouldJoinBg(BattleGroundQueueTypeId queueTypeId, BattleGroundBracketId bracketId);
};

class BGLeaveAction : public Action
{
public:
    BGLeaveAction(PlayerbotAI* ai, string name = "bg leave") : Action(ai) {}
    virtual bool Execute(Event event);
};

class BGStatusAction : public Action
{
public:
    BGStatusAction(PlayerbotAI* ai) : Action(ai, "bg status") {}
    virtual bool Execute(Event event);
    virtual bool isUseful();
};

class BGStatusCheckAction : public Action
{
public:
    BGStatusCheckAction(PlayerbotAI* ai, string name = "bg status check") : Action(ai, name) {}
    virtual bool Execute(Event event);
    virtual bool isUseful();
};