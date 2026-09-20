#if defined _NAVALNY_EASTER_INCLUDED
    #endinput
#endif
#define _NAVALNY_EASTER_INCLUDED

#define NAVALNY_EGG_WORLD (700147)
#define NAVALNY_EGG_INTERIOR (1)
#define DIALOG_NAVALNY_EGG_GIFT (22403)
#define DIALOG_NAVALNY_EGG_MATVEI (22404)

new g_NavalnyEggActor = -1;
new g_MatveiEggActor = -1;
new g_NavalnyEggArea = -1;
new g_MatveiEggArea = -1;

stock NavalnyEgg_Init()
{
    mysql_query(mysql,
        "CREATE TABLE IF NOT EXISTS navalny_easter_rewards (account_id INT NOT NULL, claimed_at INT NOT NULL DEFAULT 0, PRIMARY KEY(account_id)) ENGINE=InnoDB DEFAULT CHARSET=utf8",
        false);

    g_MatveiEggActor = CreateActor(5500086,-2.053030,2004.976684,1554.243164,206.693130);
    g_NavalnyEggActor = CreateActor(5500082,1.907594,2004.799682,1554.243164,126.194793);
    if(g_MatveiEggActor != -1) SetActorVirtualWorld(g_MatveiEggActor,NAVALNY_EGG_WORLD);
    if(g_NavalnyEggActor != -1) SetActorVirtualWorld(g_NavalnyEggActor,NAVALNY_EGG_WORLD);

    CreateDynamic3DTextLabel("{FFFF00}Ìàòâåé Áîäÿê\n{FFFFFF}Ïîäîéäèòå äëÿ âçàèìîäåéñòâèÿ",-1,
        -2.053030,2004.976684,1555.243164,8.0,_,_,_,NAVALNY_EGG_WORLD,NAVALNY_EGG_INTERIOR);
    CreateDynamic3DTextLabel("{FFFF00}Àëåêñåé Íàâàëüíûé\n{FFFFFF}Ïîäîéäèòå äëÿ âçàèìîäåéñòâèÿ",-1,
        1.907594,2004.799682,1555.243164,8.0,_,_,_,NAVALNY_EGG_WORLD,NAVALNY_EGG_INTERIOR);

    g_MatveiEggArea = CreateDynamicSphere(-2.053030,2004.976684,1554.243164,2.5,NAVALNY_EGG_WORLD,NAVALNY_EGG_INTERIOR);
    g_NavalnyEggArea = CreateDynamicSphere(1.907594,2004.799682,1554.243164,2.5,NAVALNY_EGG_WORLD,NAVALNY_EGG_INTERIOR);
    return 1;
}

stock bool:NavalnyEgg_IsNearActor(playerid, actor_type)
{
    if(GetPlayerVirtualWorld(playerid) != NAVALNY_EGG_WORLD || GetPlayerInterior(playerid) != NAVALNY_EGG_INTERIOR) return false;
    if(actor_type == 1) return IsPlayerInRangeOfPoint(playerid,3.0,1.907594,2004.799682,1554.243164);
    return IsPlayerInRangeOfPoint(playerid,3.0,-2.053030,2004.976684,1554.243164);
}

stock NavalnyEgg_OnEnterArea(playerid, areaid)
{
    if(areaid != g_NavalnyEggArea && areaid != g_MatveiEggArea) return 0;
    ToggleInteractionWindow(playerid,true);
    return 1;
}

stock NavalnyEgg_OnLeaveArea(playerid, areaid)
{
    if(areaid != g_NavalnyEggArea && areaid != g_MatveiEggArea) return 0;
    if(!NavalnyEgg_IsNearActor(playerid,0) && !NavalnyEgg_IsNearActor(playerid,1))
        ToggleInteractionWindow(playerid,false);
    return 1;
}

