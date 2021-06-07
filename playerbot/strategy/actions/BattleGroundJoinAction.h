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
#include "ChooseMoveDoAction.h"

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

//Picks a random BG and selects all BM's of that bg type.
//Selects the closest BM as potential move target.
class QueueAtBmAction : public ChooseMoveDoListAction<CreatureDataPair const*> {
public:
    bool canJoinBgQueue(BattleGroundQueueTypeId queueTypeId); //Check to see if bot can join this bgqueue

    void GetRandomBgQueue();                        //Select random bg queue from bgs bot may join
    BattleGroundQueueTypeId getBgQueue() { return (BattleGroundQueueTypeId)stoi(getQualifier()); }

    QueueAtBmAction(PlayerbotAI* ai, string name = "queue at bm", string targetValueName = "bg masters") : ChooseMoveDoListAction(ai, name, targetValueName) { GetRandomBgQueue(); }

    virtual bool isUseful();                        //Check if bot may join selected bg.

    virtual bool getPotentialTargets();

    virtual bool IsValidBm(CreatureDataPair const* bmPair, bool allowDead); //Check if selected BM is friendly/exists/alive
    virtual bool FilterPotentialTargets();          //Filter only those BM's that are valid.

    virtual bool ExecuteAction(Event event);        //Face BM and set BG strategy and BG type
};
