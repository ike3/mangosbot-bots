#include "ObjectGuid.h"
#include "botpch.h"
#include "../../playerbot.h"
#include "../../playerbotAI.h"
#include "LfgActions.h"
#include "../../AiFactory.h"
//#include "../../PlayerbotAIConfig.h"
//#include "../ItemVisitors.h"
#include "../../RandomPlayerbotMgr.h"
//#include "../../../../game/LFGMgr.h"
#include "strategy/values/PositionValue.h"
//#include "ServerFacade.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "Object.h"
#include "Player.h"
#include "ObjectMgr.h"
#include "strategy/values/LastMovementValue.h"
#include "strategy/actions/LogLevelAction.h"
#include "strategy/values/LastSpellCastValue.h"
#include "MovementActions.h"
#include "MotionMaster.h"
#include "MovementGenerator.h"
#include "../values/PositionValue.h"
#include "MotionGenerators/TargetedMovementGenerator.h"
#include "BattleGround.h"
#include "BattleGroundMgr.h"
#include "BattlegroundTacticsWS.h"
#include "BattleGroundWS.h"

using namespace ai;

#ifdef MANGOS
enum GameObjectsWS
{
    GO_WS_SILVERWING_FLAG = 179830,
    GO_WS_WARSONG_FLAG = 179831
};
#endif

enum AreaTriggersWS
{
    AT_SILVERWING_FLAG = 3646,
    AT_WARSONG_FLAG = 3647
};

//WS_FLAG_POS_HORDE = { 915.958f, 1433.925f, 346.193f, 0.0f };
//WS_FLAG_POS_ALLIANCE = { 1539.219f, 1481.747f, 352.458f, 0.0f };

ObjectGuid BGTacticsWS::AllianceWsgFlagStand(BattleGround * bg)
{
    ObjectGuid wsgflagA;
    uint32 lowguid = 90000;
    uint32 id = 179830;
    GameObject* AllianceflagStand = bg->GetBgMap()->GetGameObject(ObjectGuid(HIGHGUID_GAMEOBJECT, id, lowguid));

    if (AllianceflagStand != nullptr)
    {
        wsgflagA = AllianceflagStand->GetObjectGuid();
        return wsgflagA;
    }
}
ObjectGuid BGTacticsWS::HordeWsgFlagStand(BattleGround * bg)
{
    ObjectGuid wsgflagH;
    uint32 lowguid = 90001;
    uint32 id = 179831;
    GameObject* HordeflagStand = bg->GetBgMap()->GetGameObject(ObjectGuid(HIGHGUID_GAMEOBJECT, id, lowguid));

    if (HordeflagStand != nullptr)
    {
        wsgflagH = HordeflagStand->GetObjectGuid();
        return wsgflagH;
    }
}

ObjectGuid BGTacticsWS::FindWsHealthy(BattleGround * bg)
{
    list<ObjectGuid> bg_gos = *ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("bg game objects");
    ObjectGuid WsHealthy;

}

//consume healthy, if low on health
bool BGTacticsWS::consumeHealthy(BattleGround *bg)
{

    //alliance healthy
    if (bot->GetHealthPercent() < 50.0f)
    {
        //Alliance healthy
        float ax = 1111.526733f;
        float ay = 1352.458130f;
        float az = 316.567047f;
        //Horde healthy
        float hx = 1320.670288f;
        float hy = 1549.642700f;
        float hz = 313.501404f;

        //distance from Alliance healthy
        float distanceA = bot->GetDistance(ax, ay, az);
        if (distanceA < sPlayerbotAIConfig.sightDistance)
        {
            return MoveTo(bg->GetMapId(), ax, ay, az);
        }

        //distance from Alliance healthy
        float distanceH = bot->GetDistance(hx, hy, hz);
        if (distanceH < sPlayerbotAIConfig.sightDistance)
        {
            return MoveTo(bg->GetMapId(), hx, hy, hz);
        }
    }
    return false;

}

//Use Berserking Buff if near
bool BGTacticsWS::useBuff(BattleGround *bg)
{
    // don't use if already has buff
    if (bot->HasAura(23505) || bot->HasAura(24378))
        return false;
    // don't use if has flag
    if (bot->HasAura(23333) || bot->HasAura(23335))
        return false;

    BattleGroundWS* Bg = (BattleGroundWS *)bot->GetBattleGround();
    bool enemyHasFlag = Bg->GetFlagState(bot->GetTeam()) == BG_WS_FLAG_STATE_ON_PLAYER;
    if (enemyHasFlag)
        return false;

    //Buff Guids
    ObjectGuid Aguid = ObjectGuid(HIGHGUID_GAMEOBJECT, uint32(179905), uint32(90006));
    ObjectGuid Hguid = ObjectGuid(HIGHGUID_GAMEOBJECT, uint32(179905), uint32(90007));
    GameObject* Aberserk = bg->GetBgMap()->GetGameObject(Aguid);
    GameObject* Hberserk = bg->GetBgMap()->GetGameObject(Hguid);

    if (bot->IsWithinDist(Aberserk, 50) && !(Aberserk->GetRespawnTime() > 0))
    {
        return MoveTo(Aberserk->GetMapId(), Aberserk->GetPositionX(), Aberserk->GetPositionY(), Aberserk->GetPositionZ());
    }

    if (bot->IsWithinDist(Hberserk, 50) && !(Hberserk->GetRespawnTime() > 0))
    {
        return MoveTo(Hberserk->GetMapId(), Hberserk->GetPositionX(), Hberserk->GetPositionY(), Hberserk->GetPositionZ());
    }
    return false;
}

