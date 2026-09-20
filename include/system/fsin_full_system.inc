// ============================================================================
// SIME RUSSIA � FSIN FULL SYSTEM EXPORT
// Extracted/assembled from SIME_RUSSIA_MECHANIC_CENTER_V5_ACTIONS_FULL_FINAL
// Encoding of fsin_full_system.inc: Windows-1251 (CP1251)
//
// Scope:
// - TEAM_FSIN runtime module (team id 11)
// - full exterior FSIN scene found in source (8 objects)
// - 2 controlled gates + horn/Y/notification interaction + autoclose
// - FSIN HR actor + requirements + 6 RP questions + rank 1 recruitment
// - wardrobe / workday / uniform switching
// - 6-car FSIN fleet, rank limits, exact 2 parking slots, vinyls, RU plates 52,
//   siren, stroboscope and tint
// - armory with original 9 items/cooldowns
// - lazaret/camera/sawmill portal pickups
// - FSIN jail storage/helpers/tick hooks
// - original visual NPC "Igla" and common-stash label present in FSIN scene
//
// IMPORTANT: the donor SIME base owns account/org enums and vehicle wrappers.
// Apply integration/BASE_PATCHES_SIME_RUSSIA.pwn and callback hooks once.
// ============================================================================

#if defined _SIME_FSIN_FULL_SYSTEM_INCLUDED
    #endinput
#endif
#define _SIME_FSIN_FULL_SYSTEM_INCLUDED

#define FSIN_FULL_TEAM_ID             (11)
#define FSIN_FULL_HR_MIN_LEVEL        (3)
#define FSIN_FULL_HR_MAX_ERRORS       (2)
#define FSIN_FULL_HR_RETRY_SECONDS    (180)
#define FSIN_FULL_HR_NOTIFY_ID        (9321)
#define FSIN_FULL_GATE_NOTIFY_ID      (9322)
#define FSIN_FULL_FLEET_COUNT         (6)
#define FSIN_FULL_NUMBER_REGION       "52"
#define FSIN_FULL_SAWMILL_COUNT       (10)
#define FSIN_FULL_SAWMILL_BOARDS      (6)
#define FSIN_FULL_SAWMILL_REWARD      (200)
#define FSIN_FULL_SAWMILL_TIME_REWARD (30)

#define DIALOG_FSIN_FULL_HR_INFO      (30410)
#define DIALOG_FSIN_FULL_HR_Q1        (30411)
#define DIALOG_FSIN_FULL_HR_Q2        (30412)
#define DIALOG_FSIN_FULL_HR_Q3        (30413)
#define DIALOG_FSIN_FULL_HR_Q4        (30414)
#define DIALOG_FSIN_FULL_HR_Q5        (30415)
#define DIALOG_FSIN_FULL_HR_Q6        (30416)
#define DIALOG_FSIN_FULL_HR_RESULT    (30417)
#define DIALOG_FSIN_FULL_FLEET        (30430)
#define DIALOG_FSIN_FULL_WORK_START   (30431)
#define DIALOG_FSIN_FULL_WORK_END     (30432)
#define DIALOG_FSIN_FULL_ARMORY       (30433)

forward ShowNotificationNew(playerid, type, duration, id, subId, caption[], btnCaption[]);

stock FsinFull_ShowSimpleNotification(playerid,const message[])
{
    return SendClientMessage(playerid,0xCECECEFF,message);
}

// Organization data copied from the donor base. The main organization table must
// still contain the FSIN row from BASE_PATCHES_SIME_RUSSIA.pwn.
new const g_FsinFullMaleSkins[10] = {32,4,26,26,26,26,26,26,26,26};
#define FSIN_FULL_WOMAN_SKIN (64)
new const Float:g_FsinFullSpawn[4] = {-1876.63,-2727.15,8.58,227.22};
new const g_FsinFullRankNames[10][60] =
{
    "�������", "��������", "�������", "��������", "���������",
    "���������", "�������", "�����", "������������", "���������"
};
new const g_FsinFullRankWages[10] = {1950,2540,4500,5700,6325,7950,8700,10550,12000,16000};

