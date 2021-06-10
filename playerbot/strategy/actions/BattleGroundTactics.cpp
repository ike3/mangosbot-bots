#include "botpch.h"
#include "../../playerbot.h"
#include "strategy/values/PositionValue.h"
#include "MovementActions.h"
#include "BattleGround.h"
#include "BattleGroundMgr.h"
#include "BattlegroundTactics.h"

using namespace ai;

// common bg positions
Position const WS_WAITING_POS_HORDE_1 = { 944.981f, 1423.478f, 345.434f, 6.18f };
Position const WS_WAITING_POS_HORDE_2 = { 948.488f, 1459.834f, 343.066f, 6.27f };
Position const WS_WAITING_POS_ALLIANCE_1 = { 1510.502f, 1493.385f, 351.995f, 3.1f };
Position const WS_WAITING_POS_ALLIANCE_2 = { 1496.578f, 1457.900f, 344.442f, 3.1f };
Position const WS_FLAG_POS_HORDE = { 915.958f, 1433.925f, 346.193f, 0.0f };
Position const WS_FLAG_POS_ALLIANCE = { 1539.219f, 1481.747f, 352.458f, 0.0f };
Position const WS_FLAG_HIDE_HORDE_1 = { 928.178f, 1458.432f, 346.889f, 4.8f };
Position const WS_FLAG_HIDE_HORDE_2 = { 914.954f, 1439.540f, 346.415f, 4.7f };
Position const WS_FLAG_HIDE_HORDE_3 = { 1163.820f, 1373.625f, 312.23f, 4.7f };
Position const WS_FLAG_HIDE_ALLIANCE_1 = { 1529.249f, 1456.470f, 353.04f, 1.25f };
Position const WS_FLAG_HIDE_ALLIANCE_2 = { 1540.286f, 1476.026f, 352.692f, 2.91f };
Position const WS_FLAG_HIDE_ALLIANCE_3 = { 1495.807f, 1466.774f, 352.350f, 1.50f };
std::vector<Position> const WS_FLAG_HIDE_HORDE = { WS_FLAG_HIDE_HORDE_1 , WS_FLAG_HIDE_HORDE_2, WS_FLAG_HIDE_HORDE_3 };
std::vector<Position> const WS_FLAG_HIDE_ALLIANCE = { WS_FLAG_HIDE_ALLIANCE_1 , WS_FLAG_HIDE_ALLIANCE_2, WS_FLAG_HIDE_ALLIANCE_3 };
Position const AB_WAITING_POS_HORDE = { 702.884f, 703.045f, -16.115f, 0.77f };
Position const AB_WAITING_POS_ALLIANCE = { 1286.054f, 1282.500f, -15.697f, 3.95f };
Position const AV_WAITING_POS_ALLIANCE = { 793.627f, -493.814f, 99.689f, 3.09f };
#ifdef MANGOSBOT_ZERO
Position const AV_WAITING_POS_HORDE = { -871.196f, -558.407f, 57.239f, 2.14f };
#else
Position const AV_WAITING_POS_HORDE = { -1381.865f, -544.872f, 54.773f, 0.76f };
#endif
Position const AV_ICEBLOOD_GARRISON_WAITING_ALLIANCE = { -492.17f, -187.077f, 57.1342f, 2.77f };
Position const AV_STONEHEARTH_WAITING_HORDE = { 28.1264f, -302.593f, 15.076f, 2.96f };

enum BattleBotWsgWaitSpot
{
    BB_WSG_WAIT_SPOT_SPAWN,
    BB_WSG_WAIT_SPOT_LEFT,
    BB_WSG_WAIT_SPOT_RIGHT
};

std::vector<uint32> const vFlagsAV = { BG_AV_GO_GY_BANNER_HORDE , BG_AV_GO_BANNER_HORDE , BG_AV_GO_GY_BANNER_ALLIANCE , BG_AV_GO_BANNER_ALLIANCE ,
                                       BG_AV_GO_BANNER_ALLIANCE_CONT , BG_AV_GO_GY_BANNER_ALLIANCE_CONT , BG_AV_GO_GY_BANNER_HORDE_CONT ,
                                       BG_AV_GO_BANNER_HORDE_CONT , BG_AV_GO_GY_BANNER_SNOWFALL };

std::vector<uint32> const vFlagsAB = { BG_AB_BANNER_ALLIANCE , BG_AB_BANNER_CONTESTED_A , BG_AB_BANNER_HORDE , BG_AB_BANNER_CONTESTED_H ,
                                       BG_AB_BANNER_STABLE, BG_AB_BANNER_BLACKSMITH, BG_AB_BANNER_FARM, BG_AB_BANNER_LUMBER_MILL,
                                       BG_AB_BANNER_MINE };

std::vector<uint32> const vFlagsWS = { GO_WS_SILVERWING_FLAG, GO_WS_WARSONG_FLAG, GO_WS_SILVERWING_FLAG_DROP, GO_WS_WARSONG_FLAG_DROP };

// BG Waypoints (vmangos)