//run to enemy flag if not taken yet
bool BGTacticsWS::moveTowardsEnemyFlag(BattleGroundWS *bg)
{
    if (bg->GetFlagState(bg->GetOtherTeam(bot->GetTeam())) == BG_WS_FLAG_STATE_ON_PLAYER)
        return false;
    if (bg->GetFlagState(bot->GetTeam()) == BG_WS_FLAG_STATE_WAIT_RESPAWN)
        return false;

    WorldObject* target_obj = bg->GetBgMap()->GetWorldObject(bg->GetDroppedFlagGuid(bg->GetOtherTeam(bot->GetTeam()) == ALLIANCE ? HORDE : ALLIANCE));
    if (target_obj == nullptr)
    {
        target_obj = bg->GetBgMap()->GetWorldObject(bg->GetDroppedFlagGuid(bg->GetOtherTeam(bot->GetTeam())));
    }
    if (target_obj == nullptr)
    {
        //if no dropped flag
        if (bot->GetTeam() == HORDE)
            target_obj = bg->GetBgMap()->GetWorldObject(AllianceWsgFlagStand(bg));  //silverwing  
        else
            target_obj = bg->GetBgMap()->GetWorldObject(HordeWsgFlagStand(bg));  //warsong
    }

    //Direct Movement, if we are close
    if (bot->IsWithinDist(target_obj, 40))
    {
        ChaseTo(target_obj);
        return true;
    }

    WorldObject* obj = nullptr;

    if (bot->GetTeam() == HORDE)
        obj = bg->GetBgMap()->GetWorldObject(bg->GetHordeFlagCarrierGuid());
    else
        obj = bg->GetBgMap()->GetWorldObject(bg->GetAllianceFlagCarrierGuid());

    if (obj == nullptr)
    {
        //if no flagcarrier
        if (bot->GetTeam() == HORDE)
            obj = bg->GetBgMap()->GetWorldObject(AllianceWsgFlagStand(bg));  //silverwing 
        else
            obj = bg->GetBgMap()->GetWorldObject(HordeWsgFlagStand(bg));  //warsong
    }
    if (obj != nullptr)
    {
        return runPathTo(obj, bg);
    }
    else
        return false;
}