// Current FSIN fleet � exact six-car configuration from fraction_gui_adapter.pwn.
new const g_FsinFullFleetModels[FSIN_FULL_FLEET_COUNT] =
{
    456, 413, 2555, 2547, 28664, 2562
};
new const g_FsinFullFleetMinRank[FSIN_FULL_FLEET_COUNT] =
{
    1, 2, 3, 8, 9, 5
};
new const g_FsinFullFleetNames[FSIN_FULL_FLEET_COUNT][48] =
{
    "�������",
    "��� 3221",
    "������",
    "LC 200",
    "Hyundai Grandeur",
    "UAZ Patriot"
};
new const g_FsinFullFleetVinyls[FSIN_FULL_FLEET_COUNT][32] =
{
    "remapfbody29",
    "remapfbody28",
    "remapfbody28",
    "remapfbody28",
    "remapfbody29",
    "remapfbody28"
};
new const Float:g_FsinFullFleetParking[2][4] =
{
    {-1849.768554,-2775.762207,6.923953,184.878997},
    {-1859.290893,-2768.127685,6.929805,184.878997}
};

new const Float:g_FsinFullJailPositions[4][4] =
{
    {-1699.121582,-2702.468261,1499.500000,180.0},
    {-1699.900756,-2700.589355,1499.500000,180.0},
    {-1698.703979,-2698.262695,1499.500000,180.0},
    {-1697.900000,-2701.500000,1499.500000,180.0}
};

new g_FsinFullGateKpp1 = -1;
new g_FsinFullGateKpp2 = -1;
new g_FsinFullGateInner = -1;
new g_FsinFullGateStatic = -1;
new g_FsinFullGateArea[2] = {-1,-1};
new bool:g_FsinFullGateOpen[2];
new g_FsinFullGateCloseTimer[2] = {-1,-1};

new g_FsinFullHrActor = -1;
new g_FsinFullIglaActor = -1;
new g_FsinFullHrHint[MAX_PLAYERS];

new g_FsinFullWardrobePickup = -1;
new g_FsinFullFleetPickup = -1;
new g_FsinFullArmoryPickup = -1;
new g_FsinFullEntrancePickup = -1;
new g_FsinFullExitPickup = -1;
new g_FsinFullBedArea[4] = {-1,...};
new g_FsinFullLazaretIn = -1;
new g_FsinFullLazaretOut = -1;
new g_FsinFullCameraIn = -1;
new g_FsinFullCameraOut = -1;
new g_FsinFullSawmillIn = -1;
new g_FsinFullSawmillOut = -1;
new const Float:g_FsinFullSawmillPos[FSIN_FULL_SAWMILL_COUNT][4] =
{
    {-1901.070800,-2597.180175,1501.040039,2.086538}, {-1898.064697,-2594.016845,1501.040039,351.237060},
    {-1904.850708,-2593.897949,1501.040039,2.610430}, {-1904.083251,-2600.031982,1501.040039,359.043304},
    {-1907.864868,-2597.151367,1501.040039,355.212921}, {-1910.035156,-2601.674316,1501.040039,9.400522},
    {-1913.193481,-2598.299804,1501.040039,4.191907}, {-1911.217285,-2593.904541,1501.040039,355.832550},
    {-1916.375122,-2594.287841,1501.040039,356.896240}, {-1917.415893,-2600.928466,1501.040039,4.236432}
};
new Text3D:g_FsinFullSawmillLabel[FSIN_FULL_SAWMILL_COUNT];
new g_FsinFullSawmillArea[FSIN_FULL_SAWMILL_COUNT];
new g_FsinFullSawmillUser[FSIN_FULL_SAWMILL_COUNT];
new g_FsinFullSawmillStation[MAX_PLAYERS];
new g_FsinFullSawmillDone[MAX_PLAYERS];
new PlayerText:g_FsinFullSawmillTD[MAX_PLAYERS][FSIN_FULL_SAWMILL_BOARDS + 4];
new bool:g_FsinFullSawmillTDReady[MAX_PLAYERS];