// Horde Flag Room to Horde Graveyard
BattleBotPath vPath_WSG_HordeFlagRoom_to_HordeGraveyard =
{
    { 933.331f, 1433.72f, 345.536f, nullptr },
    { 944.859f, 1423.05f, 345.437f, nullptr },
    { 966.691f, 1422.53f, 345.223f, nullptr },
    { 979.588f, 1422.84f, 345.46f, nullptr },
    { 997.806f, 1422.52f, 344.623f, nullptr },
    { 1008.53f, 1417.02f, 343.206f, nullptr },
    { 1016.42f, 1402.33f, 341.352f, nullptr },
    { 1029.14f, 1387.49f, 340.836f, nullptr },
};
// Horde Graveyard to Horde Tunnel
BattleBotPath vPath_WSG_HordeGraveyard_to_HordeTunnel =
{
    { 1029.14f, 1387.49f, 340.836f, nullptr },
    { 1034.95f, 1392.62f, 340.856f, nullptr },
    { 1038.21f, 1406.43f, 341.562f, nullptr },
    { 1043.87f, 1426.9f, 339.197f, nullptr },
    { 1054.53f, 1441.47f, 339.725f, nullptr },
    { 1056.33f, 1456.03f, 341.463f, nullptr },
    { 1057.39f, 1469.98f, 342.148f, nullptr },
    { 1057.67f, 1487.55f, 342.537f, nullptr },
    { 1048.7f, 1505.37f, 341.117f, nullptr },
    { 1042.19f, 1521.69f, 338.003f, nullptr },
    { 1050.01f, 1538.22f, 332.43f, nullptr },
    { 1068.15f, 1548.1f, 321.446f, nullptr },
    { 1088.14f, 1538.45f, 316.398f, nullptr },
    { 1101.26f, 1522.79f, 314.918f, nullptr },
    { 1114.67f, 1503.18f, 312.947f, nullptr },
    { 1126.45f, 1487.4f, 314.136f, nullptr },
    { 1124.37f, 1462.28f, 315.853f, nullptr },
};
// Horde Tunnel to Horde Flag Room
BattleBotPath vPath_WSG_HordeTunnel_to_HordeFlagRoom =
{
    { 1124.37f, 1462.28f, 315.853f, nullptr },
    { 1106.87f, 1462.13f, 316.558f, nullptr },
    { 1089.44f, 1461.04f, 316.332f, nullptr },
    { 1072.07f, 1459.46f, 317.449f, nullptr },
    { 1051.09f, 1459.89f, 323.126f, nullptr },
    { 1030.1f, 1459.58f, 330.204f, nullptr },
    { 1010.76f, 1457.49f, 334.896f, nullptr },
    { 1005.47f, 1448.19f, 335.864f, nullptr },
    { 999.974f, 1458.49f, 335.632f, nullptr },
    { 982.632f, 1459.18f, 336.127f, nullptr },
    { 965.049f, 1459.15f, 338.076f, nullptr },
    { 944.526f, 1459.0f, 344.207f, nullptr },
    { 937.479f, 1451.12f, 345.553f, nullptr },
    { 933.331f, 1433.72f, 345.536f, nullptr },
};
// Horde Tunnel to Alliance Tunnel 1
BattleBotPath vPath_WSG_HordeTunnel_to_AllianceTunnel_1 =
{
    { 1124.37f, 1462.28f, 315.853f, nullptr },
    { 1135.07f, 1462.43f, 315.569f, nullptr },
    { 1152.2f, 1465.51f, 311.056f, nullptr },
    { 1172.62f, 1470.34f, 306.812f, nullptr },
    { 1193.1f, 1475.0f, 305.155f, nullptr },
    { 1212.99f, 1477.94f, 306.929f, nullptr },
    { 1233.88f, 1476.29f, 308.015f, nullptr },
    { 1250.52f, 1470.94f, 309.8f, nullptr },
    { 1266.09f, 1465.75f, 312.242f, nullptr },
    { 1283.31f, 1463.55f, 311.819f, nullptr },
    { 1297.11f, 1461.2f, 315.485f, nullptr },
    { 1314.31f, 1460.76f, 317.926f, nullptr },
    { 1329.8f, 1461.24f, 320.267f, nullptr },
    { 1348.02f, 1461.06f, 323.167f, nullptr },
};
// Horde Tunnel to Alliance Tunnel 2
BattleBotPath vPath_WSG_HordeTunnel_to_AllianceTunnel_2 =
{
    { 1124.37f, 1462.28f, 315.853f, nullptr },
    { 1138.61f, 1452.12f, 312.988f, nullptr },
    { 1154.35f, 1442.42f, 310.728f, nullptr },
    { 1171.29f, 1438.04f, 307.462f, nullptr },
    { 1185.03f, 1435.43f, 309.484f, nullptr },
    { 1202.24f, 1432.26f, 310.193f, nullptr },
    { 1219.48f, 1429.2f, 310.301f, nullptr },
    { 1235.94f, 1429.97f, 309.727f, nullptr },
    { 1249.3f, 1434.12f, 312.37f, nullptr },
    { 1265.88f, 1439.71f, 314.373f, nullptr },
    { 1282.87f, 1443.85f, 314.907f, nullptr },
    { 1300.06f, 1447.16f, 316.737f, nullptr },
    { 1313.79f, 1449.86f, 317.651f, nullptr },
    { 1329.76f, 1457.36f, 320.37f, nullptr },
    { 1348.02f, 1461.06f, 323.167f, nullptr },
};
// Horde GY Jump to Horde Tunnel
BattleBotPath vPath_WSG_HordeGYJump_to_HordeTunnel =
{
    { 1077.87f, 1400.27f, 323.153f, nullptr },
    { 1088.42f, 1402.68f, 319.605f, nullptr },
    { 1104.34f, 1409.4f, 315.304f, nullptr },
    { 1115.4f, 1418.91f, 313.772f, nullptr },
    { 1122.83f, 1430.74f, 312.765f, nullptr },
    { 1125.26f, 1442.56f, 313.996f, nullptr },
    { 1124.37f, 1462.28f, 315.853f, nullptr },
};
// Horde GY Jump to Alliance Tunnel
BattleBotPath vPath_WSG_HordeGYJump_to_AllianceTunnel =
{
    { 1077.87f, 1400.27f, 323.153f, nullptr },
    { 1091.57f, 1397.37f, 317.739f, nullptr },
    { 1113.14f, 1398.07f, 314.937f, nullptr },
    { 1133.88f, 1401.36f, 314.333f, nullptr },
    { 1151.25f, 1403.39f, 310.679f, nullptr },
    { 1172.17f, 1405.13f, 308.046f, nullptr },
    { 1192.63f, 1409.01f, 306.914f, nullptr },
    { 1212.59f, 1415.38f, 308.805f, nullptr },
    { 1228.5f, 1422.68f, 309.404f, nullptr },
    { 1242.89f, 1431.01f, 310.664f, nullptr },
    { 1259.33f, 1436.99f, 314.488f, nullptr },
    { 1276.1f, 1442.0f, 314.162f, nullptr },
    { 1299.13f, 1450.26f, 317.148f, nullptr },
    { 1315.54f, 1456.24f, 318.449f, nullptr },
    { 1330.63f, 1460.27f, 320.435f, nullptr },
    { 1348.02f, 1461.06f, 323.167f, nullptr },
};
// Alliance Flag Room to Alliance Graveyard
BattleBotPath vPath_WSG_AllianceFlagRoom_to_AllianceGraveyard =
{
    { 1519.53f, 1481.87f, 352.024f, nullptr },
    { 1508.27f, 1493.17f, 352.005f, nullptr },
    { 1490.78f, 1493.51f, 352.141f, nullptr },
    { 1469.79f, 1494.13f, 351.774f, nullptr },
    { 1453.65f, 1494.39f, 350.614f, nullptr },
    { 1443.51f, 1501.75f, 348.317f, nullptr },
    { 1443.33f, 1517.78f, 345.534f, nullptr },
    { 1443.55f, 1533.4f, 343.148f, nullptr },
    { 1441.47f, 1548.12f, 342.752f, nullptr },
    { 1433.79f, 1552.67f, 342.763f, nullptr },
    { 1422.88f, 1552.37f, 342.751f, nullptr },
    { 1415.33f, 1554.79f, 343.156f, nullptr },
};
// Alliance Graveyard to Alliance Tunnel
BattleBotPath vPath_WSG_AllianceGraveyard_to_AllianceTunnel =
{
    { 1415.33f, 1554.79f, 343.156f, nullptr },
    { 1428.29f, 1551.79f, 342.751f, nullptr },
    { 1441.51f, 1545.79f, 342.757f, nullptr },
    { 1441.15f, 1530.35f, 343.712f, nullptr },
    { 1435.53f, 1517.29f, 346.698f, nullptr },
    { 1424.81f, 1499.24f, 349.486f, nullptr },
    { 1416.31f, 1483.94f, 348.536f, nullptr },
    { 1408.83f, 1468.4f, 347.648f, nullptr },
    { 1404.64f, 1449.79f, 347.279f, nullptr },
    { 1405.34f, 1432.33f, 345.792f, nullptr },
    { 1406.38f, 1416.18f, 344.755f, nullptr },
    { 1400.22f, 1401.87f, 340.496f, nullptr },
    { 1385.96f, 1394.15f, 333.829f, nullptr },
    { 1372.38f, 1390.75f, 328.722f, nullptr },
    { 1362.93f, 1390.02f, 327.034f, nullptr },
    { 1357.91f, 1398.07f, 325.674f, nullptr },
    { 1354.17f, 1411.56f, 324.327f, nullptr },
    { 1351.44f, 1430.38f, 323.506f, nullptr },
    { 1350.36f, 1444.43f, 323.388f, nullptr },
    { 1348.02f, 1461.06f, 323.167f, nullptr },
};
// Alliance Tunnel to Alliance Flag Room
BattleBotPath vPath_WSG_AllianceTunnel_to_AllianceFlagRoom =
{
    { 1348.02f, 1461.06f, 323.167f, nullptr },
    { 1359.8f, 1461.49f, 324.527f, nullptr },
    { 1372.47f, 1461.61f, 324.354f, nullptr },
    { 1389.08f, 1461.12f, 325.913f, nullptr },
    { 1406.57f, 1460.48f, 330.615f, nullptr },
    { 1424.04f, 1459.57f, 336.029f, nullptr },
    { 1442.5f, 1459.7f, 342.024f, nullptr },
    { 1449.59f, 1469.14f, 342.65f, nullptr },
    { 1458.03f, 1458.43f, 342.746f, nullptr },
    { 1469.4f, 1458.14f, 342.794f, nullptr },
    { 1489.06f, 1457.86f, 342.794f, nullptr },
    { 1502.27f, 1457.52f, 347.589f, nullptr },
    { 1512.87f, 1457.81f, 352.039f, nullptr },
    { 1517.53f, 1468.79f, 352.033f, nullptr },
    { 1519.53f, 1481.87f, 352.024f, nullptr },
};
// Alliance GY Jump to Alliance Tunnel
BattleBotPath vPath_WSG_AllianceGYJump_to_AllianceTunnel =
{
    { 1387.85f, 1538.06f, 321.854f, nullptr },
    { 1376.87f, 1529.48f, 321.66f, nullptr },
    { 1369.76f, 1521.76f, 318.544f, nullptr },
    { 1360.97f, 1508.68f, 320.007f, nullptr },
    { 1355.78f, 1495.7f, 323.959f, nullptr },
    { 1351.58f, 1482.36f, 324.189f, nullptr },
    { 1348.02f, 1461.06f, 323.167f, nullptr },
};
// Alliance GY Jump to Horde Tunnel
BattleBotPath vPath_WSG_AllianceGYJump_to_HordeTunnel =
{
    { 1387.85f, 1538.06f, 321.855f, nullptr },
    { 1377.58f, 1535.88f, 321.053f, nullptr },
    { 1363.98f, 1532.59f, 319.913f, nullptr },
    { 1353.94f, 1529.5f, 316.643f, nullptr },
    { 1340.71f, 1524.94f, 315.246f, nullptr },
    { 1330.75f, 1521.6f, 314.868f, nullptr },
    { 1320.73f, 1518.48f, 316.097f, nullptr },
    { 1307.28f, 1514.6f, 318.134f, nullptr },
    { 1297.12f, 1511.95f, 318.073f, nullptr },
    { 1283.61f, 1508.28f, 316.707f, nullptr },
    { 1273.51f, 1505.39f, 314.615f, nullptr },
    { 1263.49f, 1502.27f, 311.343f, nullptr },
    { 1250.22f, 1497.81f, 309.106f, nullptr },
    { 1237.31f, 1492.4f, 307.577f, nullptr },
    { 1224.04f, 1487.97f, 306.302f, nullptr },
    { 1213.89f, 1485.29f, 305.739f, nullptr },
    { 1203.69f, 1482.8f, 306.177f, nullptr },
    { 1190.05f, 1479.62f, 303.89f, nullptr },
    { 1179.83f, 1477.22f, 303.686f, nullptr },
    { 1169.65f, 1474.65f, 305.842f, nullptr },
    { 1156.05f, 1471.33f, 310.002f, nullptr },
    { 1142.54f, 1467.68f, 311.727f, nullptr },
    { 1135.4f, 1465.54f, 315.622f, nullptr },
    { 1124.37f, 1462.28f, 315.853f, nullptr },
};
// Horde GY Jump to Alliance Flag Room through Side Entrance
BattleBotPath vPath_WSG_HordeGYJump_to_AllianceFlagRoom =
{
    { 1077.87f, 1400.27f, 323.153f, nullptr },
    { 1084.45f, 1388.76f, 319.724f, nullptr },
    { 1088.27f, 1371.39f, 319.17f, nullptr },
    { 1090.71f, 1350.54f, 316.097f, nullptr },
    { 1098.71f, 1332.2f, 317.792f, nullptr },
    { 1109.45f, 1320.92f, 318.267f, nullptr },
    { 1123.49f, 1311.38f, 317.472f, nullptr },
    { 1145.46f, 1302.64f, 317.741f, nullptr },
    { 1168.4f, 1288.85f, 318.053f, nullptr },
    { 1186.49f, 1284.27f, 316.972f, nullptr },
    { 1199.4f, 1286.83f, 317.377f, nullptr },
    { 1215.89f, 1304.02f, 312.822f, nullptr },
    { 1232.18f, 1324.73f, 312.345f, nullptr },
    { 1247.16f, 1329.97f, 315.095f, nullptr },
    { 1269.9f, 1335.18f, 311.879f, nullptr },
    { 1289.97f, 1341.28f, 318.625f, nullptr },
    { 1305.99f, 1347.63f, 321.123f, nullptr },
    { 1325.81f, 1361.58f, 319.39f, nullptr },
    { 1337.48f, 1371.68f, 318.706f, nullptr },
    { 1342.62f, 1390.03f, 321.435f, nullptr },
    { 1352.23f, 1397.97f, 325.547f, nullptr },
    { 1366.38f, 1385.61f, 328.196f, nullptr },
    { 1382.67f, 1380.56f, 332.371f, nullptr },
    { 1395.17f, 1393.12f, 336.183f, nullptr },
    { 1409.03f, 1411.5f, 344.626f, nullptr },
    { 1405.12f, 1438.81f, 346.533f, nullptr },
    { 1409.95f, 1460.93f, 347.687f, nullptr },
    { 1430.87f, 1461.08f, 353.992f, nullptr },
    { 1449.36f, 1459.44f, 358.499f, nullptr },
    { 1471.4f, 1458.48f, 362.557f, nullptr },
    { 1488.64f, 1464.01f, 362.454f, nullptr },
    { 1488.75f, 1474.6f, 358.79f, nullptr },
    { 1490.44f, 1485.99f, 352.112f, nullptr },
    { 1502.97f, 1493.87f, 352.199f, nullptr },
    { 1519.53f, 1481.87f, 352.024f, nullptr },
};
// Alliance GY Jump to Horde Flag Room through Side Entrance
BattleBotPath vPath_WSG_AllianceGYJump_to_HordeFlagRoom =
{
    { 1387.85f, 1538.06f, 321.855f, nullptr },
    { 1370.13f, 1549.33f, 321.122f, nullptr },
    { 1346.7f, 1564.64f, 316.708f, nullptr },
    { 1324.23f, 1574.24f, 317.11f, nullptr },
    { 1304.03f, 1576.06f, 314.625f, nullptr },
    { 1277.44f, 1569.2f, 312.201f, nullptr },
    { 1249.2f, 1555.53f, 309.172f, nullptr },
    { 1229.95f, 1558.21f, 306.936f, nullptr },
    { 1209.65f, 1573.56f, 308.95f, nullptr },
    { 1189.93f, 1587.73f, 309.608f, nullptr },
    { 1173.76f, 1592.66f, 309.805f, nullptr },
    { 1147.86f, 1590.75f, 310.37f, nullptr },
    { 1124.1f, 1579.89f, 314.881f, nullptr },
    //{ 1102.61f, 1573.98f, 315.804f, nullptr },
    { 1091.28f, 1558.56f, 316.451f, nullptr },
    { 1092.6f, 1547.71f, 316.709f, nullptr },
    { 1086.22f, 1541.5f, 316.924f, nullptr },
    { 1071.64f, 1548.25f, 319.88f, nullptr },
    { 1054.86f, 1544.78f, 328.415f, nullptr },
    { 1043.08f, 1528.67f, 336.984f, nullptr },
    { 1043.21f, 1512.43f, 339.099f, nullptr },
    { 1050.71f, 1485.48f, 342.852f, nullptr },
    { 1042.67f, 1461.07f, 342.305f, nullptr },
    { 1023.13f, 1457.49f, 345.535f, nullptr },
    { 992.797f, 1458.42f, 354.84f, nullptr },
    { 967.257f, 1458.84f, 356.131f, nullptr },
    { 964.566f, 1450.29f, 354.865f, nullptr },
    { 963.586f, 1432.46f, 345.206f, nullptr },
    { 953.017f, 1423.3f, 345.835f, nullptr },
    { 933.331f, 1433.72f, 345.536f, nullptr },
};
// Horde Tunnel to Horde Base Roof
BattleBotPath vPath_WSG_HordeTunnel_to_HordeBaseRoof =
{
    { 1124.37f, 1462.28f, 315.853f, nullptr },
    { 1106.87f, 1462.13f, 316.558f, nullptr },
    { 1089.44f, 1461.04f, 316.332f, nullptr },
    { 1072.07f, 1459.46f, 317.449f, nullptr },
    { 1051.09f, 1459.89f, 323.126f, nullptr },
    { 1030.1f, 1459.58f, 330.204f, nullptr },
    { 1010.76f, 1457.49f, 334.896f, nullptr },
    { 981.948f, 1459.07f, 336.154f, nullptr },
    { 981.768f, 1480.46f, 335.976f, nullptr },
    { 974.664f, 1495.9f, 340.837f, nullptr },
    { 964.661f, 1510.21f, 347.509f, nullptr },
    { 951.188f, 1520.99f, 356.377f, nullptr },
    { 937.37f, 1513.27f, 362.589f, nullptr },
    { 935.947f, 1499.58f, 364.199f, nullptr },
    { 935.9f, 1482.08f, 366.396f, nullptr },
    { 937.564f, 1462.81f, 367.287f, nullptr },
    { 945.871f, 1458.65f, 367.287f, nullptr },
    { 956.972f, 1459.48f, 367.291f, nullptr },
    { 968.317f, 1459.71f, 367.291f, nullptr },
    { 979.934f, 1454.58f, 367.078f, nullptr },
    { 979.99f, 1442.87f, 367.093f, nullptr },
    { 978.632f, 1430.71f, 367.125f, nullptr },
    { 970.395f, 1422.32f, 367.289f, nullptr },
    { 956.338f, 1425.09f, 367.293f, nullptr },
    { 952.778f, 1433.0f, 367.604f, nullptr },
    { 952.708f, 1445.01f, 367.604f, nullptr },
};
// Alliance Tunnel to Alliance Base Roof
BattleBotPath vPath_WSG_AllianceTunnel_to_AllianceBaseRoof =
{
    { 1348.02f, 1461.06f, 323.167f, nullptr },
    { 1359.8f, 1461.49f, 324.527f, nullptr },
    { 1372.47f, 1461.61f, 324.354f, nullptr },
    { 1389.08f, 1461.12f, 325.913f, nullptr },
    { 1406.57f, 1460.48f, 330.615f, nullptr },
    { 1424.04f, 1459.57f, 336.029f, nullptr },
    { 1442.5f, 1459.7f, 342.024f, nullptr },
    { 1471.86f, 1456.65f, 342.794f, nullptr },
    { 1470.93f, 1440.5f, 342.794f, nullptr },
    { 1472.24f, 1427.49f, 342.06f, nullptr },
    { 1476.86f, 1412.46f, 341.426f, nullptr },
    { 1484.42f, 1396.69f, 346.117f, nullptr },
    { 1490.7f, 1387.59f, 351.861f, nullptr },
    { 1500.79f, 1382.98f, 357.784f, nullptr },
    { 1511.08f, 1391.29f, 364.444f, nullptr },
    { 1517.85f, 1403.18f, 370.336f, nullptr },
    { 1517.99f, 1417.59f, 371.636f, nullptr },
    { 1517.07f, 1431.56f, 372.106f, nullptr },
    { 1516.66f, 1445.55f, 372.939f, nullptr },
    { 1514.23f, 1457.37f, 373.689f, nullptr },
    { 1503.73f, 1457.67f, 373.684f, nullptr },
    { 1486.24f, 1457.8f, 373.718f, nullptr },
    { 1476.78f, 1460.35f, 373.711f, nullptr },
    { 1477.37f, 1470.83f, 373.709f, nullptr },
    { 1477.5f, 1484.83f, 373.715f, nullptr },
    { 1480.53f, 1495.26f, 373.721f, nullptr },
    { 1492.61f, 1494.72f, 373.721f, nullptr },
    { 1499.37f, 1489.02f, 373.718f, nullptr },
    { 1500.63f, 1472.89f, 373.707f, nullptr },
};
// Alliance Base to Stables
BattleBotPath vPath_AB_AllianceBase_to_Stables =
{
    { 1285.67f, 1282.14f, -15.8466f, nullptr },
    { 1272.52f, 1267.83f, -21.7811f, nullptr },
    { 1250.44f, 1248.09f, -33.3028f, nullptr },
    { 1232.56f, 1233.05f, -41.5241f, nullptr },
    { 1213.25f, 1224.93f, -47.5513f, nullptr },
    { 1189.29f, 1219.49f, -53.119f, nullptr },
    { 1177.17f, 1210.21f, -56.4593f, nullptr },
    { 1167.98f, 1202.9f, -56.4743f, nullptr },
};
// Alliance Base to Gold Mine
BattleBotPath vPath_AB_AllianceBase_to_GoldMine =
{
    { 1285.67f, 1282.14f, -15.8466f, nullptr },
    { 1276.41f, 1267.11f, -20.775f, nullptr },
    { 1261.34f, 1241.52f, -31.2971f, nullptr },
    { 1244.91f, 1219.03f, -41.9658f, nullptr },
    { 1232.25f, 1184.41f, -50.3348f, nullptr },
    { 1226.89f, 1150.82f, -55.7935f, nullptr },
    { 1224.09f, 1120.38f, -57.0633f, nullptr },
    { 1220.03f, 1092.72f, -59.1744f, nullptr },
    { 1216.05f, 1060.86f, -67.2771f, nullptr },
    { 1213.77f, 1027.96f, -74.429f, nullptr },
    { 1208.56f, 998.394f, -81.9493f, nullptr },
    { 1197.42f, 969.73f, -89.9385f, nullptr },
    { 1185.23f, 944.531f, -97.2433f, nullptr },
    { 1166.29f, 913.945f, -107.214f, nullptr },
    { 1153.17f, 887.863f, -112.34f, nullptr },
    { 1148.89f, 871.391f, -111.96f, nullptr },
    { 1145.24f, 850.82f, -110.514f, nullptr },
};
// Alliance Base to Lumber Mill
BattleBotPath vPath_AB_AllianceBase_to_LumberMill =
{
    { 1285.67f, 1282.14f, -15.8466f, nullptr },
    { 1269.13f, 1267.89f, -22.7764f, nullptr },
    { 1247.79f, 1249.77f, -33.2518f, nullptr },
    { 1226.29f, 1232.02f, -43.9193f, nullptr },
    { 1196.68f, 1230.15f, -50.4644f, nullptr },
    { 1168.72f, 1228.98f, -53.9329f, nullptr },
    { 1140.82f, 1226.7f, -53.6318f, nullptr },
    { 1126.85f, 1225.77f, -47.98f, nullptr },
    { 1096.5f, 1226.57f, -53.1769f, nullptr },
    { 1054.52f, 1226.14f, -49.2011f, nullptr },
    { 1033.52f, 1226.08f, -45.5968f, nullptr },
    { 1005.52f, 1226.08f, -43.2912f, nullptr },
    { 977.53f, 1226.68f, -40.16f, nullptr },
    { 957.242f, 1227.94f, -34.1487f, nullptr },
    { 930.689f, 1221.57f, -18.9588f, nullptr },
    { 918.202f, 1211.98f, -12.2494f, nullptr },
    { 880.329f, 1192.63f, 7.61168f, nullptr },
    { 869.965f, 1178.52f, 10.9678f, nullptr },
    { 864.74f, 1163.78f, 12.385f, nullptr },
    { 859.165f, 1148.84f, 11.5289f, nullptr },
};
// Stables to Blacksmith
BattleBotPath vPath_AB_Stables_to_Blacksmith =
{
    { 1169.52f, 1198.71f, -56.2742f, nullptr },
    { 1166.93f, 1185.2f, -56.3634f, nullptr },
    { 1173.84f, 1170.6f, -56.4094f, nullptr },
    { 1186.7f, 1163.92f, -56.3961f, nullptr },
    { 1189.7f, 1150.68f, -55.8664f, nullptr },
    { 1185.18f, 1129.31f, -58.1044f, nullptr },
    { 1181.7f, 1108.6f, -62.1797f, nullptr },
    { 1177.92f, 1087.95f, -63.5768f, nullptr },
    { 1174.52f, 1067.23f, -64.402f, nullptr },
    { 1171.27f, 1051.09f, -65.0833f, nullptr },
    { 1163.22f, 1031.7f, -64.954f, nullptr },
    { 1154.25f, 1010.25f, -63.5299f, nullptr },
    { 1141.07f, 999.479f, -63.3713f, nullptr },
    { 1127.12f, 1000.37f, -60.628f, nullptr },
    { 1106.17f, 1001.66f, -61.7457f, nullptr },
    { 1085.64f, 1005.62f, -58.5932f, nullptr },
    { 1064.88f, 1008.65f, -52.3547f, nullptr },
    { 1044.16f, 1011.96f, -47.2647f, nullptr },
    { 1029.72f, 1014.88f, -45.3546f, nullptr },
    { 1013.94f, 1028.7f, -43.9786f, nullptr },
    { 990.89f, 1039.3f, -42.7659f, nullptr },
    { 978.269f, 1043.84f, -44.4588f, nullptr },
};
// Horde Base to Farm
BattleBotPath vPath_AB_HordeBase_to_Farm =
{
    { 707.259f, 707.839f, -17.5318f, nullptr },
    { 712.063f, 712.928f, -20.1802f, nullptr },
    { 725.941f, 728.682f, -29.7536f, nullptr },
    { 734.715f, 739.591f, -35.2144f, nullptr },
    { 747.607f, 756.161f, -40.899f, nullptr },
    { 753.994f, 766.668f, -43.3049f, nullptr },
    { 758.715f, 787.106f, -46.7014f, nullptr },
    { 762.077f, 807.831f, -48.4721f, nullptr },
    { 764.132f, 821.68f, -49.656f, nullptr },
    { 767.947f, 839.274f, -50.8574f, nullptr },
    { 773.745f, 852.013f, -52.6226f, nullptr },
    { 785.123f, 869.103f, -54.2089f, nullptr },
    { 804.429f, 874.961f, -55.2691f, nullptr },
};
// Horde Base to Gold Mine
BattleBotPath vPath_AB_HordeBase_to_GoldMine =
{
    { 707.259f, 707.839f, -17.5318f, nullptr },
    { 717.935f, 716.874f, -23.3941f, nullptr },
    { 739.195f, 732.483f, -34.5791f, nullptr },
    { 757.087f, 742.008f, -38.1123f, nullptr },
    { 776.946f, 748.775f, -42.7346f, nullptr },
    { 797.138f, 754.539f, -46.3237f, nullptr },
    { 817.37f, 760.167f, -48.9235f, nullptr },
    { 837.638f, 765.664f, -49.7374f, nullptr },
    { 865.092f, 774.738f, -51.9831f, nullptr },
    { 878.86f, 777.149f, -47.2361f, nullptr },
    { 903.911f, 780.212f, -53.1424f, nullptr },
    { 923.454f, 787.888f, -54.7937f, nullptr },
    { 946.218f, 798.93f, -59.0904f, nullptr },
    { 978.1f, 813.321f, -66.7268f, nullptr },
    { 1002.94f, 817.895f, -77.3119f, nullptr },
    { 1030.77f, 820.92f, -88.7717f, nullptr },
    { 1058.61f, 823.889f, -94.1623f, nullptr },
    { 1081.6f, 828.32f, -99.4137f, nullptr },
    { 1104.64f, 844.773f, -106.387f, nullptr },
    { 1117.56f, 853.686f, -110.716f, nullptr },
    { 1144.9f, 850.049f, -110.522f, nullptr },
};
// Horde Base to Lumber Mill
BattleBotPath vPath_AB_HordeBase_to_LumberMill =
{
    { 707.259f, 707.839f, -17.5318f, nullptr },
    { 721.611f, 726.507f, -27.9646f, nullptr },
    { 733.846f, 743.573f, -35.8633f, nullptr },
    { 746.201f, 760.547f, -40.838f, nullptr },
    { 758.937f, 787.565f, -46.741f, nullptr },
    { 761.289f, 801.357f, -48.0037f, nullptr },
    { 764.341f, 822.128f, -49.6908f, nullptr },
    { 769.766f, 842.244f, -51.1239f, nullptr },
    { 775.322f, 855.093f, -53.1161f, nullptr },
    { 783.995f, 874.216f, -55.0822f, nullptr },
    { 789.917f, 886.902f, -56.2935f, nullptr },
    { 798.03f, 906.259f, -57.1162f, nullptr },
    { 803.183f, 919.266f, -57.6692f, nullptr },
    { 813.248f, 937.688f, -57.7106f, nullptr },
    { 820.412f, 958.712f, -56.1492f, nullptr },
    { 814.247f, 973.692f, -50.4602f, nullptr },
    { 807.697f, 985.502f, -47.2383f, nullptr },
    { 795.672f, 1002.69f, -44.9382f, nullptr },
    { 784.653f, 1020.77f, -38.6278f, nullptr },
    { 784.826f, 1037.34f, -31.5719f, nullptr },
    { 786.083f, 1051.28f, -24.0793f, nullptr },
    { 787.314f, 1065.23f, -16.8918f, nullptr },
    { 788.892f, 1086.17f, -6.42608f, nullptr },
    { 792.077f, 1106.53f, 4.81124f, nullptr },
    { 800.398f, 1119.48f, 8.5814f, nullptr },
    { 812.476f, 1131.1f, 10.439f, nullptr },
    { 829.704f, 1142.52f, 10.738f, nullptr },
    { 842.646f, 1143.51f, 11.9984f, nullptr },
    { 857.674f, 1146.16f, 11.529f, nullptr },
};
// Farm to Blacksmith
BattleBotPath vPath_AB_Farm_to_Blacksmith =
{
    { 803.826f, 874.909f, -55.2547f, nullptr },
    { 808.763f, 887.991f, -57.4437f, nullptr },
    { 818.33f, 906.674f, -59.3554f, nullptr },
    { 828.634f, 924.972f, -60.5664f, nullptr },
    { 835.255f, 937.308f, -60.2915f, nullptr },
    { 845.244f, 955.78f, -60.4208f, nullptr },
    { 852.125f, 967.965f, -61.3135f, nullptr },
    { 863.232f, 983.109f, -62.6402f, nullptr },
    { 875.413f, 989.245f, -61.2916f, nullptr },
    { 895.765f, 994.41f, -63.6287f, nullptr },
    { 914.16f, 1001.09f, -58.37f, nullptr },
    { 932.418f, 1011.44f, -51.9225f, nullptr },
    { 944.244f, 1018.92f, -49.1438f, nullptr },
    { 961.55f, 1030.81f, -45.814f, nullptr },
    { 978.122f, 1043.87f, -44.4682f, nullptr },
};
// Stables to Gold Mine
BattleBotPath vPath_AB_Stables_to_GoldMine =
{
    { 1169.52f, 1198.71f, -56.2742f, nullptr },
    { 1166.72f, 1183.58f, -56.3633f, nullptr },
    { 1172.14f, 1170.99f, -56.4735f, nullptr },
    { 1185.18f, 1164.02f, -56.4269f, nullptr },
    { 1193.98f, 1155.85f, -55.924f, nullptr },
    { 1201.51f, 1145.65f, -56.4733f, nullptr },
    { 1205.39f, 1134.81f, -56.2366f, nullptr },
    { 1207.57f, 1106.9f, -58.4748f, nullptr },
    { 1209.4f, 1085.98f, -63.4022f, nullptr },
    { 1212.68f, 1065.25f, -66.514f, nullptr },
    { 1216.42f, 1037.52f, -72.0457f, nullptr },
    { 1215.4f, 1011.56f, -78.3338f, nullptr },
    { 1209.8f, 992.293f, -83.2433f, nullptr },
    { 1201.23f, 973.121f, -88.5661f, nullptr },
    { 1192.16f, 954.183f, -94.2209f, nullptr },
    { 1181.88f, 935.894f, -99.5239f, nullptr },
    { 1169.86f, 918.68f, -105.588f, nullptr },
    { 1159.36f, 900.497f, -110.461f, nullptr },
    { 1149.32f, 874.429f, -112.142f, nullptr },
    { 1145.34f, 849.824f, -110.523f, nullptr },
};
// Stables to Lumber Mill
BattleBotPath vPath_AB_Stables_to_LumberMill =
{
    { 1169.52f, 1198.71f, -56.2742f, nullptr },
    { 1169.33f, 1203.43f, -56.5457f, nullptr },
    { 1164.77f, 1208.73f, -56.1907f, nullptr },
    { 1141.52f, 1224.99f, -53.8204f, nullptr },
    { 1127.54f, 1224.82f, -48.2081f, nullptr },
    { 1106.56f, 1225.58f, -50.5154f, nullptr },
    { 1085.6f, 1226.54f, -53.1863f, nullptr },
    { 1064.6f, 1226.82f, -50.4381f, nullptr },
    { 1043.6f, 1227.27f, -46.5439f, nullptr },
    { 1022.61f, 1227.72f, -44.7157f, nullptr },
    { 1001.61f, 1227.62f, -42.6876f, nullptr },
    { 980.623f, 1226.93f, -40.4687f, nullptr },
    { 959.628f, 1227.1f, -35.3838f, nullptr },
    { 938.776f, 1226.34f, -23.5399f, nullptr },
    { 926.138f, 1217.21f, -16.2176f, nullptr },
    { 911.966f, 1205.99f, -9.69655f, nullptr },
    { 895.135f, 1198.85f, -0.546275f, nullptr },
    { 873.419f, 1189.27f, 9.3466f, nullptr },
    { 863.821f, 1181.72f, 9.76912f, nullptr },
    { 851.803f, 1166.3f, 10.4423f, nullptr },
    { 853.921f, 1150.92f, 11.543f, nullptr },
};
// Farm to Gold Mine
BattleBotPath vPath_AB_Farm_to_GoldMine =
{
    { 803.826f, 874.909f, -55.2547f, nullptr },
    { 801.662f, 865.689f, -56.9445f, nullptr },
    { 806.433f, 860.776f, -57.5899f, nullptr },
    { 816.236f, 857.397f, -57.7029f, nullptr },
    { 826.717f, 855.846f, -57.9914f, nullptr },
    { 836.128f, 851.257f, -57.8321f, nullptr },
    { 847.933f, 843.837f, -58.1296f, nullptr },
    { 855.08f, 832.688f, -57.7373f, nullptr },
    { 864.513f, 813.663f, -57.574f, nullptr },
    { 864.229f, 797.762f, -54.2057f, nullptr },
    { 862.967f, 787.372f, -53.0276f, nullptr },
    { 864.163f, 776.33f, -52.0372f, nullptr },
    { 872.583f, 777.391f, -48.5342f, nullptr },
    { 893.575f, 777.922f, -49.1826f, nullptr },
    { 915.941f, 783.534f, -53.6598f, nullptr },
    { 928.105f, 789.929f, -55.4802f, nullptr },
    { 946.263f, 800.46f, -59.166f, nullptr },
    { 958.715f, 806.845f, -62.1494f, nullptr },
    { 975.79f, 811.913f, -65.9648f, nullptr },
    { 989.468f, 814.883f, -71.3089f, nullptr },
    { 1010.13f, 818.643f, -80.0817f, nullptr },
    { 1023.97f, 820.667f, -86.1114f, nullptr },
    { 1044.84f, 823.011f, -92.0583f, nullptr },
    { 1058.77f, 824.482f, -94.1937f, nullptr },
    { 1079.13f, 829.402f, -99.1207f, nullptr },
    { 1092.85f, 836.986f, -102.755f, nullptr },
    { 1114.75f, 851.21f, -109.782f, nullptr },
    { 1128.22f, 851.928f, -111.078f, nullptr },
    { 1145.14f, 849.895f, -110.523f, nullptr },
};
// Farm to Lumber Mill
BattleBotPath vPath_AB_Farm_to_LumberMill =
{
    { 803.826f, 874.909f, -55.2547f, nullptr },
    { 802.874f, 894.28f, -56.4661f, nullptr },
    { 806.844f, 920.39f, -57.3157f, nullptr },
    { 814.003f, 934.161f, -57.6065f, nullptr },
    { 824.594f, 958.47f, -58.4916f, nullptr },
    { 820.434f, 971.184f, -53.201f, nullptr },
    { 808.339f, 987.79f, -47.5705f, nullptr },
    { 795.98f, 1004.76f, -44.9189f, nullptr },
    { 785.497f, 1019.18f, -39.2806f, nullptr },
    { 783.94f, 1032.46f, -33.5692f, nullptr },
    { 784.956f, 1053.41f, -22.8368f, nullptr },
    { 787.499f, 1074.25f, -12.4232f, nullptr },
    { 789.406f, 1088.11f, -5.28606f, nullptr },
    { 794.617f, 1109.17f, 6.1966f, nullptr },
    { 801.514f, 1120.77f, 8.81455f, nullptr },
    { 817.3f, 1134.59f, 10.6064f, nullptr },
    { 828.961f, 1142.98f, 10.7354f, nullptr },
    { 841.63f, 1147.75f, 11.6916f, nullptr },
    { 854.326f, 1150.55f, 11.537f, nullptr },
};