//if we have the flag, run home
bool BGTacticsWS::homerun(BattleGroundWS *bg)
{
    uint32 Preference = context->GetValue<uint32>("bg role")->Get();
    if (!(bg->GetFlagState(bg->GetOtherTeam(bot->GetTeam())) == BG_WS_FLAG_STATE_ON_PLAYER))
        return false;

    if (bot->GetObjectGuid() == bg->GetAllianceFlagCarrierGuid()) //flag-Carrier, bring it home (hordeguy)
    {
        WorldObject* obj = bg->GetBgMap()->GetWorldObject(bg->GetHordeFlagCarrierGuid());
        if (!obj || Preference < 9)
        {
            obj = bg->GetBgMap()->GetWorldObject(HordeWsgFlagStand(bg));  //warsong
        }
        if (bot->IsWithinDistInMap(obj, INTERACTION_DISTANCE) && (bg->GetFlagState(bot->GetTeam()) == BG_WS_FLAG_STATE_ON_BASE))
        {
            WorldPacket data(CMSG_AREATRIGGER);
            data << uint32(AT_WARSONG_FLAG);
            bot->GetSession()->HandleAreaTriggerOpcode(data);
            return true;
        }
        if (bot->IsWithinDistInMap(obj, 40.0f) && obj->GetTypeId() == TYPEID_PLAYER)
        {
            return ChaseTo(obj);
        }
        return runPathTo(obj, bg);
    }
    if (bot->GetObjectGuid() == bg->GetHordeFlagCarrierGuid())//flag-Carrier, bring it home (allianceguy)
    {
        WorldObject* obj = bg->GetBgMap()->GetWorldObject(bg->GetAllianceFlagCarrierGuid());
        if (!obj || Preference < 8)
        {
            obj = bg->GetBgMap()->GetWorldObject(AllianceWsgFlagStand(bg));  //silverwing
        }
        if (bot->IsWithinDistInMap(obj, INTERACTION_DISTANCE) && (bg->GetFlagState(bot->GetTeam()) == BG_WS_FLAG_STATE_ON_BASE))
        {
            WorldPacket data(CMSG_AREATRIGGER);
            data << uint32(AT_SILVERWING_FLAG);
            bot->GetSession()->HandleAreaTriggerOpcode(data);
            return true;
        }
        if (bot->IsWithinDistInMap(obj, 40.0f) && obj->GetTypeId() == TYPEID_PLAYER)
        {
            return ChaseTo(obj);
        }
        return runPathTo(obj, bg);
    }
    else
    {
        //hordeguy     GetAllianceFlagCarrierGuid
        //allianceguy  GetHordeFlagCarrierGuid
        //int Preference = urand(0, 9);
        //random choice if defense or offense
        bool supporter = Preference < 3;
        if (supporter || (bg->GetFlagState(bot->GetTeam()) != BG_WS_FLAG_STATE_ON_PLAYER))
        {
            if (bot->GetTeam() == ALLIANCE)
            {
                Player* ourGuyA = sObjectAccessor.FindPlayer(bg->GetHordeFlagCarrierGuid());
                if (ourGuyA != nullptr)
                {
                    if (!bot->IsWithinDist(ourGuyA, 40))
                        return runPathTo(ourGuyA, bg);

                    return Follow(ourGuyA);
                }
            }
            if (bot->GetTeam() == HORDE)
            {
                Player* ourGuyH = sObjectAccessor.FindPlayer(bg->GetAllianceFlagCarrierGuid());
                if (ourGuyH != nullptr)
                {
                    if (!bot->IsWithinDist(ourGuyH, 40))
                        return runPathTo(ourGuyH, bg);

                    return Follow(ourGuyH);
                }
            }
        }
        else
        {
            //as a none supporter, attack their flag, if they carry it
            if (bot->GetTeam() == HORDE)
            {
                Player* theirGuyA = sObjectAccessor.FindPlayer(bg->GetHordeFlagCarrierGuid());
                if (theirGuyA != nullptr)
                {
                    if (bot->IsWithinDist(theirGuyA, 40.0f))
                    {
                        bot->GetPlayerbotAI()->GetAiObjectContext()->GetValue<Unit*>("current target")->Set(theirGuyA);
                        bot->GetPlayerbotAI()->GetAiObjectContext()->GetValue<ObjectGuid>("pull target")->Set(bg->GetHordeFlagCarrierGuid());
                        return true;
                    }
                    return runPathTo(theirGuyA, bg);
                }
            }
            if (bot->GetTeam() == ALLIANCE)
            {
                Player* theirGuyH = sObjectAccessor.FindPlayer(bg->GetAllianceFlagCarrierGuid());
                if (theirGuyH != nullptr)
                {
                    if (bot->IsWithinDistInMap(theirGuyH, 40.0f))
                    {
                        bot->GetPlayerbotAI()->GetAiObjectContext()->GetValue<Unit*>("current target")->Set(theirGuyH);
                        bot->Attack((Unit*)theirGuyH, !ai->IsRanged(bot) || sServerFacade.GetDistance2d(bot, theirGuyH) <= sPlayerbotAIConfig.tooCloseDistance);
                        bot->GetPlayerbotAI()->GetAiObjectContext()->GetValue<ObjectGuid>("pull target")->Set(bg->GetAllianceFlagCarrierGuid());
                        return true;
                    }
                    return runPathTo(theirGuyH, bg);
                }
            }
        }
    }

    return false;
}

//the alliance base entrance is at x=1351.759155 , y=1462.368042, z = 324.673737
//the horde base entrance is at x=1125.778076, y=1452.059937, z =315.698883
//left center of the battlefield is at x=1239.085693,y=1541.408569,z=306.491791
//middle center of the battlefield is at x=1227.446289,y=1476.235718,z=307.484589
//right center of the battlefield is at x=1269.962158,y=1382.655640,z=308.545288
//the alliance flag is at x=1538.387207,y=1480.903198,z=352.576385
//the horde flag is at x=919.161316,x=1433.871338,z=345.902771

//horde healthy is at x=1111.526733,y=1352.458130,z=316.567047
//alliance healthy is at x=1320.670288,y=1549.642700, z=313.501404

//alliance fasty is at x=1449.574219,y=1470.698608, z=342.675476
//horde fasty is at x=1006.590210,y=1450.435059, z=335.721283

//alliance graveyard is at x=1370.087769, y=1539.914551, z=321.451538

//Alliance wants to move towards a lower x value

//get back to alliance base