// --------------------------------------------------------------------------
// Initialization / scene
// --------------------------------------------------------------------------
stock FsinFull_CreateScene()
{
    CreateDynamic3DTextLabel("{FF5252}�\t{ffffff}������ �C��\t{FF5252}�\n{ffffff}��������� ��� ��������{FF5252}", 0xFF5252FF, -1823.55,-2633.54,10.38, 15.0);
    CreateDynamic3DTextLabel("{FF5252}�\t{ffffff}������ �C��\t{FF5252}�\n{ffffff}��������� ��� ��������{FF5252}", 0xFF5252FF, -1820.60,-2650.97,12.70, 10.0);

    g_FsinFullGateArea[0] = CreateDynamicSphere(-1823.66,-2633.54,8.58,10.0);
    g_FsinFullGateArea[1] = CreateDynamicSphere(-1820.49,-2651.36,10.90,15.0);

    g_FsinFullGateKpp1 = CreateObject(11611,-1827.21,-2634.129,8.58,0.0,0.0,99.4);
    g_FsinFullGateKpp2 = CreateObject(11612,-1820.11,-2632.95,8.58,0.0,0.0,-80.5);
    g_FsinFullGateInner = CreateObject(11615,-1820.49,-2651.36,10.9,0.0,0.0,99.5);
    g_FsinFullGateStatic = CreateObject(11614,-1778.2,-2709.2,10.9,0.0,0.0,142.0);

    // All four boundary objects found inside the FSIN exterior bbox in the donor.
    CreateObject(11781,-1776.81,-2731.33,7.5,0.0,0.0,233.0);
    CreateObject(11781,-1764.28,-2795.48,7.5,0.0,0.0,233.0);
    CreateObject(11781,-1683.55,-2812.77,7.5,0.0,0.0,51.0);
    CreateObject(11781,-1681.68,-2737.23,7.5,0.0,0.0,143.0);

    // Visible FSIN lazaret shell. The supplied coordinates are a custom interior,
    // so the room must be streamed explicitly in virtual world 6 / interior 1.
    CreateDynamicObject(19379,-2004.500000,-1492.000000,1750.850000,0.0,90.0,0.0,6,1);
    CreateDynamicObject(19379,-2004.500000,-1501.500000,1750.850000,0.0,90.0,0.0,6,1);
    CreateDynamicObject(19379,-2009.350000,-1492.000000,1753.250000,0.0,0.0,0.0,6,1);
    CreateDynamicObject(19379,-1999.650000,-1492.000000,1753.250000,0.0,0.0,0.0,6,1);
    CreateDynamicObject(19379,-2004.500000,-1486.750000,1753.250000,0.0,0.0,90.0,6,1);
    CreateDynamicObject(19379,-2004.500000,-1506.250000,1753.250000,0.0,0.0,90.0,6,1);
    CreateDynamicObject(19379,-2004.500000,-1492.000000,1755.650000,0.0,90.0,0.0,6,1);
    CreateDynamicObject(1800,-2005.465087,-1493.152709,1751.000000,0.0,0.0,50.449573,6,1);
    CreateDynamicObject(1800,-2005.371337,-1490.781982,1751.000000,0.0,0.0,327.187438,6,1);
    CreateDynamicObject(1800,-2005.318237,-1488.515625,1751.000000,0.0,0.0,86.815521,6,1);
    CreateDynamicObject(1800,-2001.401367,-1493.826416,1751.000000,0.0,0.0,265.736175,6,1);
    CreateDynamic3DTextLabel("{66CCFF}������� ����\n{FFFFFF}��������� � ����� ��� �������",0xFFFFFFFF,-2004.620239,-1492.151245,1752.200000,15.0,6,1);
    g_FsinFullHrActor = CreateActor(81,-1817.008300,-2631.807617,7.590855,2.969990);
    Create3DTextLabel("{ffff00}� ����� ������ ���� �\n{ffffff}��������� ��� ��������������", -1, -1817.008300,-2631.807617,7.590855, 15.0, 0);

    // Visual NPC/label present in the donor FSIN scene. No separate interaction
    // implementation for this actor was found in the source, so it is preserved as-is.
    g_FsinFullIglaActor = CreateActor(57,-1721.147827,-2815.024169,7.582671,132.815338);
    Create3DTextLabel("{ffff00}� ���� �\n{ffffff}��������� �����\n��� ��������������", -1, -1721.147827,-2815.024169,7.582671, 15.0, 0);
    Create3DTextLabel("{ffff00}����� ������\n{ffffff}��������� ����� ��� ��������������", -1, -1698.703979,-2698.262695,1499.500000, 15.0, 6);
    return 1;
}