BattleBotPath vPath_AV_Horde_Cave_to_Tower_Point_Crossroad =
{
    { -885.928f, -536.612f, 55.1936f, nullptr },
    { -880.957f, -525.119f, 53.6791f, nullptr },
    { -839.408f, -499.746f, 49.7505f, nullptr },
    { -820.21f, -469.193f, 49.4085f,  nullptr },
    { -812.602f, -460.45f, 54.0872f,  nullptr },
    { -789.646f, -441.864f, 57.8833f, nullptr },
    { -776.405f, -432.056f, 61.9256f, nullptr },
    { -760.773f, -430.154f, 64.8376f, nullptr },
    { -734.801f, -419.622f, 67.5354f, nullptr },
    { -718.313f, -404.674f, 67.5994f, nullptr },
    { -711.436f, -362.86f, 66.7543f,  nullptr },
};

// 2.4 + Horde cave is moved
#ifdef MANGOSBOT_ZERO
BattleBotPath vPath_AV_Horde_Cave_to_Frostwolf_Graveyard_Flag =
{
    { -885.928f, -536.612f, 55.1936f, nullptr },
    { -892.195f, -527.288f, 54.3716f, nullptr },
    { -901.886f, -506.521f, 54.2415f, nullptr },
    { -908.843f, -494.624f, 46.3098f, nullptr },
    { -919.567f, -480.127f, 43.6671f, nullptr },
    { -939.133f, -456.123f, 43.1962f, nullptr },
    { -965.027f, -425.647f, 47.8552f, nullptr },
    { -980.983f, -417.492f, 49.7504f, nullptr },
    { -1011.67f, -415.94f, 51.0241f, nullptr },
    { -1035.81f, -410.183f, 51.3285f, nullptr },
    { -1049.87f, -400.511f, 51.4724f, nullptr },
    { -1054.03f, -379.162f, 51.4679f, nullptr },
    { -1081.32f, -348.198f, 54.5837f, nullptr },
};
#else
BattleBotPath vPath_AV_Horde_Cave_to_Frostwolf_Graveyard_Flag =
{
    { -1362.530f, -529.904f, 52.687f, nullptr },
    { -1335.523f, -513.374f, 51.142f, nullptr },
    { -1303.788f, -522.090f, 51.872f, nullptr },
    { -1275.968f, -517.437f, 50.574f, nullptr },
    { -1250.891f, -514.016f, 50.603f, nullptr },
    { -1229.146f, -511.427f, 50.739f, nullptr },
    { -1202.997f, -505.638f, 51.577f, nullptr },
    { -1183.442f, -484.124f, 51.845f, nullptr },
    { -1150.984f, -464.204f, 55.353f, nullptr },
    { -1144.187f, -442.148f, 56.736f, nullptr },
    { -1036.291f, -409.307f, 56.365f, nullptr },
    { -1107.650f, -377.764f, 51.432f, nullptr },
    { -1081.32f, -348.198f, 54.5837f, nullptr },
};
#endif

BattleBotPath vPath_AV_Frostwolf_Graveyard_Flag_to_Coldtooth_Mine_Entrance =
{
    { -1081.32f, -348.198f, 54.5837f, nullptr },
    { -1065.539f, -333.798f, 56.316f, nullptr },
    { -1046.376f, -319.197f, 56.647f, nullptr },
    { -994.817f, -336.004f, 58.477f, nullptr },
    { -930.840f, -337.938f, 60.100f, nullptr },
    { -953.328f, -322.536f, 63.448f, nullptr },
    { -950.566f, -293.341f, 64.368f, nullptr },
    { -952.234f, -245.737f, 65.631f, nullptr },
};

BattleBotPath vPath_AV_Coldtooth_Mine_Entrance_to_Coldtooth_Mine_Boss =
{
    { -952.234f, -245.737f, 65.631f, nullptr },
    { -962.481f, -230.770f, 66.976f, nullptr },
    { -967.597f, -203.598f, 68.842f, nullptr },
    { -948.614f, -182.609f, 65.192f, nullptr },
    { -936.487f, -158.700f, 60.954f, nullptr },
    { -899.206f, -154.360f, 61.618f, nullptr },
    { -879.446f, -152.687f, 62.299f, nullptr },
    { -871.995f, -131.877f, 62.660f, nullptr },
    { -869.881f, -113.359f, 64.956f, nullptr },
    { -853.671f, -91.427f, 68.569f, nullptr },
};