//cross the BattleGround to get to flags or flag carriers
bool BGTacticsWS::runPathTo(WorldObject *target, BattleGround *bg)
{
    uint32 Preference = context->GetValue<uint32>("bg role")->Get();
    if (target == nullptr)
        return false;
    if (target->IsWithinDist(bot, 40) && sServerFacade.IsFriendlyTo(target, bot))
        return MoveNear(target);
    if (target->IsWithinDist(bot, 40) && sServerFacade.IsHostileTo(target, bot) && target->GetTypeId() == TYPEID_PLAYER)
        return bot->Attack((Unit*)target, !ai->IsRanged(bot) || sServerFacade.GetDistance2d(bot, target) <= sPlayerbotAIConfig.tooCloseDistance);
    if (target->GetPositionX() > bot->GetPositionX()) //He's somewhere at the alliance side
    {
        if (Preference < 6) //preference < 4 = move through tunnel (< 6 becuse GY disabled)
        {
            if (bot->GetPositionX() < 1006.f) //to the fasty
            {
                MoveTo(bg->GetMapId(), 1006.590210f, 1450.435059f, 335.721283f);
                return  true;
            }
            else if (bot->GetPositionX() < 1125.f) //to the horde entrance
            {
                if (bot->GetPositionY() < 1400.f)
                    MoveTo(bg->GetMapId(), 1125.778076f, bot->GetPositionY(), 316.567047f);
                else
                    MoveTo(bg->GetMapId(), 1125.778076f, 1452.059937f, 315.698883f);
                return  true;
            }
        }
        else if (Preference > 9) { // preference < 7 = move through graveyard (BUGGED)
            if (bot->GetPositionX() < 985.f) //to the gate at the upper tunnel
            {
                MoveTo(bg->GetMapId(), 985.940125f, 1423.260254f, 345.418121f);
                return  true;
            }
            else if (bot->GetPositionX() < 1054.5f) //to the gate at the upper tunnel
            {
                MoveTo(bg->GetMapId(), 1055.182251f, 1396.967529f, 339.361511f);
                return  true;
            }
            else if (bot->GetPositionX() < 1125.f) //to the horde entrance
            {
                MoveTo(bg->GetMapId(), 1125.778076f, bot->GetPositionY(), 316.567047f);
                return  true;
            }
        }
        else { //all other preference: run down the ramp 
            if (bot->GetPositionX() < 985.f) //to the gate at the upper tunnel
            {
                MoveTo(bg->GetMapId(), 985.940125f, 1423.260254f, 345.418121f);
                return  true;
            }
            else if (bot->GetPositionX() < 1031.f) //to the first step of the ramp from the tunnel
            {
                MoveTo(bg->GetMapId(), 1031.764282f, 1454.516235f, 343.337860f);
                return  true;
            }
            else if (bot->GetPositionX() < 1051.f && bot->GetPositionY() < 1494.f) //to the second step of the ramp from the tunnel
            {
                MoveTo(bg->GetMapId(), 1051.304810f, 1494.917725f, 342.043518f);
                return  true;
            }
            else if (bot->GetPositionX() < 1050.f && bot->GetPositionY() < 1538.f) //down the ramp
            {
                MoveTo(bg->GetMapId(), 1050.089478f, 1538.054443f, 332.460388f);
                return  true;
            }
            else if (bot->GetPositionX() < 1050.f && bot->GetPositionY() < 1560.f) //down the ramp
            {
                MoveTo(bg->GetMapId(), 1050.089478f, 1560.054443f, 332.460388f);
                return  true;
            }
            else if (bot->GetPositionX() < 1098.f) //at the ground now
            {
                MoveTo(bg->GetMapId(), 1098.716797f, 1535.618652f, 315.727539f);
                return  true;
            }
            else if (bot->GetPositionX() < 1239.f)
            {
                MoveTo(bg->GetMapId(), 1239.085693f, 1541.408569f + frand(-2, +2), 306.491791f);
                return  true;
            }
        }
        if (bot->GetPositionX() < 1227.f) //move to a more random location in the middle part
        {
            if (bot->GetPositionY() < 1400.f)
                MoveTo(bg->GetMapId(), 1269.962158f, 1382.655640f + frand(-2, +2), 308.545288f);
            else
                MoveTo(bg->GetMapId(), 1227.446289f, 1476.235718f + frand(-2, +2), 307.484589f);
            return  true;
        }
        if (Preference < 5) //through the tunnel
        {
            if (bot->GetPositionX() < 1351.f) //move to the alliance entrance
            {
                MoveTo(bg->GetMapId(), 1351.759155f + frand(0, 2), 1462.368042f + frand(-2, +2), 323.673737f);
                return  true;
            }
            else if (bot->GetPositionX() < 1449.f) //move to the alliance fasty
            {
                MoveTo(bg->GetMapId(), 1449.574219f, 1470.698608f, 342.675476f);
                return  true;
            }
            else { //move to the flag position
                MoveTo(bg->GetMapId(), 1538.387207f, 1480.903198f, 352.576385f);
                return  true;
            }
        }
        else { //up the ramp
            if (bot->GetPositionX() < 1360.f) //gate at the ramp
            {
                MoveTo(bg->GetMapId(), 1360.088501f, 1393.451660f + frand(-2, +2), 326.183624f);
                return  true;
            }
            if (bot->GetPositionX() < 1399.f) //half way up
            {
                MoveTo(bg->GetMapId(), 1399.362061f, 1405.105347f + frand(-2, +2), 341.481476f);
                return  true;
            }
            if (bot->GetPositionX() < 1417.f) //first halway
            {
                MoveTo(bg->GetMapId(), 1417.096191f, 1459.552368f + frand(-2, +2), 349.591827f);
                return  true;
            }
            if (bot->GetPositionX() < 1500.2f) //gate to the flag room
            {
                MoveTo(bg->GetMapId(), 1505.045654f, 1493.787231f, 352.017670f);
                return  true;
            }
            else { //move to the flag position
                MoveTo(bg->GetMapId(), 1538.387207f, 1480.903198f, 352.576385f);
                context->GetValue<uint32>("bg role")->Set(urand(0, 9));
                return  true;
            }
        }
    }
    else //move towards horde base
    {
        if (Preference < 4) //through the tunnel
        {
            if (bot->GetPositionX() > 1449.7f) //to the fasty
            {
                MoveTo(bg->GetMapId(), 1449.574219f, 1470.698608f, 342.675476f);
                return  true;
            }
            else if (bot->GetPositionX() > 1443.9f) { // moving from the fasty to the gate directly is bugged.. moving back to the tunnel first
                MoveTo(bg->GetMapId(), 1443.761963f, 1459.581909f, 342.115417f);
            }
            else if (bot->GetPositionX() > 1380.9f) { // move into the tunnel
                MoveTo(bg->GetMapId(), 1380.761963f, 1457.581909f, 329.115417f);
            }
            else if (bot->GetPositionX() > 1351.9f) //to the alliance entrance
            {
                if (bot->GetPositionY() > 1500.f)
                    MoveTo(bg->GetMapId(), 1125.778076f, bot->GetPositionY(), 315.698883f);
                else
                    MoveTo(bg->GetMapId(), 1125.778076f, 1452.059937f, 315.698883f);
                return  true;
            }
            if (bot->GetPositionX() > 1240.f) //move to a more random location in the middle part
            {
                if (bot->GetPositionY() > 1500.f)
                    MoveTo(bg->GetMapId(), 1239.085693f, 1541.408569f + frand(-2, +2), 306.491791f);
                else
                    MoveTo(bg->GetMapId(), 1227.446289f, 1476.235718f + frand(-2, +2), 307.484589f);
                return  true;
            }
        }
        else if (Preference < 7) // through the graveyard
        {
            if (bot->GetPositionX() > 1510.2f) //To the first gate
            {
                MoveTo(bg->GetMapId(), 1500.045654f, 1493.787231f, 352.017670f);
                return  true;
            }
            else if (bot->GetPositionX() > 1460.f) //to the second gate
            {
                MoveTo(bg->GetMapId(), 1459.490234f, 1494.175072f, 351.565155f);
                return  true;
            }
            else if (bot->GetPositionX() > 1424.f) //to the graveyard
            {
                MoveTo(bg->GetMapId(), 1423.106201f + frand(-2, +2), 1532.851196f, 342.152100f);
                return  true;
            }
            else if (bot->GetPositionX() > 1345.f) // to the field
            {
                MoveTo(bg->GetMapId(), 1344.334595f + frand(-2, +2), 1514.917236f, 319.081726f);
                return  true;
            }
        }
        else
        {
            if (bot->GetPositionX() > 1505.2f) //To the first gate
            {
            MoveTo(bg->GetMapId(), 1500.045654f, 1493.787231f, 352.017670f);
            return  true;
            }
            else if (bot->GetPositionX() > 1460.f) //to the second gate
            {
            MoveTo(bg->GetMapId(), 1459.490234f, 1494.175072f, 351.565155f);
            return  true;
            }
            else if (bot->GetPositionX() > 1418.f) //half on the upper ramp
            {
            MoveTo(bg->GetMapId(), 1417.096191f, 1459.552368f, 349.591827f);
            return  true;
            }
            else if (bot->GetPositionX() > 1400.f) //middle down the ramp
            {
            MoveTo(bg->GetMapId(), 1399.362061f, 1405.105347f, 341.481476f);
            return  true;
            }
            else if (bot->GetPositionX() > 1357.f) //at the gate
            {
            MoveTo(bg->GetMapId(), 1356.088501f, 1393.451660f, 326.183624f);
            return  true;
            }
            else if (bot->GetPositionX() > 1270.f) // run the gate side way to the middle field
            {
                MoveTo(bg->GetMapId(), 1269.962158f, 1398.655640f + frand(-2, +2), 309.945288f);
            //MoveTo(bg->GetMapId(), 1269.962158f, 1382.655640f + frand(-2, +2), 308.545288f);
            return true;
            }
        }
        if (Preference < 5) //horde ramp
        {
            if (bot->GetPositionX() > 1099.f) //move to the horde ramp gate
            {
                MoveTo(bg->GetMapId(), 1096.716797f, 1535.618652f, 315.727539f);
                return  true;
            }
            if (bot->GetPositionX() > 1071.f) //move the ramp up a piece
            {
                MoveTo(bg->GetMapId(), 1070.089478f, 1538.054443f, 332.460388f);
                return  true;
            }
            if (bot->GetPositionX() > 1050.2f) //move the ramp up a piece
            {
                MoveTo(bg->GetMapId(), 1050.089478f, 1538.054443f, 332.460388f);
                return  true;
            }
            if (bot->GetPositionX() > 1032.f) //up in front of first entrance
            {
                MoveTo(bg->GetMapId(), 1031.764282f, 1454.516235f, 343.337860f);
                return  true;
            }
            if (bot->GetPositionX() > 986.f) //up in front of first entrance
            {
                MoveTo(bg->GetMapId(), 985.940125f, 1423.260254f, 345.418121f);
                return  true;
            }
            MoveTo(bg->GetMapId(), 919.161316f, 1433.871338f, 345.902771f);
            return  true;
        }
        else { //horde tunnel

            if (bot->GetPositionX() > 1125.9f) //move to the horde entrance
            {
                MoveTo(bg->GetMapId(), 1125.778076f, 1452.059937f, 315.698883f);
                return  true;
            }
            else if (bot->GetPositionX() > 1006.7f) //move to the horde fasty
            {
                MoveTo(bg->GetMapId(), 1006.590210f, 1450.435059f, 335.721283f);
                return  true;
            }
            else { //move to the flag position
                MoveTo(bg->GetMapId(), 919.161316f, 1433.871338f, 345.902771f);
                context->GetValue<uint32>("bg role")->Set(urand(0, 9));
                return  true;
            }
        }
    }
    return false;
}