stock NavalnyEgg_ShowMatvei(playerid, stage)
{
    SetPVarInt(playerid,"navalny_matvei_stage",stage);
    switch(stage)
    {
        case 0: Dialog(playerid,DIALOG_NAVALNY_EGG_MATVEI,DIALOG_STYLE_MSGBOX,"{FFFF00}Ìàòâåé Áîäÿê",
            "{FFFFFF}Ïåðåä Âàìè ñèäèò Ìàòâåé Áîäÿê.", "Êàê òåáÿ çîâóò?", "Óéòè");
        case 1: Dialog(playerid,DIALOG_NAVALNY_EGG_MATVEI,DIALOG_STYLE_MSGBOX,"{FFFF00}Ìàòâåé",
            "{FFFFFF}Ìàòâåé.", "Ïîëíîñòüþ", "Óéòè");
        case 2: Dialog(playerid,DIALOG_NAVALNY_EGG_MATVEI,DIALOG_STYLE_MSGBOX,"{FFFF00}Ìàòâåé",
            "{FFFFFF}Ìàòâåé Áîäÿê Ðåâîëüâåðîâè÷.", "Ãäå òû æèâåøü?", "Óéòè");
        case 3: Dialog(playerid,DIALOG_NAVALNY_EGG_MATVEI,DIALOG_STYLE_MSGBOX,"{FFFF00}Ìàòâåé",
            "{FFFFFF}Ðîñòîâñêàÿ 19á.", "À çà÷åì òû ñþäà çàëåç?", "Óéòè");
        case 4: Dialog(playerid,DIALOG_NAVALNY_EGG_MATVEI,DIALOG_STYLE_MSGBOX,"{FFFF00}Ìàòâåé",
            "{FFFFFF}Íå çíàþ, äðóçüÿ ãîâîðèëè ÷òî çäåñü ìîæíî ïîëàçèòü.", "À âû ÷òî-íèáóäü óêðàëè?", "Óéòè");
        case 5: Dialog(playerid,DIALOG_NAVALNY_EGG_MATVEI,DIALOG_STYLE_MSGBOX,"{FFFF00}Ìàòâåé",
            "{FFFFFF}×åñòíî íåò.", "Õîðîøî, Ìàòâåé, ïîøåë íàõóé", "Óéòè");
    }
    return 1;
}

stock NavalnyEgg_TryClaim(playerid)
{
    new account_id = GetPlayerAccountID(playerid);
    if(account_id <= 0) return SendClientMessage(playerid,0xCECECEFF,"Ñíà÷àëà íåîáõîäèìî àâòîðèçîâàòüñÿ"),1;

    new query[180];
    mysql_format(mysql,query,sizeof query,
        "INSERT IGNORE INTO navalny_easter_rewards (account_id,claimed_at) VALUES (%d,%d)",account_id,gettime());
    new Cache:result = mysql_query(mysql,query,true);
    new error = mysql_errno();
    new affected = error ? 0 : cache_affected_rows();
    cache_delete(result);

    if(error) return SendClientMessage(playerid,0xCECECEFF,"Íå óäàëîñü ñîõðàíèòü ïîëó÷åíèå ïîäàðêà"),1;
    if(affected != 1) return SendClientMessage(playerid,0xCECECEFF,"Âû óæå çàáèðàëè ïîäàðîê èç ýòîãî ëîãîâà"),1;

    GivePlayerMoneyEx(playerid,1488,"Ïàñõàëêà ãàðàæà Íàâàëüíîãî",true,true);
    SendClientMessage(playerid,0x66CC00FF,"Äåðæè ïîäàðî÷åê)");
    return 1;
}

stock NavalnyEgg_OnInteract(playerid)
{
    if(NavalnyEgg_IsNearActor(playerid,1))
    {
        ToggleInteractionWindow(playerid,false);
        Dialog(playerid,DIALOG_NAVALNY_EGG_GIFT,DIALOG_STYLE_MSGBOX,"{FFFF00}Àëåêñåé Íàâàëüíûé",
            "{FFFFFF}Ïðèâåò ýòî Íàâàëüíûé è ÿ òåïåðü èãðàþ íà ïðîåêòå "SERVER_NAME"!\nÈ ÿ õî÷ó òåáå ñäåëàòü íåáîëüøîé ïîäàðîê çà òî ÷òî íàøåë ìîå ëîãîâî))))",
            "Ïîëó÷èòü","Óéòè");
        return 1;
    }
    if(NavalnyEgg_IsNearActor(playerid,0))
    {
        ToggleInteractionWindow(playerid,false);
        return NavalnyEgg_ShowMatvei(playerid,0);
    }
    return 0;
}

stock NavalnyEgg_OnDialogResponse(playerid,dialogid,response)
{
    if(dialogid == DIALOG_NAVALNY_EGG_GIFT)
    {
        if(response) NavalnyEgg_TryClaim(playerid);
        return 1;
    }
    if(dialogid == DIALOG_NAVALNY_EGG_MATVEI)
    {
        if(!response)
        {
            DeletePVar(playerid,"navalny_matvei_stage");
            return 1;
        }
        new stage = GetPVarInt(playerid,"navalny_matvei_stage");
        if(stage >= 5)
        {
            DeletePVar(playerid,"navalny_matvei_stage");
            SendClientMessage(playerid,0xCECECEFF,"Õîðîøî Ìàòâåé ïîøåë íàõóé");
            return 1;
        }
        return NavalnyEgg_ShowMatvei(playerid,stage+1);
    }
    return 0;
}

stock NavalnyEgg_OnDisconnect(playerid)
{
    DeletePVar(playerid,"navalny_matvei_stage");
    return 1;
}