BattleBotPath vPath_AV_Frostwolf_Graveyard_Flag_to_Horde_Base_First_Crossroads =
{
     { -1081.32f, -348.198f, 54.5837f, nullptr },
     { -1085.54f, -349.316f, 54.5448f, nullptr },
     { -1096.08f, -339.028f, 54.5433f, nullptr },
     { -1112.8f, -338.773f, 53.4886f, nullptr },
     { -1137.95f, -349.136f, 51.1539f, nullptr },
     { -1160.92f, -352.495f, 51.7134f, nullptr },
     { -1181.12f, -360.f, 52.4494f, nullptr },
     { -1191.79f, -365.559f, 52.4952f, nullptr },
     { -1210.16f, -366.523f, 55.6316f, nullptr },
     { -1238.08f, -364.752f, 59.3705f, nullptr },
     { -1242.44f, -361.619f, 59.7433f, nullptr },
     { -1246.25f, -350.16f, 59.4911f, nullptr },
     { -1245.71f, -338.859f, 59.2882f, nullptr },
     { -1238.46f, -325.945f, 60.084f, nullptr },
     { -1227.58f, -312.249f, 63.2087f, nullptr },
     { -1216.36f, -296.148f, 70.0471f, nullptr },
     { -1210.f, -276.949f, 73.7288f, nullptr },
     { -1207.09f, -259.721f, 72.6882f, nullptr },
     { -1208.37f, -254.367f, 72.5688f, nullptr },
     { -1211.83f, -252.648f, 72.7613f, nullptr },
     { -1236.28f, -251.079f, 73.3273f, nullptr },
     { -1243.33f, -253.936f, 73.3273f, nullptr },
     { -1249.37f, -260.819f, 73.3704f, nullptr },
     { -1257.24f, -272.391f, 72.9263f, nullptr },
     { -1266.31f, -281.263f, 76.3849f, nullptr },
     { -1272.81f, -285.884f, 82.5902f, nullptr },
     { -1278.52f, -288.089f, 86.4665f, nullptr },
     { -1288.71f, -289.227f, 89.7414f, nullptr },
     { -1306.2f, -289.35f, 90.7095f, nullptr },
     { -1320.18f, -289.806f, 90.4719f, nullptr },
};

BattleBotPath vPath_AV_Horde_Base_First_Crossroads_to_East_Frostwolf_Tower_Flag =
{
    { -1320.18f, -289.806f, 90.4719f, nullptr },
    { -1315.93f, -293.794f, 90.4653f, nullptr },
    { -1312.71f, -300.242f, 90.9343f, nullptr },
    { -1309.39f, -305.389f, 91.7743f, nullptr },
    { -1306.77f, -310.068f, 91.7371f, nullptr },
    { -1300.2f, -322.303f, 91.3631f, nullptr },
    { -1295.93f, -320.154f, 91.3596f, nullptr },
    { -1301.5f, -309.995f, 95.7382f, nullptr },
    { -1309.02f, -315.629f, 99.4271f, nullptr },
    { -1304.13f, -321.638f, 102.323f, nullptr },
    { -1298.43f, -317.239f, 104.769f, nullptr },
    { -1304.62f, -310.076f, 107.328f, nullptr },
    { -1306.73f, -316.046f, 107.328f, nullptr },
    { -1314.61f, -321.14f, 107.316f, nullptr },
    { -1311.92f, -324.548f, 109.202f, nullptr },
    { -1304.72f, -328.225f, 113.563f, nullptr },
    { -1301.77f, -326.848f, 113.84f, nullptr },
    { -1294.77f, -321.092f, 113.792f, nullptr },
    { -1294.33f, -312.247f, 113.794f, nullptr },
    { -1302.25f, -307.602f, 113.856f, nullptr },
    { -1305.22f, -311.212f, 113.864f, nullptr },
    { -1303.38f, -315.877f, 113.867f, nullptr },
};

BattleBotPath vPath_AV_Horde_Base_First_Crossroads_to_West_Frostwolf_Tower_Flag =
{
    { -1320.18f, -289.806f, 90.4719f, nullptr },
    { -1318.43f, -283.088f, 90.5913f, nullptr },
    { -1315.58f, -275.043f, 91.1062f, nullptr },
    { -1311.21f, -271.424f, 91.8402f, nullptr },
    { -1306.39f, -269.65f, 92.0506f, nullptr },
    { -1292.18f, -264.937f, 91.6452f, nullptr },
    { -1292.76f, -261.18f, 91.6437f, nullptr },
    { -1296.92f, -261.369f, 92.6981f, nullptr },
    { -1303.73f, -263.629f, 95.987f, nullptr },
    { -1300.5f, -272.465f, 99.691f, nullptr },
    { -1293.33f, -269.5f, 102.51f, nullptr },
    { -1295.96f, -263.022f, 105.042f, nullptr },
    { -1304.46f, -266.371f, 107.612f, nullptr },
    { -1299.74f, -270.916f, 107.612f, nullptr },
    { -1296.95f, -279.413f, 107.585f, nullptr },
    { -1291.49f, -276.799f, 110.059f, nullptr },
    { -1286.89f, -269.783f, 114.142f, nullptr },
    { -1290.87f, -260.594f, 114.151f, nullptr },
    { -1299.88f, -257.96f, 114.111f, nullptr },
    { -1306.39f, -263.798f, 114.147f, nullptr },
    { -1305.91f, -269.074f, 114.066f, nullptr },
    { -1298.82f, -267.215f, 114.151f, nullptr },
};

BattleBotPath vPath_AV_Horde_Base_First_Crossroads_to_Horde_Base_Second_Crossroads =
{
    { -1320.18f, -289.806f, 90.4719f, nullptr },
    { -1334.14f, -290.898f, 90.8243f, nullptr },
    { -1344.61f, -291.546f, 90.8375f, nullptr },
};

BattleBotPath vPath_AV_Horde_Base_Second_Crossroads_to_Horde_Base_Entrance_DrekThar =
{
    { -1344.61f, -291.546f, 90.8375f, nullptr },
    { -1348.33f, -287.291f, 91.1178f, nullptr },
    { -1350.16f, -282.141f, 92.7105f, nullptr },
    { -1355.48f, -265.233f, 98.9033f, nullptr },
    { -1360.94f, -247.529f, 99.3667f, nullptr },
};

BattleBotPath vPath_AV_Horde_Base_Second_Crossroads_to_Horde_Base_DrekThar1 =
{
    { -1360.94f, -247.529f, 99.3667f, nullptr },
    { -1354.74f, -242.226f, 99.3695f, nullptr },
    { -1348.1f, -235.612f, 99.3657f, nullptr },
    { -1349.98f, -229.222f, 99.3699f, nullptr },
    { -1366.23f, -223.049f, 98.4174f, nullptr },
};

BattleBotPath vPath_AV_Horde_Base_Second_Crossroads_to_Horde_Base_DrekThar2 =
{
    { -1360.94f, -247.529f, 99.3667f, nullptr },
    { -1368.08f, -247.832f, 99.3703f, nullptr },
    { -1377.83f, -245.929f, 99.3629f, nullptr },
    { -1381.21f, -239.565f, 99.3698f, nullptr },
    { -1372.52f, -225.593f, 98.4269f, nullptr },
};

BattleBotPath vPath_AV_Horde_Base_Second_Crossroads_to_Horde_Base_Graveyard_Flag =
{
    { -1344.61f, -291.546f, 90.8375f, nullptr },
    { -1353.15f, -296.183f, 90.6364f, nullptr },
    { -1360.12f, -302.321f, 91.5223f, nullptr },
    { -1372.69f, -308.451f, 91.2869f, nullptr },
    { -1392.43f, -318.109f, 88.7925f, nullptr },
    { -1400.8f, -314.327f, 89.0501f, nullptr },
    { -1401.94f, -310.103f, 89.3816f, nullptr },
};

BattleBotPath vPath_AV_Tower_Point_Crossroads_to_Tower_Point_Bottom =
{
    { -711.436f, -362.86f, 66.7543f,  nullptr },
    { -713.433f, -357.847f, 66.6605f, nullptr },
    { -726.362f, -345.477f, 66.8089f, nullptr },
    { -748.788f, -344.154f, 66.7348f, nullptr },
    { -759.771f, -342.304f, 67.2223f, nullptr },
};

BattleBotPath vPath_AV_TowerPoint_Bottom_to_Tower_Point_Flag =
{
    { -759.771f, -342.304f, 67.2223f, nullptr },
    { -762.294f, -343.172f, 67.3607f, nullptr },
    { -764.151f, -350.571f, 68.7991f, nullptr },
    { -766.112f, -357.945f, 68.6996f, nullptr },
    { -770.997f, -370.089f, 68.3956f, nullptr },
    { -763.765f, -368.338f, 69.1196f, nullptr },
    { -761.735f, -359.76f, 72.7363f, nullptr },
    { -771.43f, -357.941f, 76.4841f, nullptr },
    { -773.925f, -365.214f, 79.2135f, nullptr },
    { -766.473f, -365.891f, 81.9322f, nullptr },
    { -765.147f, -355.981f, 84.3558f, nullptr },
    { -771.041f, -360.772f, 84.3558f, nullptr },
    { -779.831f, -356.316f, 84.3425f, nullptr },
    { -780.107f, -362.818f, 87.4599f, nullptr },
    { -775.392f, -371.248f, 90.8508f, nullptr },
    { -767.966f, -372.722f, 90.8949f, nullptr },
    { -759.167f, -366.147f, 90.8259f, nullptr },
    { -760.11f, -357.787f, 90.8949f, nullptr },
    { -764.399f, -355.108f, 90.8013f, nullptr },
    { -767.9f, -362.019f, 90.8949f, nullptr },
};

BattleBotPath vPath_AV_Tower_Point_Bottom_to_Frostwolf_Graveyard_Flag =
{
    { -759.771f, -342.304f, 67.2223f, nullptr },
    { -764.971f, -339.278f, 67.6875f, nullptr },
    { -773.394f, -335.633f, 66.4157f, nullptr },
    { -796.758f, -340.437f, 61.5754f, nullptr },
    { -828.745f, -348.592f, 50.1022f, nullptr },
    { -846.826f, -355.181f, 50.0754f, nullptr },
    { -869.897f, -359.01f, 50.9404f, nullptr },
    { -888.679f, -365.688f, 49.3732f, nullptr },
    { -908.082f, -381.24f, 48.9888f, nullptr },
    { -934.234f, -388.41f, 48.9912f, nullptr },
    { -960.683f, -395.321f, 49.028f, nullptr },
    { -970.161f, -397.02f, 49.2312f, nullptr },
    { -993.784f, -397.619f, 50.0896f, nullptr },
    { -1018.82f, -393.742f, 50.703f, nullptr },
    { -1047.38f, -380.337f, 51.1403f, nullptr },
    { -1066.7f, -361.097f, 51.3909f, nullptr },
    { -1079.61f, -345.548f, 55.1131f, nullptr },
};

BattleBotPath vPath_AV_Frostwolf_Graveyard_to_Frostwolf_Graveyard_Flag =
{
    { -1089.6f, -268.375f, 57.038f, nullptr },
    { -1087.23f, -285.712f, 56.625f, nullptr },
    { -1084.83f, -307.023f, 56.5773f, nullptr },
    { -1082.81f, -327.926f, 54.863f, nullptr },
    { -1082.08f, -333.784f, 54.885f, nullptr },
    { -1079.61f, -345.548f, 55.1131f, nullptr },
};

BattleBotPath vPath_AV_Tower_Point_Crossroads_to_Iceblood_Graveyard_Flag =
{
    { -711.436f, -362.86f, 66.7543f,  nullptr },
    { -703.169f, -363.672f, 66.3514f, nullptr },
    { -682.166f, -374.453f, 65.6513f, nullptr },
    { -653.798f, -387.044f, 62.0839f, nullptr },
    { -625.062f, -397.093f, 59.0311f, nullptr },
    { -614.138f, -396.501f, 60.8585f, nullptr },
};

BattleBotPath vPath_AV_Iceblood_Graveyard_Flag_to_Iceblood_Tower_Crossroad =
{
    { -614.138f, -396.501f, 60.8585f, nullptr },
    { -620.856f, -389.074f, 58.4251f, nullptr },
    { -622.361f, -366.641f, 56.862f, nullptr },
    { -618.134f, -346.849f, 55.2242f, nullptr },
    { -586.744f, -317.327f, 48.149f, nullptr },
    { -579.697f, -315.037f, 46.345f, nullptr },
};

BattleBotPath vPath_AV_Iceblood_Tower_Crossroad_to_Iceblood_Tower =
{
    { -579.697f, -315.037f, 46.345f, nullptr },
    { -579.747f, -308.986f, 46.6127f, nullptr },
    { -575.92f, -296.013f, 48.4349f, nullptr },
    { -567.029f, -286.975f, 50.7703f, nullptr },
    { -557.697f, -276.264f, 52.1503f, nullptr },
};

BattleBotPath vPath_AV_Iceblood_Tower_to_Iceblood_Tower_Flag =
{
    { -557.697f, -276.264f, 52.1503f, nullptr },
    { -562.505f, -271.251f, 52.9165f, nullptr },
    { -575.986f, -258.447f, 52.5129f, nullptr },
    { -580.103f, -261.305f, 52.5013f, nullptr },
    { -571.844f, -269.038f, 56.8539f, nullptr },
    { -565.86f, -261.478f, 60.5514f, nullptr },
    { -572.102f, -256.66f, 63.3275f, nullptr },
    { -576.45f, -262.642f, 65.8959f, nullptr },
    { -568.831f, -268.036f, 68.4696f, nullptr },
    { -568.091f, -260.214f, 68.4696f, nullptr },
    { -561.978f, -254.631f, 68.4482f, nullptr },
    { -570.682f, -250.791f, 73.0299f, nullptr },
    { -576.069f, -252.266f, 74.9855f, nullptr },
    { -581.294f, -260.533f, 74.9366f, nullptr },
    { -579.309f, -268.79f, 74.9984f, nullptr },
    { -570.275f, -271.346f, 75.0083f, nullptr },
    { -566.436f, -268.102f, 74.9324f, nullptr },
    { -571.044f, -263.753f, 75.0087f, nullptr },
};

BattleBotPath vPath_AV_Iceblood_Tower_to_Iceblood_Garrison =
{
    { -557.697f, -276.264f, 52.1503f, nullptr },
    { -550.396f, -269.326f, 52.4539f, nullptr },
    { -542.628f, -254.901f, 54.7447f, nullptr },
    { -536.969f, -242.614f, 57.233f, nullptr },
    { -532.204f, -230.96f, 56.7126f, nullptr },
    { -499.532f, -204.606f, 57.4507f, nullptr },
    { -492.17f, -187.077f, 57.1342f, nullptr },
};

BattleBotPath vPath_AV_Iceblood_Garrison_to_Captain_Galvangar =
{
    { -492.17f, -187.077f, 57.1342f, nullptr },
    { -496.52f, -186.015f, 57.4265f, nullptr },
    { -508.094f, -180.795f, 57.9437f, nullptr },
    { -519.813f, -179.116f, 57.9527f, nullptr },
    { -528.962f, -186.573f, 57.9513f, nullptr },
    { -538.064f, -186.984f, 57.9556f, nullptr },
    { -540.747f, -169.935f, 57.0124f, nullptr },
};

BattleBotPath vPath_AV_Iceblood_Garrison_to_Snowfall_Flag =
{
    { -492.17f, -187.077f, 57.1342f, nullptr },
    { -478.362f, -193.882f, 55.0986f, nullptr },
    { -463.702f, -196.051f, 48.5213f, nullptr },
    { -456.512f, -196.217f, 43.158f, nullptr },
    { -429.951f, -198.515f, 26.8893f, nullptr },
    { -412.975f, -190.921f, 25.7659f, nullptr },
    { -387.531f, -186.251f, 15.2001f, nullptr },
    { -370.088f, -186.351f, 12.8474f, nullptr },
    { -353.23f, -179.161f, 10.3823f, nullptr },
    { -319.906f, -162.842f, 9.26139f, nullptr },
    { -316.616f, -151.198f, 10.5439f, nullptr },
    { -307.702f, -133.042f, 14.3502f, nullptr },
    { -293.423f, -117.684f, 19.8246f, nullptr },
    { -274.446f, -97.1283f, 40.3783f, nullptr },
    { -256.993f, -83.0377f, 54.9012f, nullptr },
    { -249.129f, -80.3578f, 58.6962f, nullptr },
    { -237.627f, -84.7628f, 63.982f, nullptr },
    { -226.054f, -92.6308f, 75.1362f, nullptr },
    { -205.464f, -114.337f, 78.6167f, nullptr },
};