//is being called, when flag "+warsong" is set
bool BGTacticsWS::Execute(Event event)
{
    if (!bot->InBattleGround())
        return false;

    if (bot->GetBattleGround()->GetStatus() == STATUS_WAIT_LEAVE)
        return false;

    if (bot->IsDead() && bot->InBattleGround())
    {
        bot->GetMotionMaster()->MovementExpired();

        if (bot->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
            return false;

        BattleGround *bg = bot->GetBattleGround();
#ifdef MANGOS
        const WorldSafeLocsEntry *pos = bg->GetClosestGraveYard(bot);
#endif
#ifdef CMANGOS
        const WorldSafeLocsEntry *pos = sObjectMgr.GetClosestGraveYard(bot->GetPositionX(), bot->GetPositionY(), bot->GetPositionZ(), bot->GetMapId(), bot->GetTeam());
#endif

        if (!bot->IsWithinDist3d(pos->x, pos->y, pos->z, 3.0))
        {
            if (!bot->GetCorpse())
            {
                bot->RemovePet(PET_SAVE_REAGENTS);
                bot->BuildPlayerRepop();
                bot->RepopAtGraveyard();
                bot->SendHeartBeat();
                bot->GetPlayerbotAI()->CastSpell(2584, bot);
            }
        }
        return true;
    }
    //Check for Warsong.
#ifdef MANGOS
    if (bot->GetBattleGround()->GetTypeID() == BattleGroundTypeId::BATTLEGROUND_WS)
#endif
#ifdef CMANGOS
    if (bot->GetBattleGround()->GetTypeId() == BattleGroundTypeId::BATTLEGROUND_WS)
#endif
    {
        /*if (ai->GetMaster() && !ai->GetMaster()->GetPlayerbotAI() && !ai->GetMaster()->IsInSameRaidWith(bot))
        {
            ai->SetMaster(NULL);
            ai->ChangeStrategy("-follow", BOT_STATE_NON_COMBAT);
        }
        else
        {
            ai->SetMaster(ai->GetMaster());
            ai->ChangeStrategy("+follow,-warsong", BOT_STATE_NON_COMBAT);
            ai->ChangeStrategy("-warsong", BOT_STATE_COMBAT);
            //if(ai->HasStrategy("follow", BOT_STATE_NON_COMBAT))

        }*/
        //if(sRandomPlayerbotMgr.IsRandomBot(bot->GetGUIDLow()))
        //    ai->SetMaster(NULL);
        ai->SetMaster(NULL);
        if (bot->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION))
            bot->RemoveAurasDueToSpell(SPELL_AURA_SPIRIT_OF_REDEMPTION);
        if (bot->HasAura(2584))
            bot->RemoveAurasDueToSpell(2584);
        if (bot->IsInCombat())
        {
            //ai->Reset();
            if (!wasInCombat)
                bot->GetMotionMaster()->MovementExpired();
        }
        wasInCombat = bot->IsInCombat();

        //In Warsong, the bots run to the other flag and take it, try to get back and protect each other.
        //If our flag was taken, pures will try to get it back
        BattleGroundWS* bg = (BattleGroundWS *)bot->GetBattleGround();
        if (!bg)
            return false;

        if (bg != nullptr && !bot->IsDead())
        {
            // startup phase
            if (bg->GetStartDelayTime() > 0)
            {
                if (!ai->HasStrategy("collision", BOT_STATE_NON_COMBAT))
                    ai->ChangeStrategy("+collision", BOT_STATE_NON_COMBAT);

                return true;
            }
            if (ai->HasStrategy("collision", BOT_STATE_NON_COMBAT))
                ai->ChangeStrategy("-collision", BOT_STATE_NON_COMBAT);

            if (ai->HasStrategy("buff", BOT_STATE_NON_COMBAT))
                ai->ChangeStrategy("-buff", BOT_STATE_NON_COMBAT);

            //If flag is close, always click it.
            bool hasflag = (bot->GetObjectGuid() == bg->GetAllianceFlagCarrierGuid() || bot->GetObjectGuid() == bg->GetHordeFlagCarrierGuid());
            bool flagonbase = bg->GetFlagState(bot->GetTeam()) == BG_WS_FLAG_STATE_ON_BASE;
            bool flagdropped = (bg->GetFlagState(bot->GetTeam()) == BG_WS_FLAG_STATE_ON_GROUND || bg->GetFlagState(bg->GetOtherTeam(bot->GetTeam())) == BG_WS_FLAG_STATE_ON_GROUND);
            bool alreadyHasFlag = bg->GetFlagState(bg->GetOtherTeam(bot->GetTeam())) == BG_WS_FLAG_STATE_ON_PLAYER;
            bool enemyHasFlag = bg->GetFlagState(bot->GetTeam()) == BG_WS_FLAG_STATE_ON_PLAYER;
            if (!alreadyHasFlag || flagdropped)
            {
                GameObject* target_obj = bg->GetBgMap()->GetGameObject(bg->GetDroppedFlagGuid(bg->GetOtherTeam(bot->GetTeam()) == ALLIANCE ? HORDE : ALLIANCE));
                if (target_obj == nullptr)
                    target_obj = bg->GetBgMap()->GetGameObject(bg->GetDroppedFlagGuid(bg->GetOtherTeam(bot->GetTeam())));
                if (target_obj == nullptr)
                {
                    if (bot->GetTeam() == HORDE)
                    {
                        target_obj = bg->GetBgMap()->GetGameObject(AllianceWsgFlagStand(bg));  //silverwing
                    }
                    else
                    {
                        target_obj = bg->GetBgMap()->GetGameObject(HordeWsgFlagStand(bg));  //silverwing
                    }
                }
                if (bot->IsWithinDistInMap(target_obj, INTERACTION_DISTANCE))
                {
                    WorldPacket data(CMSG_GAMEOBJ_USE);
                    data << target_obj->GetObjectGuid();
                    if (bot->getClass() == CLASS_DRUID)
                    {
                        if (bot->GetShapeshiftForm() > 0)
                            bot->SetShapeshiftForm(FORM_NONE);
                        bot->GetPlayerbotAI()->RemoveAura("Prowl");
                    }
                    if (bot->IsMounted())
                    {
                        WorldPacket emptyPacket;
                        bot->GetSession()->HandleCancelMountAuraOpcode(emptyPacket);
                    }
                    if (bot->HasStealthAura())
                        bot->GetPlayerbotAI()->RemoveAura("Stealth");

#ifdef MANGOS
                    if(target_obj->isSpawned())
#endif
#ifdef CMANGOS
                    if (target_obj->IsSpawned())
#endif
                        bot->GetSession()->HandleGameObjectUseOpcode(data);
                    ai->Reset();
                    return true;
                }
                else
                {
                    if (bot->IsWithinDist(target_obj, 40))
                    {
                        MoveNear(target_obj);
                        return true;
                    }
                }
            }
            //check if we are moving or in combat
            if (!IsMovingAllowed()/* || bot->isMoving()*/)
                return false;
            bool moving = false;
#ifdef MANGOS
            if (!bot->isMoving())
#endif
#ifdef CMANGOS
            if (!bot->IsMoving())
#endif
                ai->Reset();
            //Only go for directive, if not in combat
            if (!bot->IsInCombat())
            {
                moving = useBuff(bg);
                if (!moving)
                    moving = consumeHealthy(bg);
                if (!moving)
                    moving = moveTowardsEnemyFlag(bg);
                if (!moving)
                    moving = homerun(bg);
            }
            int Preference = sRandomPlayerbotMgr.GetValue(bot->GetGUIDLow(), "preference");
            //if (!moving)
            //    moving = moveTowardsEnemyFlag(bg);
            if (!moving && hasflag)
                moving = homerun(bg);
            //if (!moving && !enemyHasFlag)
            //    moving = moveTowardsEnemyFlag(bg);
            //moving = consumeHealthy(bg);
            //if (!moving)
            
            if (!moving)
            {
#ifdef MANGOS
                if (!flagdropped && !alreadyHasFlag && !bg->GetBgMap()->GetGameObject(HordeWsgFlagStand(bg))->isSpawned() && !bg->GetBgMap()->GetGameObject(AllianceWsgFlagStand(bg))->isSpawned())
#endif
#ifdef CMANGOS
                if (!flagdropped && !alreadyHasFlag && !bg->GetBgMap()->GetGameObject(HordeWsgFlagStand(bg))->IsSpawned() && !bg->GetBgMap()->GetGameObject(AllianceWsgFlagStand(bg))->IsSpawned())
#endif
                {
                    float distance = sPlayerbotAIConfig.tooCloseDistance + sPlayerbotAIConfig.grindDistance * urand(3, 10) / 10.0f;

                    Map* map = bot->GetMap();

                    for (int i = 0; i < 10; ++i)
                    {
                        float x = bot->GetPositionX();
                        float y = bot->GetPositionY();
                        float z = bot->GetPositionZ();
                        x += urand(0, distance) - distance / 2;
                        y += urand(0, distance) - distance / 2;
                        bot->UpdateGroundPositionZ(x, y, z);

                        //if (map->IsInWater(x, y, z))
                           //continue;
                         //  continue;

                        bool moved = MoveNear(bot->GetMapId(), x, y, z);
                        if (moved)
                            break;
                    }
                }
                else
                {
                    //ai->Reset();
                    //AttackEnemyPlayerAction* action = new AttackEnemyPlayerAction(ai);
                    AttackAnythingAction* action = new AttackAnythingAction(ai);
                    action->Execute(event);
                }
            }
        }
    }
    return true;
}

