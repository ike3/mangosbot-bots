#include "botpch.h"
#include "../../playerbot.h"
#include "strategy/values/PositionValue.h"
#include "MovementActions.h"
#include "BattleGround.h"
#include "BattleGroundMgr.h"
#include "BattlegroundTactics.h"

using namespace ai;

//
// old bg tactics code
//

#ifdef MANGOS
enum GameObjectsWS
{
    GO_WS_SILVERWING_FLAG = 179830,
    GO_WS_WARSONG_FLAG = 179831
};
#endif

/*typedef void(*BattleBotWaypointFunc) ();

struct BattleBotWaypoint
{
    BattleBotWaypoint(float x_, float y_, float z_, BattleBotWaypointFunc func) :
        x(x_), y(y_), z(z_), pFunc(func) {};
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    BattleBotWaypointFunc pFunc = nullptr;
};

typedef std::vector<BattleBotWaypoint> BattleBotPath;*/

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

/*ObjectGuid BGTacticsWS::FindWsHealthy(BattleGround * bg)
{
    list<ObjectGuid> bg_gos = *ai->GetAiObjectContext()->GetValue<list<ObjectGuid> >("bg game objects");
    ObjectGuid WsHealthy;
    return WsHealthy;
}*/

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

#ifdef MANGOS
    BattleGroundWS* Bg = (BattleGroundWS *)bot->GetBattleGround();
    bool enemyHasFlag = Bg->GetFlagState(bot->GetTeam()) == BG_WS_FLAG_STATE_ON_PLAYER;
    if (enemyHasFlag)
        return false;
#endif

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
#ifdef MANGOS
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
#endif
        return false;
}