BattleBotPath vPath_AV_Iceblood_Graveyard_to_Iceblood_Graveyard_Flag =
{
    { -536.289f, -397.294f, 49.7357f, nullptr },
    { -549.254f, -386.331f, 50.1129f, nullptr },
    { -558.918f, -388.345f, 50.7527f, nullptr },
    { -571.161f, -397.302f, 52.6324f, nullptr },
    { -590.297f, -409.179f, 56.239f, nullptr },
    { -602.969f, -411.808f, 59.1209f, nullptr },
    { -613.748f, -407.195f, 59.6702f, nullptr },
    { -614.138f, -396.501f, 60.8585f, nullptr },
};

BattleBotPath vPath_AV_Iceblood_Graveyard_to_Iceblood_Tower_Crossroad =
{
    { -536.599f, -373.801f, 49.599f, nullptr },
    { -527.204f, -355.746f, 37.897f, nullptr },
    { -514.888f, -345.189f, 33.675f, nullptr },
    { -579.697f, -315.037f, 46.345f, nullptr },
};

BattleBotPath vPath_AV_Iceblood_Tower_Crossroad_to_Field_of_Strife_Stoneheart_Snowfall_Crossroad =
{
    { -579.697f, -315.037f, 46.345f, nullptr },
    { -572.416f, -317.133f, 43.9943f, nullptr },
    { -559.201f, -326.073f, 39.5171f, nullptr },
    { -544.32f, -335.203f, 37.6494f, nullptr },
    { -520.032f, -342.158f, 34.3473f, nullptr },
    { -486.033f, -344.504f, 29.4772f, nullptr },
    { -462.603f, -340.414f, 31.3555f, nullptr },
    { -455.422f, -336.249f, 30.5176f, nullptr },
    { -450.551f, -332.547f, 25.4498f, nullptr },
    { -440.977f, -326.828f, 18.9782f, nullptr },
    { -421.08f, -321.656f, 17.7909f, nullptr },
    { -398.601f, -318.181f, 18.5789f, nullptr },
    { -387.651f, -312.289f, 21.504f, nullptr },
    { -360.257f, -299.908f, 9.75685f, nullptr },
    { -294.669f, -283.616f, 6.66756f, nullptr },
    { -244.919f, -272.52f, 6.66754f, nullptr },
    { -206.645f, -264.661f, 6.66755f, nullptr },
    { -171.291f, -266.565f, 9.06823f, nullptr },
    { -147.596f, -253.494f, 6.78363f, nullptr },
};

BattleBotPath vPath_AV_Iceblood_Tower_Crossroad_to_Stoneheart_Bunker =
{
    { -579.697f, -315.037f, 46.345f, nullptr },
    { -522.484f, -343.483f, 34.589f, nullptr },
    { -487.608f, -349.201f, 28.785f, nullptr },
    { -452.9f,   -362.035f, 31.530f, nullptr },
    { -434.891f, -355.562f, 32.349f, nullptr },
    { -418.556f, -342.514f, 18.697f, nullptr },
    { -397.991f, -372.594f, 31.355f, nullptr },
    { -366.720f, -394.493f, 10.211f, nullptr },
    { -322.537f, -404.822f, 10.162f, nullptr },
    { -290.379f, -406.149f, 13.998f, nullptr },
    { -269.02f,  -418.522f, 17.358f, nullptr },
    { -244.184f, -433.898f, 20.024f, nullptr },
    { -210.095f, -450.071f, 26.732f, nullptr },
    { -184.418f, -467.245f, 27.624f, nullptr },
    { -162.879f, -478.651f, 28.841f, nullptr },
    { -134.983f, -479.253f, 28.439f, nullptr },
    //{ -104.292f, -455.243f, 22.3564f, nullptr},
};

BattleBotPath vPath_AV_Snowfall_Flag_to_Field_of_Strife_Stoneheart_Snowfall_Crossroad =
{
    { -205.464f, -114.337f, 78.6167f, nullptr },
    { -196.784f, -127.119f, 78.1071f, nullptr },
    { -193.525f, -137.091f, 75.7275f, nullptr },
    { -180.605f, -153.454f, 63.4406f, nullptr },
    { -175.934f, -158.667f, 58.9224f, nullptr },
    { -169.015f, -173.639f, 46.3033f, nullptr },
    { -164.039f, -186.72f, 36.9312f, nullptr },
    { -159.523f, -203.286f, 27.413f, nullptr },
    { -154.95f, -220.108f, 16.6193f, nullptr },
    { -147.742f, -233.671f, 9.55408f, nullptr },
    { -134.536f, -243.891f, 7.96268f, nullptr },
    { -147.596f, -253.494f, 6.78363f, nullptr },
};

BattleBotPath vPath_AV_Field_of_Strife_Stoneheart_Snowfall_Crossroad_to_Stonehearth_Outpost =
{
    { -147.596f, -253.494f, 6.78363f, nullptr },
    { -119.031f, -245.732f, 9.12747f, nullptr },
    { -69.9914f, -241.707f, 8.7055f, nullptr },
    { -41.7233f, -231.875f, 10.2956f, nullptr },
    { -11.2685f, -237.666f, 10.7819f, nullptr },
    { 22.1935f, -244.842f, 14.09f, nullptr },
    { 33.7178f, -258.971f, 14.8231f, nullptr },
    { 29.4264f, -283.735f, 16.3003f, nullptr },
    { 28.1264f, -302.593f, 15.076f, nullptr },
};

BattleBotPath vPath_AV_Stonehearth_Outpost_to_Captain_Balinda_Stonehearth =
{
    { 28.1264f, -302.593f, 15.076f, nullptr },
    { 19.509f, -300.648f, 14.02f, nullptr },
    { 11.9925f, -299.182f, 15.0127f, nullptr },
    { -2.23151f, -296.73f, 15.5632f, nullptr },
    { -20.1345f, -296.292f, 15.5632f, nullptr },
    { -25.8903f, -306.347f, 15.5632f, nullptr },
    { -31.9783f, -309.925f, 15.5632f, nullptr },
    { -46.5667f, -294.841f, 15.0786f, nullptr },
};

BattleBotPath vPath_AV_Stonehearth_Outpost_to_Stonehearth_Bunker_First_Crossroad =
{
    { 28.1264f, -302.593f, 15.076f, nullptr },
    { 5.744f, -329.103f, 16.504f, nullptr },
    { -43.294f, -353.252f, 11.042f, nullptr },
    { -74.443f, -380.701f, 14.543f, nullptr },
    { -95.088f, -418.128f, 16.971f, nullptr },
    { -104.292f, -455.243f, 22.3564f, nullptr },
};

BattleBotPath vPath_AV_Stonehearth_Outpost_to_Stonehearth_Graveyard_Crossroad =
{
    { 28.1264f, -302.593f, 15.076f, nullptr },
    { 26.4279f, -302.732f, 14.7797f, nullptr },
    { 43.9172f, -312.202f, 18.0643f, nullptr },
    { 57.9471f, -317.071f, 25.175f, nullptr },
    { 82.0413f, -321.368f, 33.9446f, nullptr },
    { 102.051f, -333.675f, 39.3055f, nullptr },
    { 117.624f, -352.173f, 42.6668f, nullptr },
    { 121.462f, -367.206f, 43.1468f, nullptr },
    { 123.153f, -375.134f, 42.8991f, nullptr },
};

BattleBotPath vPath_AV_Stonehearth_Graveyard_Crossroad_to_Stonehearth_Graveyard_Flag =
{
    { 123.153f, -375.134f, 42.8991f, nullptr },
    { 101.9f, -389.081f, 45.0974f, nullptr },
    { 89.1516f, -393.047f, 45.1475f, nullptr },
    { 79.8805f, -401.379f, 46.516f, nullptr },
};

BattleBotPath vPath_AV_Stonehearth_Graveyard_to_Stonehearth_Graveyard_Flag =
{
    { 73.8433f, -485.163f, 48.7233f, nullptr },
    { 74.4106f, -469.205f, 48.5722f, nullptr },
    { 74.0364f, -450.204f, 48.7063f, nullptr },
    { 73.3269f, -433.156f, 49.0149f, nullptr },
    { 73.6789f, -417.638f, 48.9345f, nullptr },
    { 79.8805f, -401.379f, 46.516f, nullptr },
};

BattleBotPath vPath_AV_Stonehearth_Graveyard_Flag_to_Stonehearth_Graveyard_Second_Crossroad =
{
    { 79.8805f, -401.379f, 46.516f, nullptr },
    { 59.3904f, -396.459f, 46.3454f, nullptr },
    { 38.213f, -391.053f, 45.6625f, nullptr },
    { 18.563f, -398.416f, 45.6217f, nullptr },
    { -14.0184f, -419.609f, 44.4167f, nullptr },
};

BattleBotPath vPath_AV_Stonehearth_Graveyard_Second_Crossroad_to_Stonehearth_Bunker_First_Crossroad =
{
    { -14.0184f, -419.609f, 44.4167f, nullptr },
    { -16.2311f, -430.739f, 45.9019f, nullptr },
    { -23.0513f, -442.558f, 46.1862f, nullptr },
    { -36.0353f, -454.259f, 45.1569f, nullptr },
    { -47.8724f, -462.469f, 41.2528f, nullptr },
    { -89.1045f, -457.978f, 24.1971f, nullptr },
    { -104.292f, -455.243f, 22.3564f, nullptr },
};

BattleBotPath vPath_AV_Stonehearth_Graveyard_Second_Crossroad_to_Iceblood_Garrison =
{
    { -14.0184f, -419.609f, 44.4167f, nullptr },
    { -23.0625f, -414.929f, 39.5583f, nullptr },
    { -35.4078f, -408.859f, 30.7036f, nullptr },
    { -43.2854f, -396.271f, 21.1548f, nullptr },
    { -52.1465f, -377.477f, 13.6804f, nullptr },
    { -63.422f, -348.549f, 12.6189f, nullptr },
    { -70.7889f, -334.456f, 12.3143f, nullptr },
    { -87.3986f, -321.764f, 10.286f, nullptr },
    { -107.322f, -315.203f, 9.54238f, nullptr },
    { -127.844f, -310.833f, 13.4444f, nullptr },
    { -155.644f, -305.004f, 10.38f, nullptr },
    { -187.766f, -302.183f, 6.66806f, nullptr },
    { -227.675f, -300.517f, 6.66806f, nullptr },
    { -244.583f, -291.433f, 6.66806f, nullptr },
    { -265.603f, -272.943f, 6.66806f, nullptr },
    { -287.897f, -256.006f, 6.71878f, nullptr },
    { -310.325f, -239.243f, 11.8723f, nullptr },
    { -325.404f, -233.386f, 17.9203f, nullptr },
    { -345.71f, -232.887f, 12.1903f, nullptr },
    { -360.847f, -211.82f, 12.329f, nullptr },
    { -372.245f, -196.784f, 12.5248f, nullptr },
    { -391.209f, -185.399f, 15.9205f, nullptr },
    { -404.248f, -184.159f, 20.8763f, nullptr },
    { -425.522f, -195.322f, 26.2692f, nullptr },
    { -436.78f, -199.258f, 29.3854f, nullptr },
    { -464.585f, -196.566f, 49.1431f, nullptr },
    { -492.17f, -187.077f, 57.1342f, nullptr },
};

BattleBotPath vPath_AV_Stonehearth_Graveyard_Second_Crossroad_to_Iceblood_Tower_Crossroad =
{
    { -104.292f, -455.243f, 22.3564f, nullptr },
    { -95.7107f, -437.38f, 18.6408f, nullptr },
    { -96.426f, -395.254f, 14.7414f, nullptr },
    { -117.9f, -376.108f, 12.2937f, nullptr },
    { -170.065f, -344.859f, 10.1405f, nullptr },
    { -220.336f, -329.639f, 8.81655f, nullptr },
    { -273.738f, -342.925f, 6.66864f, nullptr },
    { -307.056f, -315.782f, 6.66756f, nullptr },
    { -329.567f, -296.801f, 6.66756f, nullptr },
    { -354.044f, -289.519f, 9.62051f, nullptr },
    { -384.954f, -283.819f, 12.0969f, nullptr },
    { -411.689f, -272.086f, 16.7252f, nullptr },
    { -432.235f, -273.793f, 20.6022f, nullptr },
    { -452.713f, -277.187f, 21.7067f, nullptr },
    { -489.158f, -284.974f, 28.429f, nullptr },
    { -499.066f, -302.407f, 31.48f, nullptr },
    { -505.393f, -314.896f, 31.9995f, nullptr },
    { -522.97f, -340.616f, 34.5491f, nullptr },
    { -542.124f, -333.426f, 37.6958f, nullptr },
    { -561.111f, -324.477f, 40.0344f, nullptr },
    { -579.697f, -315.037f, 46.345f, nullptr },
};

BattleBotPath vPath_AV_Stonehearth_Bunker_First_Crossroad_to_Stonehearth_Bunker_Flag =
{
    { -104.292f, -455.243f, 22.3564f, nullptr },
    { -111.899f, -466.777f, 24.0451f, nullptr },
    { -122.548f, -480.921f, 26.5748f, nullptr },
    { -133.831f, -478.232f, 28.1725f, nullptr },
    { -128.332f, -462.402f, 26.4943f, nullptr },
    { -156.579f, -449.815f, 29.0267f, nullptr },
    { -168.009f, -444.6f, 33.2796f, nullptr },
    { -160.378f, -440.192f, 33.2796f, nullptr },
    { -154.387f, -445.423f, 33.2796f, nullptr },
    { -159.655f, -458.512f, 40.395f, nullptr },
    { -165.724f, -454.853f, 40.403f, nullptr },
    { -165.652f, -447.139f, 40.403f, nullptr },
    { -161.038f, -440.504f, 40.403f, nullptr },
    { -153.491f, -441.386f, 40.3957f, nullptr },
};

BattleBotPath vPath_AV_Stonehearth_Graveyard_Crossroad_to_Icewing_Bunker_Crossroad =
{
    { 123.153f, -375.134f, 42.8991f, nullptr },
    { 132.848f, -385.475f, 42.2487f, nullptr },
    { 147.934f, -393.887f, 42.6617f, nullptr },
    { 170.279f, -400.056f, 42.802f, nullptr },
    { 209.274f, -410.47f, 42.15f, nullptr },
    { 232.206f, -406.171f, 41.2464f, nullptr },
};

BattleBotPath vPath_AV_Icewing_Bunker_Crossroad_to_Icewing_Bunker_Flag =
{
    { 232.206f, -406.171f, 41.2464f, nullptr },
    { 232.415f, -399.32f, 43.0377f, nullptr },
    { 241.705f, -378.726f, 43.2973f, nullptr },
    { 243.685f, -361.498f, 43.2563f, nullptr },
    { 233.604f, -348.561f, 42.4664f, nullptr },
    { 208.557f, -367.638f, 44.8858f, nullptr },
    { 199.002f, -374.949f, 49.2678f, nullptr },
    { 197.929f, -366.972f, 49.2678f, nullptr },
    { 204.639f, -363.321f, 49.2678f, nullptr },
    { 214.635f, -374.753f, 56.3819f, nullptr },
    { 206.689f, -377.633f, 56.3917f, nullptr },
    { 199.606f, -370.834f, 56.3917f, nullptr },
    { 200.792f, -361.881f, 56.3798f, nullptr },
};

BattleBotPath vPath_AV_Icewing_Bunker_Crossroad_to_Alliance_Slope_Crossroad =
{
    { 232.206f, -406.171f, 41.2464f, nullptr },
    { 245.764f, -414.729f, 34.8094f, nullptr },
    { 260.329f, -406.891f, 26.2452f, nullptr },
    { 277.874f, -393.876f, 11.3203f, nullptr },
    { 292.624f, -385.478f, 3.80607f, nullptr },
    { 315.119f, -384.083f, -0.803525f, nullptr },
    { 352.975f, -389.716f, -0.510339f, nullptr },
    { 383.883f, -393.12f, -1.07409f, nullptr },
    { 401.915f, -389.568f, -1.24385f, nullptr },
};