stock FsinFull_CreatePickups()
{
    // Wardrobe / workday.
    g_FsinFullWardrobePickup = CreatePickup(1275,23,-1875.208618,-2722.175048,8.624625,0);
    CreateDynamic3DTextLabel("{FF5252}<<<����������>>>\n{FFFFFF}��������� ��� ��������������", 0x3399FFFF, -1875.208618,-2722.175048,9.424625,10.0);

    // Fleet/autopark.
    g_FsinFullFleetPickup = CreatePickup(19131,23,-1857.56,-2776.02,7.58,0);
    CreateDynamic3DTextLabel("{fcd000}(����)\n\n{FFFFFF}��������� ��� ��������������\n� ���������� �����������", -1, -1857.56,-2776.02,7.58,15.0);

    // Armory � exact donor position.
    g_FsinFullArmoryPickup = CreatePickup(1239,23,-1877.71,-2735.64,7.58,0);
    CreateDynamic3DTextLabel("{3399FF}- ��������� ����� -\n{FFFFFF}����", 0x3399FFFF, -1877.71,-2735.64,9.08,8.0);

    // FSIN lazaret beds use the common hospital interaction/healing system.
    g_FsinFullBedArea[0] = CreateDynamicSphere(-2005.465087,-1493.152709,1751.548583,2.0,6,1);
    g_FsinFullBedArea[1] = CreateDynamicSphere(-2005.371337,-1490.781982,1751.814208,2.0,6,1);
    g_FsinFullBedArea[2] = CreateDynamicSphere(-2005.318237,-1488.515625,1751.548583,2.0,6,1);
    g_FsinFullBedArea[3] = CreateDynamicSphere(-2001.401367,-1493.826416,1751.548583,2.0,6,1);

    // Main entrance: exterior world 0 <-> FSIN interior 1, virtual world 6.
    g_FsinFullEntrancePickup = CreatePickup(1318,23,-1747.629028,-2835.377197,8.033645,0);
    Create3DTextLabel("{FF5252}� ���� �� ���� �",-1,-1747.629028,-2835.377197,9.033645,15.0,0);
    g_FsinFullExitPickup = CreatePickup(1318,23,-1816.255615,-2735.155273,1501.040039,6);
    Create3DTextLabel("{FF5252}� ����� �� ���� �",-1,-1816.255615,-2735.155273,1502.040039,15.0,6);

    // Prison/lazaret/sawmill portals � exact donor pickups and labels.
    g_FsinFullLazaretIn = CreatePickup(1318,23,-1801.931030,-2757.826660,1501.000000,-1);
    g_FsinFullLazaretOut = CreatePickup(1318,23,-2003.557861,-1506.610107,1751.005249,-1);
    CreateDynamic3DTextLabel("����� �� ��������",0xFFFF00FF,-2003.557861,-1506.610107,1752.005249,10.0);

    g_FsinFullCameraIn = CreatePickup(19132,23,-1801.861206,-2766.176757,1501.000000,-1);
    CreateDynamic3DTextLabel("���� � ������",0xFFFF00FF,-1801.861206,-2766.176757,1502.000000,10.0);
    g_FsinFullCameraOut = CreatePickup(19132,23,-1697.535400,-2702.098876,1499.500000,-1);
    CreateDynamic3DTextLabel("����� �� ������",0xFFFF00FF,-1697.535400,-2702.098876,1500.500000,10.0);

    g_FsinFullSawmillIn = CreatePickup(1318,23,-1798.070190,-2759.390869,1501.000000,-1);
    CreateDynamic3DTextLabel("���� � ������� ����������",0xFFFF00FF,-1798.070190,-2759.390869,1502.000000,10.0);
    g_FsinFullSawmillOut = CreatePickup(1318,23,-1887.860717,-2609.774902,1501.233764,-1);
    CreateDynamic3DTextLabel("����� �� ������� ����������",0xFFFF00FF,-1887.860717,-2609.774902,1502.233764,10.0);
    for(new i; i < FSIN_FULL_SAWMILL_COUNT; i++)
    {
        g_FsinFullSawmillUser[i] = INVALID_PLAYER_ID;
        g_FsinFullSawmillArea[i] = CreateDynamicSphere(g_FsinFullSawmillPos[i][0],g_FsinFullSawmillPos[i][1],g_FsinFullSawmillPos[i][2],2.5,6,1);
        g_FsinFullSawmillLabel[i] = CreateDynamic3DTextLabel("{D99B45}����������� ������\n{FFFFFF}����������� ������ ��������������",-1,g_FsinFullSawmillPos[i][0],g_FsinFullSawmillPos[i][1],g_FsinFullSawmillPos[i][2]+0.65,12.0,INVALID_PLAYER_ID,INVALID_VEHICLE_ID,0,6,1);
    }
    return 1;
}