//if we have the flag, run home
bool BGTacticsWS::homerun(BattleGroundWS *bg)
{
#ifdef MANGOS
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

#endif
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
#ifdef MANGOS
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
#endif
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
#ifdef MANGOS
    if (sBattleGroundMgr.IsArenaType(bg->GetTypeID()))
#endif
#ifdef CMANGOS
    if (sBattleGroundMgr.IsArenaType(bg->GetTypeId()))
#endif
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
#ifdef MANGOS
    switch (bg->GetTypeID())
#endif
#ifdef CMANGOS
    switch (bg->GetTypeId())
#endif
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

//
// actual bg tactics below
//

bool BGTactics::Execute(Event event)
{
    if (!bot->InBattleGround())
        return false;

    BattleGround *bg = bot->GetBattleGround();
    if (!bg)
        return false;

    if (bg->GetStatus() == STATUS_WAIT_LEAVE)
        return false;

    std::vector<BattleBotPath*> const* vPaths;
    std::vector<uint32> const* vFlagIds;
    switch (bot->GetBattleGround()->GetTypeId())
    {
    case BATTLEGROUND_AB:
    {
        vPaths = &vPaths_AB;
        vFlagIds = &vFlagsAB;
        break;
    }
    case BATTLEGROUND_AV:
    {
        vPaths = &vPaths_AV;
        vFlagIds = &vFlagsAV;
        break;
    }
    case BATTLEGROUND_WS:
    {
        vPaths = &vPaths_WS;
        vFlagIds = &vFlagsWS;
        break;
    }
    default:
        break;
    }

    if (getName() == "move to start")
        return moveToStart();

    if (getName() == "select objective")
    {
        return selectObjective();
    }
    
    if (getName() == "protect fc")
    {
        if (!bot->IsMounted() && !sServerFacade.IsInCombat(bot))
            if (ai->DoSpecificAction("check mount state"))
                return true;

        uint32 role = context->GetValue<uint32>("bg role")->Get();
        bool supporter = role < 5;
        if (supporter)
            if (protectFC())
                return true;
    }

    if (getName() == "move to objective")
    {
        if (bg->GetStatus() == STATUS_WAIT_JOIN)
            return false;

        if (bot->IsMoving())
            return false;
        
        if (!bot->IsStopped())
            return false;

        switch (bot->GetMotionMaster()->GetCurrentMovementGeneratorType())
        {
        case IDLE_MOTION_TYPE:
        case CHASE_MOTION_TYPE:
        case POINT_MOTION_TYPE:
            break;
        default:
            return true;
        }

        if (vFlagIds)
            if (atFlag(*vPaths, *vFlagIds))
                return true;

        if (useBuff())
            return true;

        if (sServerFacade.IsInCombat(bot) && !(bot->HasAura(BG_WS_SPELL_WARSONG_FLAG) || bot->HasAura(BG_WS_SPELL_SILVERWING_FLAG)))
        {
            //bot->GetMotionMaster()->MovementExpired();
            return false;
        }

        if (!moveToObjective())
        {
            if (selectObjectiveWp(*vPaths))
                return true;
        }
        else
            return true;

        // bot with flag should only move to objective
        if (bot->HasAura(BG_WS_SPELL_WARSONG_FLAG) || bot->HasAura(BG_WS_SPELL_SILVERWING_FLAG))
            return false;

        if (startNewPathBegin(*vPaths))
            return true;

        if (startNewPathFree(*vPaths))
            return true;
    }

    if (getName() == "use buff")
        return useBuff();

    if (getName() == "check flag")
    {
        if (vFlagIds)
        {
            if (atFlag(*vPaths, *vFlagIds))
                return true;
            else
                return false;
        }
        else
            return false;
    }

    if (getName() == "check objective")
        return resetObjective();

    return false;
}

bool BGTactics::moveToStart()
{
    BattleGround *bg = bot->GetBattleGround();
    if (!bg)
        return false;

    if (bg->GetStatus() != STATUS_WAIT_JOIN)
        return false;

    if (bg->GetTypeId() == BATTLEGROUND_WS)
    {
        int startSpot = urand(BB_WSG_WAIT_SPOT_SPAWN, BB_WSG_WAIT_SPOT_RIGHT);
        if (startSpot == BB_WSG_WAIT_SPOT_RIGHT)
        {
            if (bot->GetTeam() == HORDE)
                MoveTo(bg->GetMapId(), WS_WAITING_POS_HORDE_1.x + frand(-2.0f, 2.0f), WS_WAITING_POS_HORDE_1.y + frand(-2.0f, 2.0f), WS_WAITING_POS_HORDE_1.z);
            else
                MoveTo(bg->GetMapId(), WS_WAITING_POS_ALLIANCE_1.x + frand(-2.0f, 2.0f), WS_WAITING_POS_ALLIANCE_1.y + frand(-2.0f, 2.0f), WS_WAITING_POS_ALLIANCE_1.z);
        }
        else if (startSpot == BB_WSG_WAIT_SPOT_LEFT)
        {
            if (bot->GetTeam() == HORDE)
                MoveTo(bg->GetMapId(), WS_WAITING_POS_HORDE_2.x + frand(-2.0f, 2.0f), WS_WAITING_POS_HORDE_2.y + frand(-2.0f, 2.0f), WS_WAITING_POS_HORDE_2.z);
            else
                MoveTo(bg->GetMapId(), WS_WAITING_POS_ALLIANCE_2.x + frand(-2.0f, 2.0f), WS_WAITING_POS_ALLIANCE_2.y + frand(-2.0f, 2.0f), WS_WAITING_POS_ALLIANCE_2.z);
        }
    }
    else if (bg->GetTypeId() == BATTLEGROUND_AB)
    {
        if (bot->GetTeam() == HORDE)
            MoveTo(bg->GetMapId(), AB_WAITING_POS_HORDE.x + frand(-2.0f, 2.0f), AB_WAITING_POS_HORDE.y + frand(-2.0f, 2.0f), AB_WAITING_POS_HORDE.z);
        else
            MoveTo(bg->GetMapId(), AB_WAITING_POS_ALLIANCE.x + frand(-2.0f, 2.0f), AB_WAITING_POS_ALLIANCE.y + frand(-2.0f, 2.0f), AB_WAITING_POS_ALLIANCE.z);
    }
    else if (bg->GetTypeId() == BATTLEGROUND_AV)
    {
        if (bot->GetTeam() == HORDE)
            MoveTo(bg->GetMapId(), AV_WAITING_POS_HORDE.x + frand(-3.0f, 3.0f), AV_WAITING_POS_HORDE.y + frand(-3.0f, 3.0f), AV_WAITING_POS_HORDE.z);
        else
            MoveTo(bg->GetMapId(), AV_WAITING_POS_ALLIANCE.x + frand(-3.0f, 3.0f), AV_WAITING_POS_ALLIANCE.y + frand(-3.0f, 3.0f), AV_WAITING_POS_ALLIANCE.z);
    }

    return true;
}

bool BGTactics::selectObjective(bool reset)
{
    BattleGround *bg = bot->GetBattleGround();
    if (!bg)
        return false;

    if (bg->GetStatus() != STATUS_IN_PROGRESS)
        return false;

    ai::PositionMap& posMap = context->GetValue<ai::PositionMap&>("position")->Get();
    ai::PositionEntry pos = context->GetValue<ai::PositionMap&>("position")->Get()["bg objective"];
    if (pos.isSet() && !reset)
        return false;

    WorldObject* BgObjective = nullptr;

    switch (bg->GetTypeId())
    {
    case BATTLEGROUND_AV:
    {
        // Alliance and Horde code is intentionally different.
        // Horde bots are more united and always go together.
        // Alliance bots can pick random objective.

        if (bot->GetTeam() == HORDE)
        {
            // End Boss
            if (!bg->IsActiveEvent(BG_AV_NODES_DUNBALDAR_SOUTH, BG_AV_NODE_STATUS_ALLY_OCCUPIED) &&
                !bg->IsActiveEvent(BG_AV_NODES_DUNBALDAR_NORTH, BG_AV_NODE_STATUS_ALLY_OCCUPIED) &&
                !bg->IsActiveEvent(BG_AV_NODES_ICEWING_BUNKER, BG_AV_NODE_STATUS_ALLY_OCCUPIED) &&
                !bg->IsActiveEvent(BG_AV_NODES_STONEHEART_BUNKER, BG_AV_NODE_STATUS_ALLY_OCCUPIED) &&
                !bg->IsActiveEvent(BG_AV_NODES_FIRSTAID_STATION, BG_AV_NODE_STATUS_ALLY_OCCUPIED))
            {
                if (Creature* pVanndar = bot->GetMap()->GetCreature(bg->GetSingleCreatureGuid(BG_AV_BOSS_A, 0)))
                {
                    BgObjective = pVanndar;
                    ostringstream out; out << "Attacking Vanndar!";
                    bot->Say(out.str(), LANG_UNIVERSAL);
                }
            }

            // Only go to Snowfall Graveyard if already close to it.
            // Need to fix AV script
            if (!BgObjective && (/*bg->IsActiveEvent(BG_AV_NODES_SNOWFALL_GRAVE, BG_AV_NODE_STATUS_ALLY_CONTESTED) || */bg->IsActiveEvent(BG_AV_NODES_SNOWFALL_GRAVE, BG_AV_NODE_STATUS_ALLY_OCCUPIED) || bg->IsActiveEvent(BG_AV_NODES_SNOWFALL_GRAVE, BG_AV_NODE_STATUS_NEUTRAL_OCCUPIED)))
            {
                if (GameObject* pGO = bot->GetMap()->GetGameObject(bg->GetSingleGameObjectGuid(BG_AV_NODES_SNOWFALL_GRAVE, BG_AV_NODE_STATUS_NEUTRAL_OCCUPIED)))
                    if (bot->IsWithinDist(pGO, VISIBILITY_DISTANCE_LARGE))
                    {
                        BgObjective = pGO;
                        ostringstream out; out << "Attacking Snowfall GY!";
                        bot->Say(out.str(), LANG_UNIVERSAL);
                    }
            }

            if (!BgObjective && !bg->IsActiveEvent(BG_AV_NODE_CAPTAIN_DEAD_A, 0))
            {
                if (Creature* pBalinda = bot->GetMap()->GetCreature(bg->GetSingleCreatureGuid(BG_AV_CAPTAIN_A, 0)))
                {
                    if (pBalinda->GetDeathState() != DEAD)
                    {
                        BgObjective = pBalinda;
                        ostringstream out; out << "Attacking Balinda!";
                        bot->Say(out.str(), LANG_UNIVERSAL);
                    }
                }
            }

            if (!BgObjective)
            {
                for (const auto& objective : AV_HordeDefendObjectives)
                {
                    if (!BgObjective && bg->IsActiveEvent(objective.first, BG_AV_NODE_STATUS_ALLY_CONTESTED))
                    {
                        if (GameObject* pGO = bot->GetMap()->GetGameObject(bg->GetSingleGameObjectGuid(objective.first, objective.second)))
                            if (bot->IsWithinDist(pGO, VISIBILITY_DISTANCE_LARGE))
                            {
                                BgObjective = pGO;
                                ostringstream out; out << "Defending Node #" << objective.first;
                                bot->Say(out.str(), LANG_UNIVERSAL);
                            }
                    }
                }
            }

            if (!BgObjective)
            {
                for (const auto& objective : AV_HordeAttackObjectives)
                {
                    if (!BgObjective && (bg->IsActiveEvent(objective.first, BG_AV_NODE_STATUS_ALLY_CONTESTED) || bg->IsActiveEvent(objective.first, BG_AV_NODE_STATUS_ALLY_OCCUPIED) || bg->IsActiveEvent(objective.first, BG_AV_NODE_STATUS_NEUTRAL_OCCUPIED)))
                    {
                        if (GameObject* pGO = bot->GetMap()->GetGameObject(bg->GetSingleGameObjectGuid(objective.first, objective.second)))
                        {
                            BgObjective = pGO;
                            ostringstream out; out << "Attacking Node #" << objective.first;
                            bot->Say(out.str(), LANG_UNIVERSAL);
                        }
                    }
                }
            }
        }
        else // ALLIANCE
        {
            // End boss
            if (!bg->IsActiveEvent(BG_AV_NODES_ICEBLOOD_TOWER, BG_AV_NODE_STATUS_HORDE_OCCUPIED) &&
                !bg->IsActiveEvent(BG_AV_NODES_TOWER_POINT, BG_AV_NODE_STATUS_HORDE_OCCUPIED) &&
                !bg->IsActiveEvent(BG_AV_NODES_FROSTWOLF_ETOWER, BG_AV_NODE_STATUS_HORDE_OCCUPIED) &&
                !bg->IsActiveEvent(BG_AV_NODES_FROSTWOLF_WTOWER, BG_AV_NODE_STATUS_HORDE_OCCUPIED) &&
                !bg->IsActiveEvent(BG_AV_NODES_FROSTWOLF_HUT, BG_AV_NODE_STATUS_HORDE_OCCUPIED))
            {
                if (Creature* pDrek = bot->GetMap()->GetCreature(bg->GetSingleCreatureGuid(BG_AV_BOSS_H, 0)))
                {
                    BgObjective = pDrek;
                    ostringstream out; out << "Attacking DrekThar!";
                    bot->Say(out.str(), LANG_UNIVERSAL);
                }
            }

            // Only go to Snowfall Graveyard if already close to it.
            // Need to fix AV script
            if (!BgObjective && (/*bg->IsActiveEvent(BG_AV_NODES_SNOWFALL_GRAVE, BG_AV_NODE_STATUS_HORDE_CONTESTED) || */bg->IsActiveEvent(BG_AV_NODES_SNOWFALL_GRAVE, BG_AV_NODE_STATUS_HORDE_OCCUPIED) || bg->IsActiveEvent(BG_AV_NODES_SNOWFALL_GRAVE, BG_AV_NODE_STATUS_NEUTRAL_OCCUPIED)))
            {
                if (GameObject* pGO = bot->GetMap()->GetGameObject(bg->GetSingleGameObjectGuid(BG_AV_NODES_SNOWFALL_GRAVE, BG_AV_NODE_STATUS_NEUTRAL_OCCUPIED)))
                    if (bot->IsWithinDist(pGO, VISIBILITY_DISTANCE_NORMAL))
                    {
                        BgObjective = pGO;
                        ostringstream out; out << "Attacking Snowfall GY!";
                        bot->Say(out.str(), LANG_UNIVERSAL);
                    }
            }

            // Chance to defend.
            if (!BgObjective && urand(0, 99) < 25)
            {
                for (const auto& objective : AV_AllianceDefendObjectives)
                {
                    if (!BgObjective && bg->IsActiveEvent(objective.first, BG_AV_NODE_STATUS_HORDE_CONTESTED))
                    {
                        if (GameObject* pGO = bot->GetMap()->GetGameObject(bg->GetSingleGameObjectGuid(objective.first, objective.second)))
                        {
                            BgObjective = pGO;
                            ostringstream out; out << "Defending Node #" << objective.first;
                            bot->Say(out.str(), LANG_UNIVERSAL);
                        }
                    }
                }
            }

            if (!BgObjective)
            {
                // Attack closest objective.
                WorldObject* pAttackObjectiveObject = nullptr;
                float attackObjectiveDistance = FLT_MAX;

                if (!bg->IsActiveEvent(BG_AV_NODE_CAPTAIN_DEAD_H, 0))
                {
                    if (Creature* pGalvangar = bot->GetMap()->GetCreature(bg->GetSingleCreatureGuid(BG_AV_CAPTAIN_H, 0)))
                    {
                        pAttackObjectiveObject = pGalvangar;
                        attackObjectiveDistance = sqrt(bot->GetDistance(pGalvangar));
                        ostringstream out; out << "Attacking Galvangar!";
                        bot->Say(out.str(), LANG_UNIVERSAL);
                    }
                }

                for (const auto& objective : AV_AllianceAttackObjectives)
                {
                    if (bg->IsActiveEvent(objective.first, BG_AV_NODE_STATUS_HORDE_CONTESTED) || bg->IsActiveEvent(objective.first, BG_AV_NODE_STATUS_HORDE_OCCUPIED) || bg->IsActiveEvent(objective.first, BG_AV_NODE_STATUS_NEUTRAL_OCCUPIED))
                    {
                        if (GameObject* pGO = bot->GetMap()->GetGameObject(bg->GetSingleGameObjectGuid(objective.first, objective.second)))
                        {
                            float const distance = sqrt(bot->GetDistance(pGO));
                            if (attackObjectiveDistance > distance)
                            {
                                pAttackObjectiveObject = pGO;
                                attackObjectiveDistance = distance;
                                ostringstream out; out << "Attacking Node #" << objective.first;
                                bot->Say(out.str(), LANG_UNIVERSAL);
                            }
                        }
                    }
                }

                if (pAttackObjectiveObject)
                {
                    BgObjective = pAttackObjectiveObject;
                }
            }
        }
        if (BgObjective)
        {
            pos.Set(BgObjective->GetPositionX(), BgObjective->GetPositionY(), BgObjective->GetPositionZ(), BgObjective->GetMapId());
            posMap["bg objective"] = pos;
            ostringstream out;
            out << "BG objective set to " << BgObjective->GetName() << " " << pos.x << " " << pos.y;
            bot->Say(out.str(), LANG_UNIVERSAL);
            return true;
        }
        break;
    }
    case BATTLEGROUND_WS:
    {
        // test free roam
        // if (!flagTaken() && !teamFlagTaken())
        //     break;

        if (bot->HasAura(BG_WS_SPELL_WARSONG_FLAG) || bot->HasAura(BG_WS_SPELL_SILVERWING_FLAG))
        {
            if (bot->GetTeam() == ALLIANCE)
            {
                if (teamFlagTaken())
                {
                    Position hidePos = WS_FLAG_HIDE_ALLIANCE[urand(0, 2)];
                    pos.Set(hidePos.x, hidePos.y, hidePos.z, bot->GetMapId());
                }
                else
                    pos.Set(WS_FLAG_POS_ALLIANCE.x, WS_FLAG_POS_ALLIANCE.y, WS_FLAG_POS_ALLIANCE.z, bot->GetMapId());
            }
            else
            {
                if (teamFlagTaken())
                {
                    Position hidePos = WS_FLAG_HIDE_HORDE[urand(0, 2)];
                    pos.Set(hidePos.x, hidePos.y, hidePos.z, bot->GetMapId());
                }
                else
                    pos.Set(WS_FLAG_POS_HORDE.x, WS_FLAG_POS_HORDE.y, WS_FLAG_POS_HORDE.z, bot->GetMapId());
            }

            ostringstream out;
            if (teamFlagTaken())
                out << "Hiding with flag" << (bot->GetTeam() == ALLIANCE ? "in Alliance base" : "in Horde base");
            else
                out << "Taking " << (bot->GetTeam() == ALLIANCE ? "Horde flag to base" : "Alliance flag to base");
            bot->Say(out.str(), LANG_UNIVERSAL);
        }
        else
        {
            uint32 role = context->GetValue<uint32>("bg role")->Get();
            bool supporter = role < 4;

            ostringstream out;
            out << "Role: " << role;
            bot->Say(out.str(), LANG_UNIVERSAL);

            if (supporter)
            {
                Unit* teamFC = AI_VALUE(Unit*, "team flag carrier");
                if (teamFC)
                {
                    ostringstream out;
                    out << "Protecting " << (bot->GetTeam() == ALLIANCE ? "Alliance FC" : "Horde FC");
                    bot->Say(out.str(), LANG_UNIVERSAL);
                    pos.Set(teamFC->GetPositionX(), teamFC->GetPositionY(), teamFC->GetPositionZ(), bot->GetMapId());
                    if (sServerFacade.GetDistance2d(bot, teamFC) < 50.0f)
                        Follow(teamFC);
                }
                else
                {
                    Unit* enemyFC = AI_VALUE(Unit*, "enemy flag carrier");
                    if (enemyFC)
                    {
                        pos.Set(enemyFC->GetPositionX(), enemyFC->GetPositionY(), enemyFC->GetPositionZ(), bot->GetMapId());

                        ostringstream out;
                        out << "Attacking " << (bot->GetTeam() == ALLIANCE ? "Horde FC" : "Alliance FC");
                        bot->Say(out.str(), LANG_UNIVERSAL);
                    }
                    else
                    {
                        if (bot->GetTeam() == ALLIANCE)
                            pos.Set(WS_FLAG_POS_HORDE.x, WS_FLAG_POS_HORDE.y, WS_FLAG_POS_HORDE.z, bot->GetMapId());
                        else
                            pos.Set(WS_FLAG_POS_ALLIANCE.x, WS_FLAG_POS_ALLIANCE.y, WS_FLAG_POS_ALLIANCE.z, bot->GetMapId());

                        ostringstream out;
                        out << "Going to " << (bot->GetTeam() == ALLIANCE ? "take Horde flag" : "take Alliance flag");
                        bot->Say(out.str(), LANG_UNIVERSAL);
                    }
                }
            }
            else
            {
                Unit* enemyFC = AI_VALUE(Unit*, "enemy flag carrier");
                if (enemyFC)
                {
                    pos.Set(enemyFC->GetPositionX(), enemyFC->GetPositionY(), enemyFC->GetPositionZ(), bot->GetMapId());

                    ostringstream out;
                    out << "Attacking " << (bot->GetTeam() == ALLIANCE ? "Horde FC" : "Alliance FC");
                    bot->Say(out.str(), LANG_UNIVERSAL);
                }
                else
                {
                    if (role > 9)  // test patrol
                    {
                        float rx, ry, rz;
                        bot->GetRandomPoint(1227.446f, 1476.235f, 307.484, 150.0f, rx, ry, rz);
                        pos.Set(rx, ry, rz, bot->GetMapId());
                        ostringstream out;
                        out << "Patrolling battlefield";
                        bot->Say(out.str(), LANG_UNIVERSAL);
                    }
                    else
                    {
                        if (bot->GetTeam() == ALLIANCE)
                            pos.Set(WS_FLAG_POS_HORDE.x, WS_FLAG_POS_HORDE.y, WS_FLAG_POS_HORDE.z, bot->GetMapId());
                        else
                            pos.Set(WS_FLAG_POS_ALLIANCE.x, WS_FLAG_POS_ALLIANCE.y, WS_FLAG_POS_ALLIANCE.z, bot->GetMapId());

                        ostringstream out;
                        out << "Going to " << (bot->GetTeam() == ALLIANCE ? "take Horde flag" : "take Alliance flag");
                        bot->Say(out.str(), LANG_UNIVERSAL);
                    }
                }
            }
        }
        if (pos.isSet())
        {
            posMap["bg objective"] = pos;
            return true;
        }
        break;
    }
    case BATTLEGROUND_AB:
    {
        if (bot->GetTeam() == HORDE) // HORDE
        {
            if (!BgObjective)
            {
                // copy of alliance tactics
                uint32 role = context->GetValue<uint32>("bg role")->Get();
                bool defender = role < 4;

                // pick 3 objectives
                std::vector<GameObject*> objectives;
                for (auto i = 0; i < 3; ++i)
                {
                    WorldObject* pAttackObjectiveObject = nullptr;
                    float attackObjectiveDistance = FLT_MAX;

                    for (const auto& objective : AB_AttackObjectives)
                    {
                        if (bg->IsActiveEvent(objective.first, BG_AB_NODE_STATUS_NEUTRAL) || bg->IsActiveEvent(objective.first, BG_AB_NODE_STATUS_ALLY_OCCUPIED) || bg->IsActiveEvent(objective.first, BG_AB_NODE_STATUS_ALLY_CONTESTED))
                        {
                            if (GameObject* pGO = bot->GetMap()->GetGameObject(bg->GetSingleGameObjectGuid(objective.first, defender ? BG_AB_NODE_STATUS_ALLY_CONTESTED : BG_AB_NODE_STATUS_ALLY_OCCUPIED)))
                            {
                                float const distance = sqrt(bot->GetDistance(pGO));
                                if (attackObjectiveDistance > distance)
                                {
                                    // do not pick if already in list
                                    vector<GameObject*>::iterator f = find(objectives.begin(), objectives.end(), pGO);
                                    if (f != objectives.end())
                                        continue;

                                    objectives.push_back(pGO);
                                    attackObjectiveDistance = distance;
                                    ostringstream out; out << "Possible Attack Point #" << objective.first;
                                    bot->Say(out.str(), LANG_UNIVERSAL);
                                }
                            }
                        }
                    }
                }
                if (!objectives.empty())
                {
                    // pick random objective
                    BgObjective = objectives[urand(0, objectives.size() - 1)];
                }
            }
        }
        else // ALLIANCE
        {
            uint32 role = context->GetValue<uint32>("bg role")->Get();
            bool defender = role < 4;

            // pick 3 objectives
            std::vector<GameObject*> objectives;
            for (auto i = 0; i < 3; ++i)
            {
                WorldObject* pAttackObjectiveObject = nullptr;
                float attackObjectiveDistance = FLT_MAX;

                for (const auto& objective : AB_AttackObjectives)
                {
                    if (bg->IsActiveEvent(objective.first, BG_AB_NODE_STATUS_NEUTRAL) || bg->IsActiveEvent(objective.first, BG_AB_NODE_STATUS_HORDE_OCCUPIED) || bg->IsActiveEvent(objective.first, BG_AB_NODE_STATUS_HORDE_CONTESTED))
                    {
                        if (GameObject* pGO = bot->GetMap()->GetGameObject(bg->GetSingleGameObjectGuid(objective.first, defender ? BG_AB_NODE_STATUS_HORDE_CONTESTED : BG_AB_NODE_STATUS_HORDE_OCCUPIED)))
                        {
                            float const distance = sqrt(bot->GetDistance(pGO));
                            if (attackObjectiveDistance > distance)
                            {
                                // do not pick if already in list
                                vector<GameObject*>::iterator f = find(objectives.begin(), objectives.end(), pGO);
                                if (f != objectives.end())
                                    continue;

                                objectives.push_back(pGO);
                                //pAttackObjectiveObject = pGO;
                                attackObjectiveDistance = distance;
                                ostringstream out; out << "Possible Attack Point #" << objective.first;
                                bot->Say(out.str(), LANG_UNIVERSAL);
                            }
                        }
                    }
                }
            }
            if (!objectives.empty())
            {
                // pick random objective
                BgObjective = objectives[urand(0, objectives.size() - 1)];
            }
        }
        if (BgObjective)
        {
            pos.Set(BgObjective->GetPositionX(), BgObjective->GetPositionY(), BgObjective->GetPositionZ(), BgObjective->GetMapId());
            posMap["bg objective"] = pos;
            ostringstream out;
            out << "BG objective set to " << BgObjective->GetName() << " " << pos.x << " " << pos.y;
            bot->Say(out.str(), LANG_UNIVERSAL);
            return true;
        }
        break;
    }
    }

    return false;
}

bool BGTactics::moveToObjective()
{
    BattleGround *bg = bot->GetBattleGround();
    if (!bg)
        return false;

    ai::PositionEntry pos = context->GetValue<ai::PositionMap&>("position")->Get()["bg objective"];
    if (!pos.isSet())
        return selectObjective();
    else
    {
        if (sServerFacade.IsDistanceGreaterThan(sServerFacade.GetDistance2d(bot, pos.x, pos.y), VISIBILITY_DISTANCE_NORMAL))
        {
            //ostringstream out; out << "It is too far away! " << pos.x << ", " << pos.y << ", Distance: " << sServerFacade.GetDistance2d(bot, pos.x, pos.y);
            //bot->Say(out.str(), LANG_UNIVERSAL);
            return false;
        }

        // don't try to move if already close
        if (sqrt(bot->GetDistance(pos.x, pos.y, pos.z, DIST_CALC_NONE)) < 2.0f)
        {
            return true;
        }

        //ostringstream out; out << "Moving to objective " << pos.x << ", " << pos.y << ", Distance: " << sServerFacade.GetDistance2d(bot, pos.x, pos.y);
        //bot->Say(out.str(), LANG_UNIVERSAL);

        // more precise position for wsg
        if (bg->GetTypeId() == BATTLEGROUND_WS)
            return MoveTo(bot->GetMapId(), pos.x, pos.y, pos.z);
        else
            return MoveNear(bot->GetMapId(), pos.x, pos.y, pos.z, 4.0f);
    }
    return false;
}

bool BGTactics::selectObjectiveWp(std::vector<BattleBotPath*> const& vPaths)
{
    ai::PositionMap& posMap = context->GetValue<ai::PositionMap&>("position")->Get();
    ai::PositionEntry pos = context->GetValue<ai::PositionMap&>("position")->Get()["bg objective"];
    if (!pos.isSet())
        return false;

    BattleBotPath* pClosestPath = nullptr;
    uint32 closestPoint = 0;
    float closestDistanceToTarget = FLT_MAX;
    bool reverse = false;

    for (const auto& pPath : vPaths)
    {
        {
            BattleBotWaypoint& lastPoint = ((*pPath)[pPath->size() - 1]);
            float const distanceFromPathEndToTarget = sqrt(Position(pos.x, pos.y, pos.z, 0.f).GetDistance(Position(lastPoint.x, lastPoint.y, lastPoint.z, 0.f)));
            if (closestDistanceToTarget > distanceFromPathEndToTarget)
            {
                float closestDistanceFromMeToPoint = FLT_MAX;

                for (uint32 i = 0; i < pPath->size(); i++)
                {
                    BattleBotWaypoint& waypoint = ((*pPath)[i]);
                    float const distanceFromMeToPoint = sqrt(bot->GetDistance(waypoint.x, waypoint.y, waypoint.z, DIST_CALC_NONE));
                    if (distanceFromMeToPoint < 50.0f && closestDistanceFromMeToPoint > distanceFromMeToPoint)
                    {
                        reverse = false;
                        pClosestPath = pPath;
                        closestPoint = i;
                        closestDistanceToTarget = distanceFromPathEndToTarget;
                        closestDistanceFromMeToPoint = distanceFromMeToPoint;
                    }
                }
            }
        }

        if (std::find(vPaths_NoReverseAllowed.begin(), vPaths_NoReverseAllowed.end(), pPath) != vPaths_NoReverseAllowed.end())
            continue;

        {
            BattleBotWaypoint& firstPoint = ((*pPath)[0]);
            float const distanceFromPathBeginToTarget = sqrt(Position(pos.x, pos.y, pos.z, 0).GetDistance(Position(firstPoint.x, firstPoint.y, firstPoint.z, 0.f)));
            if (closestDistanceToTarget > distanceFromPathBeginToTarget)
            {
                float closestDistanceFromMeToPoint = FLT_MAX;

                for (uint32 i = 0; i < pPath->size(); i++)
                {
                    BattleBotWaypoint& waypoint = ((*pPath)[i]);
                    float const distanceFromMeToPoint = sqrt(bot->GetDistance(waypoint.x, waypoint.y, waypoint.z, DIST_CALC_NONE));
                    if (distanceFromMeToPoint < 50.0f && closestDistanceFromMeToPoint > distanceFromMeToPoint)
                    {
                        reverse = true;
                        pClosestPath = pPath;
                        closestPoint = i;
                        closestDistanceToTarget = distanceFromPathBeginToTarget;
                        closestDistanceFromMeToPoint = distanceFromMeToPoint;
                    }
                }
            }
        }
    }

    if (!pClosestPath)
        return false;

    // Prevent picking last point of path.
    // It means we are already there.
    if (reverse)
    {
        if (closestPoint == 0)
            return false;
    }
    else
    {
        if (closestPoint == pClosestPath->size() - 1)
            return false;
    }

    BattleBotPath* currentPath = pClosestPath;
    uint32 currentPoint = reverse ? closestPoint + 1 : closestPoint - 1;

    return moveToObjectiveWp(currentPath, currentPoint, reverse);

    return false;
}

bool BGTactics::resetObjective()
{
    // sometimes change priority
    if (!urand(0, 4))
        context->GetValue<uint32>("bg role")->Set(urand(0, 9));

    ai::PositionMap& posMap = context->GetValue<ai::PositionMap&>("position")->Get();
    ai::PositionEntry pos = context->GetValue<ai::PositionMap&>("position")->Get()["bg objective"];
    pos.Reset();
    posMap["bg objective"] = pos;

    return selectObjective(true);
}

bool BGTactics::moveToObjectiveWp(BattleBotPath* const& currentPath, uint32 currentPoint, bool reverse)
{
    if (!currentPath)
        return false;

    uint32 const lastPointInPath = reverse ? 0 : ((*currentPath).size() - 1);

    if ((currentPoint == lastPointInPath) ||
        (bot->IsInCombat() && !(bot->HasAura(BG_WS_SPELL_WARSONG_FLAG) || bot->HasAura(BG_WS_SPELL_SILVERWING_FLAG))) || !bot->IsAlive())
    {
        // Path is over.
        ostringstream out; out << "Reached path end!";
        bot->Say(out.str(), LANG_UNIVERSAL);
        resetObjective();
        return false;
    }

    uint32 currPoint = currentPoint;

    if (reverse)
        currPoint--;
    else
        currPoint++;

    uint32 nPoint = reverse ? max((int)(currPoint - urand(1, 5)), 0) : min((uint32)(currPoint + urand(1, 5)), lastPointInPath);
    if (reverse && nPoint < 0)
        nPoint = 0;

    BattleBotWaypoint& nextPoint = currentPath->at(nPoint);

    //ostringstream out; out << "WP: ";
    //reverse ? out << currPoint << " <<< -> " << nPoint : out << currPoint << ">>> ->" << nPoint;
    //out << ", " << nextPoint.x << ", " << nextPoint.y << " Path Size: " << currentPath->size() << ", Dist: " << sServerFacade.GetDistance2d(bot, nextPoint.x, nextPoint.y);
    //bot->Say(out.str(), LANG_UNIVERSAL);

    return MoveTo(bot->GetMapId(), nextPoint.x + frand(-2, 2), nextPoint.y + frand(-2, 2), nextPoint.z);
}

bool BGTactics::startNewPathBegin(std::vector<BattleBotPath*> const& vPaths)
{
    struct AvailablePath
    {
        AvailablePath(BattleBotPath* pPath_, bool reverse_) : pPath(pPath_), reverse(reverse_) {}
        BattleBotPath* pPath = nullptr;
        bool reverse = false;
    };
    std::vector<AvailablePath> availablePaths;

    for (const auto& pPath : vPaths)
    {
        BattleBotWaypoint* pStart = &((*pPath)[0]);
        if (sqrt(bot->GetDistance(pStart->x, pStart->y, pStart->z, DIST_CALC_NONE) < INTERACTION_DISTANCE))
            availablePaths.emplace_back(AvailablePath(pPath, false));

        // Some paths are not allowed backwards.
        if (std::find(vPaths_NoReverseAllowed.begin(), vPaths_NoReverseAllowed.end(), pPath) != vPaths_NoReverseAllowed.end())
            continue;

        BattleBotWaypoint* pEnd = &((*pPath)[(*pPath).size() - 1]);
        if (sqrt(bot->GetDistance(pEnd->x, pEnd->y, pEnd->z, DIST_CALC_NONE) < INTERACTION_DISTANCE))
            availablePaths.emplace_back(AvailablePath(pPath, true));
    }

    if (availablePaths.empty())
        return false;

    uint32 randomPath = urand(0, availablePaths.size() - 1);
    AvailablePath const* chosenPath = &availablePaths[randomPath];

    BattleBotPath* currentPath = chosenPath->pPath;
    bool reverse = chosenPath->reverse;
    uint32 currentPoint = reverse ? currentPath->size() - 1 : 0;

    return moveToObjectiveWp(currentPath, currentPoint, reverse);
}

bool BGTactics::startNewPathFree(std::vector<BattleBotPath*> const& vPaths)
{
    BattleBotPath* pClosestPath = nullptr;
    uint32 closestPoint = 0;
    float closestDistance = FLT_MAX;

    for (const auto& pPath : vPaths)
    {
        for (uint32 i = 0; i < pPath->size(); i++)
        {
            BattleBotWaypoint& waypoint = ((*pPath)[i]);
            float const distanceToPoint = sqrt(bot->GetDistance(waypoint.x, waypoint.y, waypoint.z, DIST_CALC_NONE));
            if (distanceToPoint < closestDistance)
            {
                pClosestPath = pPath;
                closestPoint = i;
                closestDistance = distanceToPoint;
            }
        }
    }

    if (!pClosestPath)
        return false;

    BattleBotPath* currentPath = pClosestPath;
    bool reverse = false;
    uint32 currentPoint = closestPoint - 1;

    return moveToObjectiveWp(currentPath, currentPoint, reverse);
}

bool BGTactics::atFlag(std::vector<BattleBotPath*> const& vPaths, std::vector<uint32> const& vFlagIds)
{
    BattleGround *bg = bot->GetBattleGround();
    if (!bg)
        return false;

    list<ObjectGuid> closeObjects;
    list<ObjectGuid> closePlayers;
    float flagRange;

    switch (bg->GetTypeId())
    {
    case BATTLEGROUND_AV:
    case BATTLEGROUND_AB:
    {
        closeObjects = *context->GetValue<list<ObjectGuid> >("closest game objects");
        closePlayers = *context->GetValue<list<ObjectGuid> >("closest friendly players");
        flagRange = INTERACTION_DISTANCE;
    }
    break;
    case BATTLEGROUND_WS:
    {
        closeObjects = *context->GetValue<list<ObjectGuid> >("nearest game objects no los");
        flagRange = VISIBILITY_DISTANCE_TINY;
    }
    break;
    }

    if (closeObjects.empty())
        return false;

    if (!closePlayers.empty())
    {
        for (auto & guid : closePlayers)
        {
            Unit* pFriend = ai->GetUnit(guid);
            if (pFriend->GetCurrentSpell(CURRENT_GENERIC_SPELL) &&
                pFriend->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_CAPTURE_BANNER)
            {
                resetObjective();
                startNewPathBegin(vPaths);
                return false;
            }
        }
    }

    //ostringstream out; out << "Found " << closeObjects.size() << " nearby objects";
    //bot->Say(out.str(), LANG_UNIVERSAL);

    for (list<ObjectGuid>::iterator i = closeObjects.begin(); i != closeObjects.end(); ++i)
    {
        GameObject* go = ai->GetGameObject(*i);
        if (!go)
            continue;

        vector<uint32>::const_iterator f = find(vFlagIds.begin(), vFlagIds.end(), go->GetEntry());
        if (f == vFlagIds.end())
            continue;

        if (!sServerFacade.isSpawned(go) || go->IsInUse() || !go->GetGoState() == GO_STATE_READY)
            continue;

        if (!bot->CanInteract(go) && bg->GetTypeId() != BATTLEGROUND_WS)
            continue;
        
        if (flagRange)
            if (!bot->IsWithinDistInMap(go, flagRange))
                continue;

        bool atBase = go->GetEntry() == vFlagsWS[GetTeamIndexByTeamId(bot->GetTeam())];
        if (atBase && !(bot->HasAura(BG_WS_SPELL_WARSONG_FLAG) || bot->HasAura(BG_WS_SPELL_SILVERWING_FLAG)))
            continue;

        switch (bg->GetTypeId())
        {
        case BATTLEGROUND_AV:
        case BATTLEGROUND_AB:
        {
            if (bot->IsMounted())
                bot->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

            if (bot->IsInDisallowedMountForm())
                bot->RemoveSpellsCausingAura(SPELL_AURA_MOD_SHAPESHIFT);

            ostringstream out; out << "Flag is nearby, using " << go->GetName();
            bot->Say(out.str(), LANG_UNIVERSAL);
            ai->SetNextCheckDelay(10000);

            WorldPacket data(CMSG_GAMEOBJ_USE);
            data << go->GetObjectGuid();
            bot->GetSession()->HandleGameObjectUseOpcode(data);
            resetObjective();
            return true;
            break;
        }
        case BATTLEGROUND_WS:
        {
            if (bot->IsWithinDistInMap(go, INTERACTION_DISTANCE))
            {
                if (atBase)
                {
                    if (bot->GetTeam() == HORDE)
                    {
                        WorldPacket data(CMSG_AREATRIGGER);
                        data << uint32(WS_AT_WARSONG_ROOM);
                        bot->GetSession()->HandleAreaTriggerOpcode(data);
                    }
                    else
                    {
                        WorldPacket data(CMSG_AREATRIGGER);
                        data << uint32(WS_AT_SILVERWING_ROOM);
                        bot->GetSession()->HandleAreaTriggerOpcode(data);
                    }
                    ostringstream out; out << "Capturing flag!";
                    bot->Say(out.str(), LANG_UNIVERSAL);
                    return true;
                }

                if (bot->IsMounted())
                    bot->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

                if (bot->IsInDisallowedMountForm())
                    bot->RemoveSpellsCausingAura(SPELL_AURA_MOD_SHAPESHIFT);

                ostringstream out; out << "Flag is nearby, using " << go->GetName();
                bot->Say(out.str(), LANG_UNIVERSAL);
                WorldPacket data(CMSG_GAMEOBJ_USE);
                data << go->GetObjectGuid();
                bot->GetSession()->HandleGameObjectUseOpcode(data);

                resetObjective();
                return true;
            }
            else
            {
                ostringstream out; out << "Flag is far, moving to " << go->GetName() << " " << go->GetPositionX() << " " << go->GetPositionY() << " Distance:" << sServerFacade.GetDistance2d(bot, go->GetPositionX(), go->GetPositionY());
                bot->Say(out.str(), LANG_UNIVERSAL);
                return MoveTo(bot->GetMapId(), go->GetPositionX(), go->GetPositionY(), go->GetPositionZ());
            }
            break;
        }
        }
    }

    return false;
}

bool BGTactics::flagTaken()
{
    BattleGroundWS* bg = (BattleGroundWS *)bot->GetBattleGround();
    if (!bg)
        return false;

    return !bg->GetFlagCarrierGuid(GetTeamIndexByTeamId(bg->GetOtherTeam(bot->GetTeam()))).IsEmpty();
}

bool BGTactics::teamFlagTaken()
{
    BattleGroundWS* bg = (BattleGroundWS *)bot->GetBattleGround();
    if (!bg)
        return false;

    return !bg->GetFlagCarrierGuid(GetTeamIndexByTeamId(bot->GetTeam())).IsEmpty();
}

bool BGTactics::protectFC()
{
    BattleGround *bg = bot->GetBattleGround();
    if (!bg)
        return false;

    Unit* teamFC = AI_VALUE(Unit*, "team flag carrier");
    if (teamFC && bot->IsWithinDistInMap(teamFC, VISIBILITY_DISTANCE_SMALL))
        return Follow(teamFC);

    return false;
}

bool BGTactics::useBuff()
{
    list<ObjectGuid> closeObjects = AI_VALUE(list<ObjectGuid>, "nearest game objects no los");

    if (closeObjects.empty())
        return false;

    bool needRegen = bot->GetHealthPercent() < sPlayerbotAIConfig.lowHealth || (AI_VALUE2(bool, "has mana", "self target") && AI_VALUE2(uint8, "mana", "self target") < sPlayerbotAIConfig.lowMana);
    bool needSpeed = (bot->HasAura(BG_WS_SPELL_WARSONG_FLAG) || bot->HasAura(BG_WS_SPELL_SILVERWING_FLAG)) || !(teamFlagTaken() || flagTaken());
    bool foundBuff = false;

    for (list<ObjectGuid>::iterator i = closeObjects.begin(); i != closeObjects.end(); ++i)
    {
        GameObject* go = ai->GetGameObject(*i);
        if (!go)
            continue;

        if (!sServerFacade.isSpawned(go))
            continue;

        // use speed buff only if close
        if (sServerFacade.IsDistanceGreaterThan(sServerFacade.GetDistance2d(bot, go), go->GetEntry() == Buff_Entries[0] ? 20.0f : VISIBILITY_DISTANCE_SMALL))
            continue;

        if (needSpeed && go->GetEntry() == Buff_Entries[0])
            foundBuff = true;

        if (needRegen && go->GetEntry() == Buff_Entries[1])
            foundBuff = true;

        // do not move to Berserk buff if bot is healer or has flag
        if (!(bot->HasAura(BG_WS_SPELL_WARSONG_FLAG) || bot->HasAura(BG_WS_SPELL_SILVERWING_FLAG)) && !ai->IsHeal(bot) && go->GetEntry() == Buff_Entries[2])
            foundBuff = true;

        if (foundBuff)
        {
            ostringstream out; out << "Moving to buff...";
            bot->Say(out.str(), LANG_UNIVERSAL);

            return MoveTo(go->GetMapId(), go->GetPositionX(), go->GetPositionY(), go->GetPositionZ());
        }
    }
    return false;
}