BattleBotPath vPath_AV_Alliance_Slope_Crossroad_to_Stormpike_Crossroad =
{
    { 401.915f, -389.568f, -1.24385f, nullptr },
    { 411.259f, -385.402f, -1.24337f, nullptr },
    { 424.079f, -380.069f, -1.24337f, nullptr },
    { 462.535f, -368.534f, -1.24387f, nullptr },
    { 508.895f, -330.261f, -1.08467f, nullptr },
    { 517.577f, -324.732f, -1.03504f, nullptr },
    { 536.967f, -321.273f, 3.75218f, nullptr },
    { 557.529f, -324.368f, 15.726f, nullptr },
    { 578.914f, -330.968f, 28.2467f, nullptr },
    { 597.588f, -336.744f, 30.2853f, nullptr },
    { 608.74f, -333.816f, 30.5787f, nullptr },
    { 621.331f, -324.856f, 30.1337f, nullptr },
    { 629.341f, -313.696f, 30.1337f, nullptr },
    { 638.087f, -287.84f, 30.1471f, nullptr },
};

BattleBotPath vPath_AV_Stormpike_Crossroad_to_Alliance_Base_Bunker_First_Crossroad =
{
    { 638.087f, -287.84f, 30.1471f, nullptr },
    { 635.381f, -271.761f, 30.1326f, nullptr },
    { 633.779f, -257.854f, 33.1093f, nullptr },
    { 631.23f, -233.502f, 37.2848f, nullptr },
    { 625.312f, -191.934f, 38.782f, nullptr },
    { 622.182f, -167.638f, 36.3214f, nullptr },
    { 619.956f, -150.28f, 33.3684f, nullptr },
    { 620.254f, -135.032f, 33.4412f, nullptr },
    { 629.777f, -99.9175f, 40.6453f, nullptr },
};

BattleBotPath vPath_AV_Alliance_Base_Bunker_First_Crossroad_to_Alliance_Base_North_Bunker =
{
    { 629.777f, -99.9175f, 40.6453f, nullptr },
    { 635.309f, -97.7424f, 41.9851f, nullptr },
    { 642.07f, -93.7443f, 46.1184f, nullptr },
    { 659.709f, -104.838f, 51.5034f, nullptr },
    { 654.41f, -118.753f, 49.7697f, nullptr },
    { 661.432f, -124.145f, 49.6422f, nullptr },
    { 679.011f, -135.631f, 51.9985f, nullptr },
    { 689.794f, -142.705f, 56.5425f, nullptr },
    { 683.375f, -145.993f, 56.5425f, nullptr },
    { 676.456f, -140.675f, 56.5425f, nullptr },
    { 684.145f, -127.197f, 63.6535f, nullptr },
    { 678.008f, -125.249f, 63.6667f, nullptr },
    { 671.246f, -128.806f, 63.665f, nullptr },
    { 669.384f, -135.545f, 63.6574f, nullptr },
    { 672.685f, -142.49f, 63.6571f, nullptr },
};

BattleBotPath vPath_AV_Alliance_Base_Bunker_First_Crossroad_to_Alliance_Base_Bunker_Second_Crossroad =
{
    { 629.777f, -99.9175f, 40.6453f, nullptr },
    { 633.117f, -67.768f, 41.3917f, nullptr },
};

BattleBotPath vPath_AV_Alliance_Base_Bunker_Second_Crossroad_to_Alliance_Base_South_Bunker =
{
    { 633.117f, -67.768f, 41.3917f, nullptr },
    { 624.951f, -67.4683f, 40.4152f, nullptr },
    { 616.973f, -73.0334f, 38.8073f, nullptr },
    { 596.24f, -89.2897f, 38.855f, nullptr },
    { 591.941f, -86.7649f, 39.5782f, nullptr },
    { 585.983f, -74.2185f, 38.0143f, nullptr },
    { 557.244f, -87.113f, 40.4615f, nullptr },
    { 546.247f, -91.6955f, 44.8191f, nullptr },
    { 548.186f, -82.8881f, 44.8191f, nullptr },
    { 555.216f, -83.2067f, 44.8191f, nullptr },
    { 561.528f, -94.9507f, 51.9364f, nullptr },
    { 567.204f, -90.1402f, 51.9291f, nullptr },
    { 566.935f, -81.8903f, 51.9429f, nullptr },
    { 560.568f, -77.4604f, 51.9305f, nullptr },
    { 555.018f, -77.9842f, 51.9347f, nullptr },
};

BattleBotPath vPath_AV_Alliance_Base_Bunker_Second_Crossroad_to_Alliance_Base_Bunker_Third_Crossroad =
{
    { 633.117f, -67.768f, 41.3917f, nullptr },
    { 635.133f, -51.7416f, 42.3031f, nullptr },
    { 648.593f, -33.8686f, 47.1592f, nullptr },
};

BattleBotPath vPath_AV_Alliance_Base_Bunker_Third_Crossroad_to_Alliance_Base_Flag =
{
    { 648.593f, -33.8686f, 47.1592f, nullptr },
    { 640.404f, -32.0183f, 46.2328f, nullptr },
};

BattleBotPath vPath_AV_Alliance_Base_Bunker_Third_Crossroad_to_Alliance_Base_Vanndar_Stormpike =
{
    { 648.593f, -33.8686f, 47.1592f, nullptr },
    { 664.325f, -28.0147f, 50.6198f, nullptr },
    { 690.605f, -20.6846f, 50.6198f, nullptr },
    { 696.331f, -27.5629f, 50.6198f, nullptr },
    { 699.006f, -31.7397f, 50.6198f, nullptr },
    { 704.958f, -34.5659f, 50.6198f, nullptr },
    { 717.709f, -16.6861f, 50.1354f, nullptr },
};

BattleBotPath vPath_AV_Stormpike_Crossroad_to_Stormpike_Flag =
{
    { 638.087f, -287.84f, 30.1471f, nullptr },
    { 667.173f, -295.225f, 30.29f, nullptr },
};

BattleBotPath vPath_AV_Stormpike_Crossroad_to_Irontooth_Mine_Entrance =
{
    { 638.087f, -287.84f, 30.1471f, nullptr },
    { 665.708f, -283.570f, 29.187f, nullptr },
    { 685.958f, -274.358f, 23.618f, nullptr },
    { 701.911f, -271.504f, 28.409f, nullptr },
    { 715.155f, -276.190f, 36.108f, nullptr },
    { 727.653f, -288.440f, 43.433f, nullptr },
    { 742.260f, -305.039f, 51.649f, nullptr },
    { 753.219f, -314.030f, 56.008f, nullptr },
};

BattleBotPath vPath_AV_Irontooth_Mine_Entrance_to_Irontooth_Mine_Boss =
{
    { 753.219f, -314.030f, 56.008f, nullptr },
    { 772.425f, -310.607f, 54.891f, nullptr },
    { 789.429f, -308.122f, 54.883f, nullptr },
    { 803.955f, -324.896f, 52.259f, nullptr },
    { 820.946f, -341.782f, 49.227f, nullptr },
    { 831.603f, -351.672f, 47.041f, nullptr },
    { 836.755f, -369.026f, 48.233f, nullptr },
    { 827.558f, -386.418f, 47.731f, nullptr },
    { 827.334f, -407.188f, 48.293f, nullptr },
    { 842.166f, -413.971f, 47.832f, nullptr },
    { 861.532f, -422.612f, 51.013f, nullptr },
    { 870.832f, -436.016f, 51.130f, nullptr },
    { 881.273f, -442.002f, 54.664f, nullptr },
};

BattleBotPath vPath_AV_Alliance_Cave_Slop_Crossroad_to_Alliance_Slope_Crossroad =
{
    { 450.8f, -434.864f, 30.5126f, nullptr },
    { 442.575f, -430.266f, 26.6539f, nullptr },
    { 422.36f, -412.85f, 12.285f, nullptr },
    { 401.915f, -389.568f, -1.24385f, nullptr },
};

BattleBotPath vPath_AV_Alliance_Cave_to_Alliance_Cave_Slop_Crossroad =
{
    { 769.016f, -491.165f, 97.7772f, nullptr },
    { 758.026f, -489.447f, 95.9521f, nullptr },
    { 742.169f, -480.684f, 85.9649f, nullptr },
    { 713.063f, -467.311f, 71.0884f, nullptr },
    { 694.957f, -434.171f, 62.8627f, nullptr },
    { 686.386f, -420.276f, 64.5562f, nullptr },
    { 650.413f, -401.791f, 67.9546f, nullptr },
    { 626.412f, -384.308f, 67.5032f, nullptr },
    { 608.756f, -385.851f, 66.5105f, nullptr },
    { 576.021f, -395.976f, 63.5599f, nullptr },
    { 546.555f, -397.812f, 52.655f, nullptr },
    { 514.201f, -416.131f, 42.4508f, nullptr },
    { 450.8f, -434.864f, 30.5126f, nullptr },
};

BattleBotPath vPath_AV_Frostdagger_Pass =
{
    { 28.1264f, -302.593f, 15.076f, nullptr },
    { 30.003f, -272.591f, 16.040f, nullptr },
    { 4.098f, -240.224f, 12.074f, nullptr },
    { -32.843f, -218.981f, 12.105f, nullptr },
    { -48.354f, -201.129f, 18.081f, nullptr },
    { -119.068f, -154.929f, 17.319f, nullptr },
    { -141.732f, -99.318f, 28.793f, nullptr },
    { -196.539f, -60.374f, 35.725f, nullptr },
    { -268.161f, -41.602f, 35.928f, nullptr },
    { -326.121f, -52.747f, 30.200f, nullptr },
    { -367.047f, -81.467f, 26.416f, nullptr },
};

BattleBotPath vPath_AV_Frostdagger_Pass_Lower_to_Iceblood_Garrison =
{
    { -367.047f, -81.467f, 26.416f, nullptr },
    { -380.850f, -111.038f, 23.634f, nullptr },
    { -419.830f, -152.710f, 23.922f, nullptr },
    { -422.173f, -189.060f, 25.747f, nullptr },
    { -492.17f, -187.077f, 57.1342f, nullptr },
};

std::vector<BattleBotPath*> const vPaths_WS =
{
    &vPath_WSG_HordeFlagRoom_to_HordeGraveyard,
    &vPath_WSG_HordeGraveyard_to_HordeTunnel,
    &vPath_WSG_HordeTunnel_to_HordeFlagRoom,
    &vPath_WSG_HordeTunnel_to_AllianceTunnel_1,
    &vPath_WSG_HordeTunnel_to_AllianceTunnel_2,
    &vPath_WSG_HordeGYJump_to_HordeTunnel,
    &vPath_WSG_HordeGYJump_to_AllianceTunnel,
    &vPath_WSG_AllianceFlagRoom_to_AllianceGraveyard,
    &vPath_WSG_AllianceGraveyard_to_AllianceTunnel,
    &vPath_WSG_AllianceTunnel_to_AllianceFlagRoom,
    &vPath_WSG_AllianceGYJump_to_AllianceTunnel,
    &vPath_WSG_AllianceGYJump_to_HordeTunnel,
    &vPath_WSG_HordeGYJump_to_AllianceFlagRoom,
    &vPath_WSG_AllianceGYJump_to_HordeFlagRoom,
    &vPath_WSG_HordeTunnel_to_HordeBaseRoof,
    &vPath_WSG_AllianceTunnel_to_AllianceBaseRoof,
};

std::vector<BattleBotPath*> const vPaths_AB =
{
    &vPath_AB_AllianceBase_to_Stables,
    &vPath_AB_AllianceBase_to_GoldMine,
    &vPath_AB_AllianceBase_to_LumberMill,
    &vPath_AB_Stables_to_Blacksmith,
    &vPath_AB_HordeBase_to_Farm,
    &vPath_AB_HordeBase_to_GoldMine,
    &vPath_AB_HordeBase_to_LumberMill,
    &vPath_AB_Farm_to_Blacksmith,
    &vPath_AB_Stables_to_GoldMine,
    &vPath_AB_Stables_to_LumberMill,
    &vPath_AB_Farm_to_GoldMine,
    &vPath_AB_Farm_to_LumberMill,
};

std::vector<BattleBotPath*> const vPaths_AV =
{
    &vPath_AV_Horde_Cave_to_Tower_Point_Crossroad,
    &vPath_AV_Tower_Point_Crossroads_to_Tower_Point_Bottom,
    //&vPath_AV_TowerPoint_Bottom_to_Tower_Point_Flag,
    &vPath_AV_Tower_Point_Bottom_to_Frostwolf_Graveyard_Flag,
    &vPath_AV_Frostwolf_Graveyard_to_Frostwolf_Graveyard_Flag,
    &vPath_AV_Tower_Point_Crossroads_to_Iceblood_Graveyard_Flag,
    &vPath_AV_Iceblood_Graveyard_Flag_to_Iceblood_Tower_Crossroad,
    &vPath_AV_Iceblood_Tower_to_Iceblood_Garrison,
    &vPath_AV_Iceblood_Garrison_to_Captain_Galvangar,
    &vPath_AV_Iceblood_Graveyard_to_Iceblood_Graveyard_Flag,
    &vPath_AV_Iceblood_Tower_Crossroad_to_Field_of_Strife_Stoneheart_Snowfall_Crossroad,
    &vPath_AV_Stonehearth_Outpost_to_Captain_Balinda_Stonehearth,
    &vPath_AV_Stonehearth_Outpost_to_Stonehearth_Graveyard_Crossroad,
    &vPath_AV_Stonehearth_Graveyard_Crossroad_to_Stonehearth_Graveyard_Flag,
    &vPath_AV_Stonehearth_Graveyard_to_Stonehearth_Graveyard_Flag,
    &vPath_AV_Stonehearth_Graveyard_Flag_to_Stonehearth_Graveyard_Second_Crossroad,
    &vPath_AV_Stonehearth_Graveyard_Second_Crossroad_to_Stonehearth_Bunker_First_Crossroad,
    &vPath_AV_Stonehearth_Graveyard_Second_Crossroad_to_Iceblood_Garrison,
    &vPath_AV_Stonehearth_Graveyard_Second_Crossroad_to_Iceblood_Tower_Crossroad,
    &vPath_AV_Stonehearth_Bunker_First_Crossroad_to_Stonehearth_Bunker_Flag,
    &vPath_AV_Stonehearth_Graveyard_Crossroad_to_Icewing_Bunker_Crossroad,
    &vPath_AV_Icewing_Bunker_Crossroad_to_Icewing_Bunker_Flag,
    &vPath_AV_Icewing_Bunker_Crossroad_to_Alliance_Slope_Crossroad,
    &vPath_AV_Alliance_Cave_Slop_Crossroad_to_Alliance_Slope_Crossroad,
    &vPath_AV_Alliance_Cave_to_Alliance_Cave_Slop_Crossroad,
    &vPath_AV_Horde_Cave_to_Frostwolf_Graveyard_Flag,
    &vPath_AV_Frostwolf_Graveyard_Flag_to_Horde_Base_First_Crossroads,
    &vPath_AV_Horde_Base_First_Crossroads_to_Horde_Base_Second_Crossroads,
    &vPath_AV_Horde_Base_First_Crossroads_to_East_Frostwolf_Tower_Flag,
    &vPath_AV_Horde_Base_First_Crossroads_to_West_Frostwolf_Tower_Flag,
    &vPath_AV_Horde_Base_Second_Crossroads_to_Horde_Base_Entrance_DrekThar,
    &vPath_AV_Horde_Base_Second_Crossroads_to_Horde_Base_DrekThar1,
    &vPath_AV_Horde_Base_Second_Crossroads_to_Horde_Base_DrekThar2,
    &vPath_AV_Horde_Base_Second_Crossroads_to_Horde_Base_Graveyard_Flag,
    &vPath_AV_Iceblood_Garrison_to_Snowfall_Flag,
    &vPath_AV_Snowfall_Flag_to_Field_of_Strife_Stoneheart_Snowfall_Crossroad,
    &vPath_AV_Field_of_Strife_Stoneheart_Snowfall_Crossroad_to_Stonehearth_Outpost,
    &vPath_AV_Alliance_Slope_Crossroad_to_Stormpike_Crossroad,
    &vPath_AV_Stormpike_Crossroad_to_Alliance_Base_Bunker_First_Crossroad,
    &vPath_AV_Stormpike_Crossroad_to_Stormpike_Flag,
    &vPath_AV_Alliance_Base_Bunker_First_Crossroad_to_Alliance_Base_North_Bunker,
    &vPath_AV_Alliance_Base_Bunker_First_Crossroad_to_Alliance_Base_Bunker_Second_Crossroad,
    &vPath_AV_Alliance_Base_Bunker_Second_Crossroad_to_Alliance_Base_South_Bunker,
    &vPath_AV_Alliance_Base_Bunker_Second_Crossroad_to_Alliance_Base_Bunker_Third_Crossroad,
    &vPath_AV_Alliance_Base_Bunker_Third_Crossroad_to_Alliance_Base_Flag,
    &vPath_AV_Alliance_Base_Bunker_Third_Crossroad_to_Alliance_Base_Vanndar_Stormpike,
    &vPath_AV_Iceblood_Tower_Crossroad_to_Iceblood_Tower,
    &vPath_AV_Iceblood_Tower_to_Iceblood_Tower_Flag,
    &vPath_AV_Iceblood_Tower_Crossroad_to_Stoneheart_Bunker,
    &vPath_AV_Stonehearth_Outpost_to_Stonehearth_Bunker_First_Crossroad,
    &vPath_AV_Iceblood_Graveyard_to_Iceblood_Tower_Crossroad,
    &vPath_AV_Frostdagger_Pass,
    &vPath_AV_Frostdagger_Pass_Lower_to_Iceblood_Garrison,
    //&vPath_AV_Frostwolf_Graveyard_Flag_to_Coldtooth_Mine_Entrance,
    //&vPath_AV_Coldtooth_Mine_Entrance_to_Coldtooth_Mine_Boss,
    //&vPath_AV_Stormpike_Crossroad_to_Irontooth_Mine_Entrance,
    //&vPath_AV_Irontooth_Mine_Entrance_to_Irontooth_Mine_Boss,
};