forward FsinFull_ProximityTick();
public FsinFull_ProximityTick()
{
    foreach(new playerid : Player)
    {
        if(!IsPlayerLogged(playerid)) continue;

        new bool:hr_near = false;
        if(GetPlayerTeamEx(playerid) == TEAM_NONE && !IsPlayerInAnyVehicle(playerid) && !GetPVarInt(playerid,"fsin_full_hr_busy"))
        {
            if(GetPlayerVirtualWorld(playerid) == 0 && IsPlayerInRangeOfPoint(playerid,4.5,-1817.008300,-2631.807617,7.590855)) hr_near = true;
        }

        if(hr_near)
        {
            ToggleInteractionWindow(playerid,true);
            SetPVarInt(playerid,"sime_context_hint",1);
            if(!g_FsinFullHrHint[playerid])
            {
                g_FsinFullHrHint[playerid] = 1;
            }
        }
        else if(g_FsinFullHrHint[playerid])
        {
            g_FsinFullHrHint[playerid] = 0;
            if(GetPVarInt(playerid,"sime_context_hint") == 1) DeletePVar(playerid,"sime_context_hint");
            ToggleInteractionWindow(playerid,false);
        }

        if(GetPlayerTeamEx(playerid) == FSIN_FULL_TEAM_ID && !IsPlayerInAnyVehicle(playerid) && FsinFull_GateGetNearby(playerid,14.0) != -1)
        {
            SetPVarInt(playerid,"sime_context_hint",3);
            ToggleInteractionWindow(playerid,true);
        }
    }
    return 1;
}

stock FsinFull_Init()
{
    for(new i; i < MAX_PLAYERS; i++)
    {
        g_FsinFullHrHint[i] = 0;
        g_FsinFullSawmillStation[i] = -1;
        g_FsinFullSawmillDone[i] = 0;
        g_FsinFullSawmillTDReady[i] = false;
    }
    new Cache:column_check = mysql_query(mysql,
        "SELECT COUNT(*) AS amount FROM information_schema.COLUMNS WHERE TABLE_SCHEMA=DATABASE() AND TABLE_NAME='accounts' AND COLUMN_NAME='jail_fsin'", true);
    new has_column = cache_get_field_content_int(0,"amount");
    cache_delete(column_check);
    if(!has_column)
        mysql_query(mysql,"ALTER TABLE accounts ADD COLUMN jail_fsin INT NOT NULL DEFAULT 0",false);
    new Cache:wanted_column_check = mysql_query(mysql,
        "SELECT COUNT(*) AS amount FROM information_schema.COLUMNS WHERE TABLE_SCHEMA=DATABASE() AND TABLE_NAME='accounts' AND COLUMN_NAME='jail_wanted_level'", true);
    new has_wanted_column = cache_get_field_content_int(0,"amount");
    cache_delete(wanted_column_check);
    if(!has_wanted_column)
        mysql_query(mysql,"ALTER TABLE accounts ADD COLUMN jail_wanted_level TINYINT UNSIGNED NOT NULL DEFAULT 0",false);
    FsinFull_CreateScene();
    FsinFull_CreatePickups();
    SetTimer("FsinFull_ProximityTick",500,true);
    return 1;
}

// --------------------------------------------------------------------------
// Gates
// --------------------------------------------------------------------------
stock FsinFull_GateGetNearby(playerid, Float:radius = 13.0)
{
    if(IsPlayerInRangeOfPoint(playerid,radius,-1823.66,-2633.54,8.58)) return 0;
    if(IsPlayerInRangeOfPoint(playerid,radius,-1820.49,-2651.36,10.90)) return 1;
    return -1;
}

forward FsinFull_GateClose(gate);
public FsinFull_GateClose(gate)
{
    if(gate < 0 || gate > 1) return 0;
    g_FsinFullGateCloseTimer[gate] = -1;
    g_FsinFullGateOpen[gate] = false;
    if(gate == 0)
    {
        MoveObject(g_FsinFullGateKpp2,-1820.11,-2632.95,8.58,2.0,0.0,0.0,-80.5);
        MoveObject(g_FsinFullGateKpp1,-1827.21,-2634.129,8.58,2.0,0.0,0.0,99.4);
    }
    else MoveObject(g_FsinFullGateInner,-1820.49,-2651.36,10.9,5.0,0.0,0.0,99.5);
    return 1;
}