bool ArenaTactics::Execute(Event event)
{
#ifndef MANGOSBOT_ZERO
    if (!bot->InBattleGround())
    {
        bool IsRandomBot = sRandomPlayerbotMgr.IsRandomBot(bot->GetGUIDLow());
        ai->ChangeStrategy("-arena", BOT_STATE_COMBAT);
        ai->ChangeStrategy("-arena", BOT_STATE_NON_COMBAT);
        ai->ResetStrategies(!IsRandomBot);
        return false;
    }

    if (bot->GetBattleGround()->GetStatus() == STATUS_WAIT_LEAVE)
        return false;

    if (bot->IsDead())
    {
        return false;
    }

    BattleGround *bg = bot->GetBattleGround();
    if (!bg)
        return false;

    // startup phase
    if (bg->GetStartDelayTime() > 0)
        return false;

    if (ai->HasStrategy("collision", BOT_STATE_NON_COMBAT))
        ai->ChangeStrategy("-collision", BOT_STATE_NON_COMBAT);

    if (ai->HasStrategy("buff", BOT_STATE_NON_COMBAT))
        ai->ChangeStrategy("-buff", BOT_STATE_NON_COMBAT);

    if (sBattleGroundMgr.IsArenaType(bg->GetTypeID()))
    {
        ai->ResetStrategies(false);
        ai->SetMaster(NULL);
    }

    if (!bot->IsInCombat())
        return moveToCenter(bg);
    else
    {
        AttackAnythingAction* action = new AttackAnythingAction(ai);
        return action->Execute(event);
    }
#endif
    return true;
}