std::vector<BattleBotPath*> const vPaths_NoReverseAllowed =
{
    &vPath_AV_Horde_Cave_to_Tower_Point_Crossroad,
    &vPath_AV_Frostwolf_Graveyard_to_Frostwolf_Graveyard_Flag,
    &vPath_AV_Iceblood_Graveyard_to_Iceblood_Graveyard_Flag,
    &vPath_AV_Stonehearth_Graveyard_to_Stonehearth_Graveyard_Flag,
    &vPath_AV_Alliance_Cave_to_Alliance_Cave_Slop_Crossroad,
    &vPath_AV_Horde_Cave_to_Frostwolf_Graveyard_Flag,
    &vPath_AV_Alliance_Cave_Slop_Crossroad_to_Alliance_Slope_Crossroad,
    &vPath_AV_Iceblood_Graveyard_to_Iceblood_Tower_Crossroad,
};

std::vector<BattleBotPath*> const vPaths_AllyMine =
{
    &vPath_AV_Stormpike_Crossroad_to_Irontooth_Mine_Entrance,
    &vPath_AV_Irontooth_Mine_Entrance_to_Irontooth_Mine_Boss,
};

std::vector<BattleBotPath*> const vPaths_HordeMine =
{
    &vPath_AV_Frostwolf_Graveyard_Flag_to_Coldtooth_Mine_Entrance,
    &vPath_AV_Coldtooth_Mine_Entrance_to_Coldtooth_Mine_Boss,
};

static std::pair<uint32, uint32> AV_HordeAttackObjectives[] =
{
    // Attack
    { BG_AV_NODES_STONEHEART_BUNKER, BG_AV_NODE_STATUS_ALLY_OCCUPIED },
    { BG_AV_NODES_STONEHEART_GRAVE, BG_AV_NODE_STATUS_ALLY_OCCUPIED },
    { BG_AV_NODES_ICEWING_BUNKER, BG_AV_NODE_STATUS_ALLY_OCCUPIED },
    { BG_AV_NODES_STORMPIKE_GRAVE, BG_AV_NODE_STATUS_ALLY_OCCUPIED },
    { BG_AV_NODES_DUNBALDAR_SOUTH, BG_AV_NODE_STATUS_ALLY_OCCUPIED },
    { BG_AV_NODES_DUNBALDAR_NORTH, BG_AV_NODE_STATUS_ALLY_OCCUPIED },
    { BG_AV_NODES_FIRSTAID_STATION, BG_AV_NODE_STATUS_ALLY_OCCUPIED }
};

static std::pair<uint32, uint32> AV_HordeDefendObjectives[] =
{
    // Defend
    { BG_AV_NODES_FROSTWOLF_GRAVE, BG_AV_NODE_STATUS_ALLY_CONTESTED },
    { BG_AV_NODES_FROSTWOLF_ETOWER, BG_AV_NODE_STATUS_ALLY_CONTESTED },
    { BG_AV_NODES_FROSTWOLF_WTOWER, BG_AV_NODE_STATUS_ALLY_CONTESTED },
    { BG_AV_NODES_TOWER_POINT, BG_AV_NODE_STATUS_ALLY_CONTESTED },
    { BG_AV_NODES_ICEBLOOD_TOWER, BG_AV_NODE_STATUS_ALLY_CONTESTED },
};

static std::pair<uint32, uint32> AV_AllianceAttackObjectives[] =
{
    // Attack
    { BG_AV_NODES_ICEBLOOD_TOWER, BG_AV_NODE_STATUS_HORDE_OCCUPIED },
    { BG_AV_NODES_ICEBLOOD_GRAVE, BG_AV_NODE_STATUS_HORDE_OCCUPIED },
    { BG_AV_NODES_TOWER_POINT, BG_AV_NODE_STATUS_HORDE_OCCUPIED },
    { BG_AV_NODES_FROSTWOLF_GRAVE, BG_AV_NODE_STATUS_HORDE_OCCUPIED },
    { BG_AV_NODES_FROSTWOLF_ETOWER, BG_AV_NODE_STATUS_HORDE_OCCUPIED },
    { BG_AV_NODES_FROSTWOLF_WTOWER, BG_AV_NODE_STATUS_HORDE_OCCUPIED },
    { BG_AV_NODES_FROSTWOLF_HUT, BG_AV_NODE_STATUS_HORDE_OCCUPIED },
};

static std::pair<uint32, uint32> AV_AllianceDefendObjectives[] =
{
    // Defend
    { BG_AV_NODES_STORMPIKE_GRAVE, BG_AV_NODE_STATUS_HORDE_CONTESTED },
    { BG_AV_NODES_DUNBALDAR_SOUTH, BG_AV_NODE_STATUS_HORDE_CONTESTED },
    { BG_AV_NODES_DUNBALDAR_NORTH, BG_AV_NODE_STATUS_HORDE_CONTESTED },
    { BG_AV_NODES_ICEWING_BUNKER, BG_AV_NODE_STATUS_HORDE_CONTESTED },
    { BG_AV_NODES_STONEHEART_BUNKER, BG_AV_NODE_STATUS_HORDE_CONTESTED },
};

static std::pair<uint32, uint32> AB_AttackObjectives[] =
{
    // Attack
    { BG_AB_NODE_STABLES, BG_AB_BANNER_STABLE },
    { BG_AB_NODE_BLACKSMITH, BG_AB_BANNER_BLACKSMITH },
    { BG_AB_NODE_FARM, BG_AB_BANNER_FARM },
    { BG_AB_NODE_LUMBER_MILL, BG_AB_BANNER_LUMBER_MILL },
    { BG_AB_NODE_GOLD_MINE, BG_AB_BANNER_MINE }
};

//
// old wsg waypoints
//

//cross the BattleGround to get to flags or flag carriers

bool BGTactics::wsgPaths()
{
    BattleGround *bg = bot->GetBattleGround();
    if (!bg)
        return false;

    ai::PositionEntry pos = context->GetValue<ai::PositionMap&>("position")->Get()["bg objective"];

    uint32 Preference = context->GetValue<uint32>("bg role")->Get();


    if (pos.x > bot->GetPositionX()) //He's somewhere at the alliance side
    {
        if (Preference < 4) //preference < 4 = move through tunnel (< 6 becuse GY disabled)
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
        else if (Preference > 7) { // preference < 7 = move through graveyard (BUGGED)
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
                return  true;
            }
        }
    }
    return false;
}


//
// actual bg tactics below
//