stock FsinFull_GateOpen(playerid, gate)
{
    if(gate < 0 || gate > 1) return 0;
    if(GetPlayerTeamEx(playerid) != FSIN_FULL_TEAM_ID)
    {
        ShowNotificationNew(playerid,2,5,-1,-1,"������ � ������� �������� ������ ����������� ����.","");
        return 1;
    }
    if(g_FsinFullGateCloseTimer[gate] != -1)
    {
        KillTimer(g_FsinFullGateCloseTimer[gate]);
        g_FsinFullGateCloseTimer[gate] = -1;
    }
    if(gate == 0)
    {
        MoveObject(g_FsinFullGateKpp2,-1820.11,-2632.95,8.58,3.0,0.0,0.0,15.0);
        MoveObject(g_FsinFullGateKpp1,-1827.21,-2634.129,8.58,3.0,0.0,0.0,15.0);
    }
    else MoveObject(g_FsinFullGateInner,-1828.0,-2652.7,10.9,5.0,0.0,0.0,99.5);

    g_FsinFullGateOpen[gate] = true;
    g_FsinFullGateCloseTimer[gate] = SetTimerEx("FsinFull_GateClose",7000,false,"i",gate);
    ShowNotificationNew(playerid,3,4,-1,-1,"������ ���� �������.","");
    return 1;
}

stock FsinFull_OnEnterDynamicArea(playerid, areaid)
{
    for(new sawmill; sawmill < FSIN_FULL_SAWMILL_COUNT; sawmill++) if(areaid == g_FsinFullSawmillArea[sawmill])
    {
        if(GetPlayerData(playerid,P_JAILFSIN) > 0 && !IsPlayerInAnyVehicle(playerid))
        {
            SetPVarInt(playerid,"sime_context_hint",4);
            ToggleInteractionWindow(playerid,true);
        }
        return 1;
    }
    new gate = -1;
    if(areaid == g_FsinFullGateArea[0]) gate = 0;
    else if(areaid == g_FsinFullGateArea[1]) gate = 1;
    else return 0;

    if(GetPlayerTeamEx(playerid) != FSIN_FULL_TEAM_ID)
    {
        ShowNotificationNew(playerid,2,5,-1,-1,"�� �� ��������� ����.","");
        return 1;
    }
    if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
        ShowNotificationNew(playerid,4,6,-1,-1,"�����������, ����� ������� ������ ����.","");
    else
    {
        SetPVarInt(playerid,"sime_context_hint",3);
        ToggleInteractionWindow(playerid,true);
        ShowNotificationNew(playerid,4,6,FSIN_FULL_GATE_NOTIFY_ID,gate,"������ ����","�������");
    }
    return 1;
}

stock FsinFull_OnLeaveDynamicArea(playerid, areaid)
{
    for(new sawmill; sawmill < FSIN_FULL_SAWMILL_COUNT; sawmill++) if(areaid == g_FsinFullSawmillArea[sawmill])
    {
        if(GetPVarInt(playerid,"sime_context_hint") == 4) DeletePVar(playerid,"sime_context_hint");
        ToggleInteractionWindow(playerid,false);
        if(g_FsinFullSawmillStation[playerid] == sawmill) FsinFull_StopSawmill(playerid,true);
        return 1;
    }
    if(areaid != g_FsinFullGateArea[0] && areaid != g_FsinFullGateArea[1]) return 0;
    if(GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
    {
        if(GetPVarInt(playerid,"sime_context_hint") == 3) DeletePVar(playerid,"sime_context_hint");
        ToggleInteractionWindow(playerid,false);
    }
    return 1;
}

stock FsinFull_GateInteraction(playerid)
{
    if(!IsPlayerConnected(playerid) || GetPlayerTeamEx(playerid) != FSIN_FULL_TEAM_ID) return 0;
    if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER) return 0;
    new gate = FsinFull_GateGetNearby(playerid);
    if(gate == -1) return 0;
    if(GetPVarInt(playerid,"sime_context_hint") == 3) DeletePVar(playerid,"sime_context_hint");
    ToggleInteractionWindow(playerid,false);
    return FsinFull_GateOpen(playerid,gate);
}

stock FsinFull_GateHandleKeys(playerid,newkeys,oldkeys)
{
    if(!IsPlayerConnected(playerid) || GetPlayerTeamEx(playerid) != FSIN_FULL_TEAM_ID) return 0;
    new gate = FsinFull_GateGetNearby(playerid);
    if(gate == -1) return 0;
    if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
    {
        if((newkeys & KEY_CROUCH) && !(oldkeys & KEY_CROUCH)) return FsinFull_GateOpen(playerid,gate);
    }
    else if((newkeys & KEY_YES) && !(oldkeys & KEY_YES)) return FsinFull_GateOpen(playerid,gate);
    return 0;
}

// --------------------------------------------------------------------------
// HR / recruitment
// -------------------------------