bool ArenaTactics::moveToCenter(BattleGround *bg)
{
#ifndef MANGOSBOT_ZERO
    uint32 Preference = context->GetValue<uint32>("bg role")->Get();
    switch (bg->GetTypeID())
    {
    case BATTLEGROUND_BE:
        if (Preference > 10)
            MoveTo(bg->GetMapId(), 6185.0f + frand(-2, +2), 236.0f + frand(-2, +2), 6.0f, false, true);
        else
            MoveTo(bg->GetMapId(), 6240.0f + frand(-2, +2), 262.0f + frand(-2, +2), 2.0f, false, true);
        break;
    case BATTLEGROUND_RL:
        if (Preference < 5)
            MoveTo(bg->GetMapId(), 1320.0f + frand(-2, +2), 1672.0f + frand(-2, +2), 38.0f, false, true);
        else
            MoveTo(bg->GetMapId(), 1273.0f + frand(-2, +2), 1666.0f + frand(-2, +2), 36.0f, false, true);
        break;
    case BATTLEGROUND_NA:
        MoveTo(bg->GetMapId(), 4055.0f + frand(-5, +5), 2921.0f + frand(-5, +5), 15.1f, false, true);
        break;
    default:
        break;
    }
    if (urand(0, 100) > 70)
        context->GetValue<uint32>("bg role")->Set(urand(0, 9));
#endif
    return true;
}