bool BGTactics::Execute(Event event)
{
    BattleGround *bg = bot->GetBattleGround();
    if (!bg)
        return false;

    if (bg->GetStatus() == STATUS_WAIT_LEAVE)
        return false;

    // disable buffin during BG to save mana
    if (bg->GetStatus() == STATUS_IN_PROGRESS)
        ai->ChangeStrategy("-buff", BOT_STATE_NON_COMBAT);

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
        uint32 role = context->GetValue<uint32>("bg role")->Get();

        int startSpot = role < 4 ? BB_WSG_WAIT_SPOT_LEFT : role > 6 ? BB_WSG_WAIT_SPOT_RIGHT : BB_WSG_WAIT_SPOT_SPAWN;
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
            bool endBoss = false;
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
                    endBoss = true;
                    ostringstream out; out << "Attacking Vanndar!";
                    //bot->Say(out.str(), LANG_UNIVERSAL);
                }
            }

            uint32 role = context->GetValue<uint32>("bg role")->Get();
            bool supporter = role < 4; // first bunker strike team

            // Only go to Snowfall Graveyard if already close to it.
            // Need to fix AV script
            if (!BgObjective && supporter && (bg->IsActiveEvent(BG_AV_NODES_SNOWFALL_GRAVE, BG_AV_NODE_STATUS_ALLY_CONTESTED) || bg->IsActiveEvent(BG_AV_NODES_SNOWFALL_GRAVE, BG_AV_NODE_STATUS_ALLY_OCCUPIED) || bg->IsActiveEvent(BG_AV_NODES_SNOWFALL_GRAVE, BG_AV_NODE_STATUS_NEUTRAL_OCCUPIED)))
            {
                if (GameObject* pGO = bot->GetMap()->GetGameObject(bg->GetSingleGameObjectGuid(BG_AV_NODES_SNOWFALL_GRAVE, BG_AV_NODE_STATUS_NEUTRAL_OCCUPIED)))
                    if (bot->IsWithinDist(pGO, VISIBILITY_DISTANCE_LARGE))
                    {
                        BgObjective = pGO;
                        ostringstream out; out << "Attacking Snowfall GY!";
                        //bot->Say(out.str(), LANG_UNIVERSAL);
                    }
            }

            if (!BgObjective && !bg->IsActiveEvent(BG_AV_NODE_CAPTAIN_DEAD_A, 0))
            {
                if (Creature* pBalinda = bot->GetMap()->GetCreature(bg->GetSingleCreatureGuid(BG_AV_CAPTAIN_A, 0)))
                {
                    if (pBalinda->GetDeathState() != DEAD)
                    {
                        uint32 attackCount = 0;
                        attackCount += getDefendersCount(AV_STONEHEARTH_WAITING_HORDE, 10.0f, false);

                        // prepare to attack Captain
                        if (attackCount < 10 && !sServerFacade.IsInCombat(pBalinda))
                        {
                            // get in position to attack Captain
                            pos.Set(AV_STONEHEARTH_WAITING_HORDE.x, AV_STONEHEARTH_WAITING_HORDE.y, AV_STONEHEARTH_WAITING_HORDE.z, bg->GetMapId());

                            ostringstream out; out << "Taking position at Stonehearth!";
                            //bot->Say(out.str(), LANG_UNIVERSAL);
                        }
                        else
                        {
                            ostringstream out; out << "Attacking Balinda!";
                            //bot->Say(out.str(), LANG_UNIVERSAL);
                        }
                        BgObjective = pBalinda;

                        // set skull on captain
                        //if (bot->IsWithinDistInMap(pBalinda, 50.0f))
                        //{
                        //    bot->GetGroup()->SetTargetIcon(7, pBalinda->GetObjectGuid());
                        //}
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
                                //bot->Say(out.str(), LANG_UNIVERSAL);
                            }
                    }
                }
            }

            // Mine capture (need paths & script fix)
            /*if (!BgObjective && supporter && !endBoss && (bg->IsActiveEvent(BG_AV_MINE_BOSSES_NORTH, TEAM_INDEX_ALLIANCE) || bg->IsActiveEvent(BG_AV_MINE_BOSSES_NORTH, TEAM_INDEX_NEUTRAL)) &&
                !bg->IsActiveEvent(BG_AV_NODES_STORMPIKE_GRAVE, BG_AV_NODE_STATUS_ALLY_OCCUPIED))
            {
                if (Creature* mBossNeutral = bot->GetMap()->GetCreature(bg->GetSingleCreatureGuid(BG_AV_MINE_BOSSES_NORTH, TEAM_INDEX_NEUTRAL)))
                {
                    if (mBossNeutral->GetDeathState() != DEAD && bg->IsActiveEvent(BG_AV_MINE_BOSSES_NORTH, TEAM_INDEX_NEUTRAL))
                    {
                        BgObjective = mBossNeutral;
                        ostringstream out; out << "Attacking Neutral Mine Boss!";
                        bot->Say(out.str(), LANG_UNIVERSAL);
                    }
                }

                if (Creature* mBossAlly = bot->GetMap()->GetCreature(bg->GetSingleCreatureGuid(BG_AV_MINE_BOSSES_NORTH, TEAM_INDEX_ALLIANCE)))
                {
                    if (!BgObjective && mBossAlly->GetDeathState() != DEAD && bg->IsActiveEvent(BG_AV_MINE_BOSSES_NORTH, TEAM_INDEX_ALLIANCE))
                    {
                        BgObjective = mBossAlly;
                        ostringstream out; out << "Attacking Ally Mine Boss!";
                        bot->Say(out.str(), LANG_UNIVERSAL);
                    }
                }
            }*/

            //bool strifeTime = bg->GetStartTime() < (uint32)(5 * MINUTE * IN_MILLISECONDS);
            // small strike team to first bunker
            if (!BgObjective/* || (!endBoss && supporter)*/)
            {
                for (const auto& objective : AV_HordeAttackObjectives)
                {
                    if ((!BgObjective/* || (supporter && objective.first == BG_AV_NODES_STONEHEART_BUNKER && !bg->IsActiveEvent(BG_AV_NODE_CAPTAIN_DEAD_A, 0))*/) &&
                        (bg->IsActiveEvent(objective.first, BG_AV_NODE_STATUS_ALLY_CONTESTED) ||
                            bg->IsActiveEvent(objective.first, BG_AV_NODE_STATUS_ALLY_OCCUPIED) ||
                            bg->IsActiveEvent(objective.first, BG_AV_NODE_STATUS_NEUTRAL_OCCUPIED)))
                    {
                        if (GameObject* pGO = bot->GetMap()->GetGameObject(bg->GetSingleGameObjectGuid(objective.first, objective.second)))
                        {
                            BgObjective = pGO;
                            //ostringstream out; out << "Attacking Node #" << objective.first;
                            //bot->Say(out.str(), LANG_UNIVERSAL);
                        }
                    }
                }
            }
        }
        else // ALLIANCE
        {
            bool endBoss = false;
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
                    endBoss = true;
                    ostringstream out; out << "Attacking DrekThar!";
                    //bot->Say(out.str(), LANG_UNIVERSAL);
                }
            }

            uint32 role = context->GetValue<uint32>("bg role")->Get();
            bool supporter = role < 4;

            // Only go to Snowfall Graveyard if already close to it.
            if (!BgObjective && supporter && (bg->IsActiveEvent(BG_AV_NODES_SNOWFALL_GRAVE, BG_AV_NODE_STATUS_HORDE_CONTESTED) || bg->IsActiveEvent(BG_AV_NODES_SNOWFALL_GRAVE, BG_AV_NODE_STATUS_HORDE_OCCUPIED) || bg->IsActiveEvent(BG_AV_NODES_SNOWFALL_GRAVE, BG_AV_NODE_STATUS_NEUTRAL_OCCUPIED)))
            {
                if (GameObject* pGO = bot->GetMap()->GetGameObject(bg->GetSingleGameObjectGuid(BG_AV_NODES_SNOWFALL_GRAVE, BG_AV_NODE_STATUS_NEUTRAL_OCCUPIED)))
                    if (bot->IsWithinDist(pGO, VISIBILITY_DISTANCE_LARGE))
                    {
                        BgObjective = pGO;
                        ostringstream out; out << "Attacking Snowfall GY!";
                        //bot->Say(out.str(), LANG_UNIVERSAL);
                    }
            }

            // Chance to defend.
            if (!BgObjective && (!endBoss && supporter))
            {
                for (const auto& objective : AV_AllianceDefendObjectives)
                {
                    if (!BgObjective && bg->IsActiveEvent(objective.first, BG_AV_NODE_STATUS_HORDE_CONTESTED))
                    {
                        if (GameObject* pGO = bot->GetMap()->GetGameObject(bg->GetSingleGameObjectGuid(objective.first, objective.second)))
                        {
                            BgObjective = pGO;
                            //ostringstream out; out << "Defending Node #" << objective.first;
                            //bot->Say(out.str(), LANG_UNIVERSAL);
                        }
                    }
                }
            }

            // Mine capture (need paths & script fix)
            /*if (!BgObjective && supporter && !endBoss && (bg->IsActiveEvent(BG_AV_MINE_BOSSES_SOUTH, TEAM_INDEX_HORDE) || bg->IsActiveEvent(BG_AV_MINE_BOSSES_SOUTH, TEAM_INDEX_NEUTRAL)) &&
                !bg->IsActiveEvent(BG_AV_NODES_FROSTWOLF_GRAVE, BG_AV_NODE_STATUS_HORDE_OCCUPIED))
            {
                if (Creature* mBossNeutral = bot->GetMap()->GetCreature(bg->GetSingleCreatureGuid(BG_AV_MINE_BOSSES_SOUTH, TEAM_INDEX_NEUTRAL)))
                {
                    if (mBossNeutral->GetDeathState() != DEAD && bg->IsActiveEvent(BG_AV_MINE_BOSSES_SOUTH, TEAM_INDEX_NEUTRAL))
                    {
                        BgObjective = mBossNeutral;
                        ostringstream out; out << "Attacking Neutral Mine Boss!";
                        bot->Say(out.str(), LANG_UNIVERSAL);
                    }
                }

                if (Creature* mBossHorde = bot->GetMap()->GetCreature(bg->GetSingleCreatureGuid(BG_AV_MINE_BOSSES_SOUTH, TEAM_INDEX_HORDE)))
                {
                    if (!BgObjective && mBossHorde->GetDeathState() != DEAD && bg->IsActiveEvent(BG_AV_MINE_BOSSES_SOUTH, TEAM_INDEX_HORDE))
                    {
                        BgObjective = mBossHorde;
                        ostringstream out; out << "Attacking Horde Mine Boss!";
                        bot->Say(out.str(), LANG_UNIVERSAL);
                    }
                }
            }*/

            if (!BgObjective)
            {
                // Attack closest objective.
                WorldObject* pAttackObjectiveObject = nullptr;
                float attackObjectiveDistance = FLT_MAX;

                if (!bg->IsActiveEvent(BG_AV_NODE_CAPTAIN_DEAD_H, 0))
                {
                    if (Creature* pGalvangar = bot->GetMap()->GetCreature(bg->GetSingleCreatureGuid(BG_AV_CAPTAIN_H, 0)))
                    {
                        uint32 attackCount = 0;
                        attackCount += getDefendersCount(AV_ICEBLOOD_GARRISON_WAITING_ALLIANCE, 10.0f, false);

                        // prepare to attack Captain
                        if (attackCount < 10 && !sServerFacade.IsInCombat(pGalvangar))
                        {
                            // get in position to attack Captain
                            pos.Set(AV_ICEBLOOD_GARRISON_WAITING_ALLIANCE.x, AV_ICEBLOOD_GARRISON_WAITING_ALLIANCE.y, AV_ICEBLOOD_GARRISON_WAITING_ALLIANCE.z, bg->GetMapId());

                            //ostringstream out; out << "Taking position at Iceblood Outpost!";
                            //bot->Say(out.str(), LANG_UNIVERSAL);
                        }
                        else
                        {
                            //ostringstream out; out << "Attacking Galvangar!";
                            //bot->Say(out.str(), LANG_UNIVERSAL);
                        }
                        pAttackObjectiveObject = pGalvangar;
                        attackObjectiveDistance = sqrt(bot->GetDistance(pGalvangar));
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
                                //ostringstream out; out << "Attacking Node #" << objective.first;
                                //bot->Say(out.str(), LANG_UNIVERSAL);
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
            if (!pos.isSet())
                pos.Set(BgObjective->GetPositionX(), BgObjective->GetPositionY(), BgObjective->GetPositionZ(), BgObjective->GetMapId());

            posMap["bg objective"] = pos;

            //ostringstream out;
            //out << "BG objective set to " << BgObjective->GetName() << " " << pos.x << " " << pos.y;
            //bot->Say(out.str(), LANG_UNIVERSAL);
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
                    //Position hidePos = WS_FLAG_HIDE_ALLIANCE[urand(0, 2)];
                    Position hidePos = WS_FLAG_HIDE_ALLIANCE[0];
                    pos.Set(hidePos.x, hidePos.y, hidePos.z, bot->GetMapId());
                }
                else
                    pos.Set(WS_FLAG_POS_ALLIANCE.x, WS_FLAG_POS_ALLIANCE.y, WS_FLAG_POS_ALLIANCE.z, bot->GetMapId());
            }
            else
            {
                if (teamFlagTaken())
                {
                    //Position hidePos = WS_FLAG_HIDE_HORDE[urand(0, 2)];
                    Position hidePos = WS_FLAG_HIDE_HORDE[0];
                    pos.Set(hidePos.x, hidePos.y, hidePos.z, bot->GetMapId());
                }
                else
                    pos.Set(WS_FLAG_POS_HORDE.x, WS_FLAG_POS_HORDE.y, WS_FLAG_POS_HORDE.z, bot->GetMapId());
            }

            //ostringstream out;
            //if (teamFlagTaken())
            //    out << "Hiding with flag" << (bot->GetTeam() == ALLIANCE ? "in Alliance base" : "in Horde base");
            //else
            //    out << "Taking " << (bot->GetTeam() == ALLIANCE ? "Horde flag to base" : "Alliance flag to base");
            //bot->Say(out.str(), LANG_UNIVERSAL);
        }
        else
        {
            uint32 role = context->GetValue<uint32>("bg role")->Get();
            bool supporter = role < 4;

            //ostringstream out;
            //out << "Role: " << role;
            //bot->Say(out.str(), LANG_UNIVERSAL);

            if (supporter)
            {
                Unit* teamFC = AI_VALUE(Unit*, "team flag carrier");
                if (teamFC)
                {
                    //ostringstream out;
                    //out << "Protecting " << (bot->GetTeam() == ALLIANCE ? "Alliance FC" : "Horde FC");
                    //bot->Say(out.str(), LANG_UNIVERSAL);
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

                        //ostringstream out;
                        //out << "Attacking " << (bot->GetTeam() == ALLIANCE ? "Horde FC" : "Alliance FC");
                        //bot->Say(out.str(), LANG_UNIVERSAL);
                    }
                    else
                    {
                        if (bot->GetTeam() == ALLIANCE)
                            pos.Set(WS_FLAG_POS_HORDE.x, WS_FLAG_POS_HORDE.y, WS_FLAG_POS_HORDE.z, bot->GetMapId());
                        else
                            pos.Set(WS_FLAG_POS_ALLIANCE.x, WS_FLAG_POS_ALLIANCE.y, WS_FLAG_POS_ALLIANCE.z, bot->GetMapId());

                        //ostringstream out;
                        //out << "Going to " << (bot->GetTeam() == ALLIANCE ? "take Horde flag" : "take Alliance flag");
                        //bot->Say(out.str(), LANG_UNIVERSAL);
                    }
                }
            }
            else
            {
                Unit* enemyFC = AI_VALUE(Unit*, "enemy flag carrier");
                if (enemyFC)
                {
                    pos.Set(enemyFC->GetPositionX(), enemyFC->GetPositionY(), enemyFC->GetPositionZ(), bot->GetMapId());

                    //ostringstream out;
                    //out << "Attacking " << (bot->GetTeam() == ALLIANCE ? "Horde FC" : "Alliance FC");
                    //bot->Say(out.str(), LANG_UNIVERSAL);
                }
                else
                {
                    if (role > 9)  // test patrol
                    {
                        float rx, ry, rz;
                        bot->GetRandomPoint(1227.446f, 1476.235f, 307.484f, 150.0f, rx, ry, rz);
                        pos.Set(rx, ry, rz, bot->GetMapId());
                        //ostringstream out;
                        //out << "Patrolling battlefield";
                        //bot->Say(out.str(), LANG_UNIVERSAL);
                    }
                    else
                    {
                        if (bot->GetTeam() == ALLIANCE)
                            pos.Set(WS_FLAG_POS_HORDE.x, WS_FLAG_POS_HORDE.y, WS_FLAG_POS_HORDE.z, bot->GetMapId());
                        else
                            pos.Set(WS_FLAG_POS_ALLIANCE.x, WS_FLAG_POS_ALLIANCE.y, WS_FLAG_POS_ALLIANCE.z, bot->GetMapId());

                        //ostringstream out;
                        //out << "Going to " << (bot->GetTeam() == ALLIANCE ? "take Horde flag" : "take Alliance flag");
                        //bot->Say(out.str(), LANG_UNIVERSAL);
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
                                    //ostringstream out; out << "Possible Attack Point #" << objective.first;
                                    //bot->Say(out.str(), LANG_UNIVERSAL);
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
                                //ostringstream out; out << "Possible Attack Point #" << objective.first;
                                //bot->Say(out.str(), LANG_UNIVERSAL);
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
            //ostringstream out;
            //out << "BG objective set to " << BgObjective->GetName() << " " << pos.x << " " << pos.y;
            //bot->Say(out.str(), LANG_UNIVERSAL);
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
        if (sServerFacade.IsDistanceGreaterThan(sServerFacade.GetDistance2d(bot, pos.x, pos.y), 100.0f))
        {
            //ostringstream out; out << "It is too far away! " << pos.x << ", " << pos.y << ", Distance: " << sServerFacade.GetDistance2d(bot, pos.x, pos.y);
            //bot->Say(out.str(), LANG_UNIVERSAL);
            return false;
        }

        // don't try to move if already close
        if (sqrt(bot->GetDistance(pos.x, pos.y, pos.z, DIST_CALC_NONE)) < 5.0f)
        {
            resetObjective();

            return true;
        }

        //ostringstream out; out << "Moving to objective " << pos.x << ", " << pos.y << ", Distance: " << sServerFacade.GetDistance2d(bot, pos.x, pos.y);
        //bot->Say(out.str(), LANG_UNIVERSAL);

        // more precise position for wsg
        if (bg->GetTypeId() == BATTLEGROUND_WS)
            return MoveTo(bot->GetMapId(), pos.x, pos.y, pos.z);
        else
            return MoveNear(bot->GetMapId(), pos.x, pos.y, pos.z, 3.0f);
    }
    return false;
}

bool BGTactics::selectObjectiveWp(std::vector<BattleBotPath*> const& vPaths)
{
    BattleGround *bg = bot->GetBattleGround();
    if (!bg)
        return false;

    ai::PositionMap& posMap = context->GetValue<ai::PositionMap&>("position")->Get();
    ai::PositionEntry pos = context->GetValue<ai::PositionMap&>("position")->Get()["bg objective"];
    if (!pos.isSet())
        return false;

    // use Rym's waypoints for WSG
    if (bg->GetTypeId() == BATTLEGROUND_WS && (bot->HasAura(BG_WS_SPELL_WARSONG_FLAG) || bot->HasAura(BG_WS_SPELL_SILVERWING_FLAG)))
        return wsgPaths();

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

        // skip no reverse paths
        if (std::find(vPaths_NoReverseAllowed.begin(), vPaths_NoReverseAllowed.end(), pPath) != vPaths_NoReverseAllowed.end())
            continue;

        // skip mine paths of own faction
        if (bot->GetTeam() == ALLIANCE && std::find(vPaths_AllyMine.begin(), vPaths_AllyMine.end(), pPath) != vPaths_AllyMine.end())
            continue;
        if (bot->GetTeam() == HORDE && std::find(vPaths_HordeMine.begin(), vPaths_HordeMine.end(), pPath) != vPaths_HordeMine.end())
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
    BattleGround *bg = bot->GetBattleGround();
    if (!bg)
        return false;

    // sometimes change role
    if (!urand(0, 5) && !(bot->HasAura(BG_WS_SPELL_WARSONG_FLAG) || bot->HasAura(BG_WS_SPELL_SILVERWING_FLAG)))
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
        //ostringstream out; out << "Reached path end!";
        //bot->Say(out.str(), LANG_UNIVERSAL);
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

        // skip mine paths of own faction
        if (bot->GetTeam() == ALLIANCE && std::find(vPaths_AllyMine.begin(), vPaths_AllyMine.end(), pPath) != vPaths_AllyMine.end())
            continue;
        if (bot->GetTeam() == HORDE && std::find(vPaths_HordeMine.begin(), vPaths_HordeMine.end(), pPath) != vPaths_HordeMine.end())
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

            //ostringstream out; out << "Flag is nearby, using " << go->GetName();
            //bot->Say(out.str(), LANG_UNIVERSAL);
            //ai->SetNextCheckDelay(10000);

            // cast banner spell
            SpellEntry const *spellInfo = sServerFacade.LookupSpellInfo(SPELL_CAPTURE_BANNER);
            if (!spellInfo)
                return false;

            Spell *spell = new Spell(bot, spellInfo, false);
            spell->m_targets.setGOTarget(go);
            spell->SpellStart(&spell->m_targets);
            ai->WaitForSpellCast(spell);

            //WorldPacket data(CMSG_GAMEOBJ_USE);
            //data << go->GetObjectGuid();
            //bot->GetSession()->HandleGameObjectUseOpcode(data);
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
                    //ostringstream out; out << "Capturing flag!";
                    //bot->Say(out.str(), LANG_UNIVERSAL);
                    return true;
                }

                if (bot->IsMounted())
                    bot->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

                if (bot->IsInDisallowedMountForm())
                    bot->RemoveSpellsCausingAura(SPELL_AURA_MOD_SHAPESHIFT);

                //ostringstream out; out << "Flag is nearby, using " << go->GetName();
                //bot->Say(out.str(), LANG_UNIVERSAL);
                WorldPacket data(CMSG_GAMEOBJ_USE);
                data << go->GetObjectGuid();
                bot->GetSession()->HandleGameObjectUseOpcode(data);

                resetObjective();
                return true;
            }
            else
            {
                //ostringstream out; out << "Flag is far, moving to " << go->GetName() << " " << go->GetPositionX() << " " << go->GetPositionY() << " Distance:" << sServerFacade.GetDistance2d(bot, go->GetPositionX(), go->GetPositionY());
                //bot->Say(out.str(), LANG_UNIVERSAL);
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
            //ostringstream out; out << "Moving to buff...";
            //bot->Say(out.str(), LANG_UNIVERSAL);

            return MoveTo(go->GetMapId(), go->GetPositionX(), go->GetPositionY(), go->GetPositionZ());
        }
    }
    return false;
}

uint32 BGTactics::getDefendersCount(Position point, float range, bool combat)
{
    uint32 defCount = 0;

    if (!bot->InBattleGround())
        return false;

    BattleGround *bg = bot->GetBattleGround();
    if (!bg)
        return 0;

    for (auto& guid : bg->GetBgMap()->GetPlayers())
    {
        Player* player = guid.getSource();
        if (!player)
            continue;

        if (sServerFacade.IsAlive(player) && player->GetTeam() == bot->GetTeam())
        {
            if (!combat && sServerFacade.IsInCombat(player))
                continue;

            if (sServerFacade.GetDistance2d(player, point.x, point.y) < range)
                ++defCount;
        }
    }

    return defCount;
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
