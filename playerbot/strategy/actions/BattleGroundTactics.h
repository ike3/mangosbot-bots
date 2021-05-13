#pragma once

#include "MovementActions.h"
#include "BattleGround.h"
#include "BattleGroundMgr.h"
#include "BattleGroundWS.h"
#include "BattleGroundAV.h"
#include "BattleGroundAB.h"
#include "CheckMountStateAction.h"

using namespace ai;

#define SPELL_CAPTURE_BANNER 21651

typedef void(*BattleBotWaypointFunc) ();

// from vmangos
struct BattleBotWaypoint
{
    BattleBotWaypoint(float x_, float y_, float z_, BattleBotWaypointFunc func) :
        x(x_), y(y_), z(z_), pFunc(func) {};
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    BattleBotWaypointFunc pFunc = nullptr;
};

typedef std::vector<BattleBotWaypoint> BattleBotPath;

extern std::vector<BattleBotPath*> const vPaths_WS;
extern std::vector<BattleBotPath*> const vPaths_AB;
extern std::vector<BattleBotPath*> const vPaths_AV;

class BGTactics : public MovementAction
{
public:
    BGTactics(PlayerbotAI* ai, string name = "bg tactics") : MovementAction(ai, name) {}
    virtual bool Execute(Event event);
    void AtCaveExit();
private:
    bool moveToStart();
    bool selectObjective(bool reset = false);
    bool moveToObjective();
    bool selectObjectiveWp(std::vector<BattleBotPath*> const& vPaths);
    bool moveToObjectiveWp(BattleBotPath* const& currentPath, uint32 currentPoint, bool reverse = false);
    bool startNewPathBegin(std::vector<BattleBotPath*> const& vPaths);
    bool startNewPathFree(std::vector<BattleBotPath*> const& vPaths);
    bool resetObjective();
    bool atFlag(std::vector<BattleBotPath*> const& vPaths, std::vector<uint32> const& vFlagIds);
    bool flagTaken();
    bool teamFlagTaken();
    bool protectFC();
    bool attackFC();
    bool nearFlag();
    bool atHordeFlag();
    bool useBuff();
    bool useHealthy();
    // Movement System
    void UpdateWaypointMovement();
    void DoGraveyardJump();
    void MoveToNextPoint();
    void MoveToNextPointSpecial();
    bool StartNewPathFromBeginning();
    void StartNewPathFromAnywhere();
    //bool StartNewPathToPosition(Position const& position, std::vector<BattleBotPath*> const& vPaths);
    void ClearPath();
    void StopMoving();
    void AV_AtFlag();
    void AtFlag(std::vector<uint32> const& vFlagIds);
};

class ArenaTactics : public MovementAction
{
public:
    ArenaTactics(PlayerbotAI* ai, string name = "arena tactics") : MovementAction(ai, name) {}
    virtual bool Execute(Event event);
private:
    bool moveToCenter(BattleGround *bg);
};
